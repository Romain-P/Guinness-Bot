package com.guiness.bot.log

import java.io.PrintStream
import java.io.PrintWriter
import java.io.Writer
import java.time.Clock
import java.time.ZoneId
import java.time.format.DateTimeFormatter
import java.time.temporal.Temporal
import java.util.*


val WS_re = Regex.fromLiteral("(s+)")

object Log {
    enum class LoggingLevel {
        DEBUG,
        INFO,
        GOOD,
        BAD,
        WARN,
        ERROR,
        TODO,
    }

    class LoggerWriter(val log: Logger, val level: LoggingLevel) : Writer() {
        var closed = false

        override fun write(cbuf: CharArray, off: Int, len: Int) {
            if (!closed)
                log.log(level, cbuf.slice(IntRange(off, off + len)).toString())
        }

        override fun flush() {
        }

        override fun close() {
            closed = true
        }

    }

    interface Logger {
        val name: String
        val context: Map<String, Any>

        fun log(level: LoggingLevel, msg: String, vararg args: Any)
        fun format(msg: String, vararg args: Any): String
        fun newLogger(name: String, context: Map<String, Any>): Logger

        fun debug(msg: String, vararg args: Any) = log(LoggingLevel.DEBUG, msg, *args)
        fun info(msg: String, vararg args: Any) = log(LoggingLevel.INFO, msg, *args)
        fun good(msg: String, vararg args: Any) = log(LoggingLevel.GOOD, msg, *args)
        fun bad(msg: String, vararg args: Any) = log(LoggingLevel.BAD, msg, *args)
        fun warn(msg: String, vararg args: Any) = log(LoggingLevel.WARN, msg, *args)
        fun error(msg: String, vararg args: Any, exc: (nsg: String) -> Exception = ::RuntimeException) {
            log(LoggingLevel.ERROR, msg, *args)
            throw exc(format(msg, *args))
        }

        fun todo(msg: String, vararg args: Any) {
            log(LoggingLevel.TODO, msg, *args)
            throw NotImplementedError(format(msg, *args))
        }

        fun writer(level: LoggingLevel) = LoggerWriter(this, level)
        fun printWriter(level: LoggingLevel) = PrintWriter(writer(level))

        fun moreContext(vararg additionnalContext: Pair<String, Any>) =
            newLogger(name, context + additionnalContext)

        fun appendName(append: String) =
            newLogger("$name/$append", context)
    }

    class StdLogger(
        val out: PrintStream,
        override val name: String,
        override val context: Map<String, Any>,
        val clock: Clock = Clock.systemUTC()
    ) : Logger {
        companion object {
            fun formatLevel(l: LoggingLevel) = when (l) {
                LoggingLevel.DEBUG -> "[*]"
                LoggingLevel.INFO -> "[.]"
                LoggingLevel.GOOD -> "[+]"
                LoggingLevel.BAD -> "[-]"
                LoggingLevel.WARN -> "[#]"
                LoggingLevel.ERROR -> "[!]"
                LoggingLevel.TODO -> "[>]"
            }
        }

        val contextDisplay by lazy { context.entries.joinToString(", ", "", "") { "${it.key}=${it.value}" } }

        override fun log(level: LoggingLevel, msg: String, vararg args: Any) {
            val time = clock.instant().toHttpDateString()

            val text = format(
                "%s -- %s %s $contextDisplay ${escapeControlSequences(msg)}",
                time,
                name,
                formatLevel(level),
                *args
            )
            out.println(text)
        }

        fun escapeControlSequences(msg: String) =
            buildString(msg.length) {
                val matches = WS_re.matchEntire(this) ?: return msg

                var last = 0
                while (last < msg.length) {
                    val match = matches.next()
                    if (match != null) {
                        append(msg.substring(last, match.range.first))
                        append("\\" + match.value)

                        last = match.range.last
                    }
                }

                append(msg.substring(lastIndex))
            }

        override fun format(msg: String, vararg args: Any) = String.format(msg, *args)

        override fun newLogger(name: String, context: Map<String, Any>) = StdLogger(out, name, context)
    }

    enum class StdLoggerType { Stdout, Stderr, Syslog }

    class StdLoggerFactory(val loggerType: StdLoggerType = StdLoggerType.Stdout) : LoggerFactory {
        val printStream: PrintStream = run {
            when (loggerType) {
                StdLoggerType.Stdout -> System.out
                StdLoggerType.Stderr -> System.err
                StdLoggerType.Syslog -> throw NotImplementedError("syslog not supported yet")
            }
        }

        override fun create(name: String, context: Map<String, Any>) = StdLogger(printStream, name, context)
    }

    interface LoggerFactory {
        fun create(name: String, context: Map<String, Any>): Logger
    }
}

inline fun <reified T> T.logger(
    factory: Log.LoggerFactory = Log.StdLoggerFactory(),
    vararg context: Pair<String, Any> = emptyArray()
) = lazy {
    factory.create(T::class.simpleName ?: "", mapOf(*context))
}

fun Temporal.toHttpDateString(): String = httpDateFormat.format(this)

val httpDateFormat: DateTimeFormatter = DateTimeFormatter
    .ofPattern("EEE, dd MMM yyyy HH:mm:ss z")
    .withLocale(Locale.US)
    .withZone(ZoneId.of("GMT"))!!