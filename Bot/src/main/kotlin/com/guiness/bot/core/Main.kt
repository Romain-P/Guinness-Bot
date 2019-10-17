package com.guiness.bot.core

import com.guiness.bot.netwotk.Proxy


class Main

fun main(args: Array<String>) {
    //NativeAPI.patchProxyPort(5555)

    Proxy.withHost("127.0.0.1", 5555)
        .withWorker(1, 4)
        .start()
}

/**
import com.guiness.bot.external.NativeAPI
import io.ktor.http.toHttpDateString
import io.ktor.network.selector.ActorSelectorManager
import io.ktor.network.sockets.*
import io.ktor.util.InternalAPI
import io.ktor.util.KtorExperimentalAPI
import io.ktor.util.moveTo
import io.ktor.util.moveToByteArray
import io.netty.handler.codec.DelimiterBasedFrameDecoder
import kotlinx.coroutines.*
import kotlinx.coroutines.io.ByteWriteChannel
import kotlinx.coroutines.io.close
import kotlinx.coroutines.io.readUntilDelimiter
import kotlinx.coroutines.io.writeStringUtf8
import kotlinx.coroutines.time.delay
import java.io.ByteArrayOutputStream
import java.io.PrintStream
import java.io.PrintWriter
import java.io.Writer
import java.lang.Exception
import java.lang.Runnable
import java.net.InetSocketAddress
import java.nio.Buffer
import java.nio.ByteBuffer
import java.time.Clock
import java.time.Duration
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

interface ByteBufferAllocator {
    fun malloc(bufsize: Int): ByteBuffer
    fun free(buf: ByteBuffer)
}

object HeapByteBufferAllocator : ByteBufferAllocator {
    override fun malloc(bufsize: Int): ByteBuffer = ByteBuffer.allocate(bufsize)
    override fun free(buf: ByteBuffer) {}
}

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

val WS_re = Regex.fromLiteral("(s+)")

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

class StdLoggerFactory(val loggerType: StdLoggerType = StdLoggerType.Stdout) :
    LoggerFactory {
    val printStream: PrintStream = run {
        when (loggerType) {
            StdLoggerType.Stdout -> System.out
            StdLoggerType.Stderr -> System.err
            StdLoggerType.Syslog -> throw NotImplementedError("syslog not supported yet")
        }
    }

    override fun create(name: String, context: Map<String, Any>) =
        StdLogger(printStream, name, context)
}

interface LoggerFactory {
    fun create(name: String, context: Map<String, Any>): Logger
}

inline fun <reified T> T.logger(
    factory: LoggerFactory = StdLoggerFactory(),
    vararg context: Pair<String, Any> = emptyArray()
) = lazy {
    factory.create(T::class.simpleName ?: "", mapOf(*context))
}

val exec: ExecutorService = Executors.newCachedThreadPool()
@KtorExperimentalAPI
val selector = ActorSelectorManager(exec.asCoroutineDispatcher())

@KtorExperimentalAPI
fun aSocket(): SocketBuilder {
    return aSocket(selector)
}

suspend inline fun <T> repeat(crossinline fn: suspend () -> T?): T {
    while (true) {
        return fn() ?: continue
    }
}

val NL: ByteBuffer = ByteBuffer.wrap(byteArrayOf(0x0))
val LFNL: ByteBuffer =  ByteBuffer.wrap(byteArrayOf(0xa, 0))
val LF: ByteBuffer =  ByteBuffer.wrap(byteArrayOf(0xa))
val CRLF: ByteBuffer =  ByteBuffer.wrap(byteArrayOf(0xd, 0xa))

@UseExperimental(InternalAPI::class)
suspend fun readPacket(buffer: ByteBuffer, packetBuffer: ByteBuffer, delim: ByteBuffer, readPosition: Int): Int {
    val len = buffer.position() + 1

    var nextReadPosition = readPosition
    var delimIndex = 0
    val toReach = delim.limit() - 1
    var endIndex: Int = -1

    for (i in readPosition .. len) {
        val byte = buffer[i]

        if (delimIndex == toReach) {
            endIndex = i
            break
        }
        else if (byte == delim[delimIndex])
            delimIndex++
        else
            delimIndex = 0
        ++nextReadPosition
    }

    if (endIndex == -1) return nextReadPosition

    val flipped = buffer.slice()
    flipped.moveTo(packetBuffer, endIndex - delim.limit())
    buffer.position(endIndex + 1)

    return 0
}

suspend fun copy(
    from: Socket, to: Socket, delim: ByteArray,
    allocator: ByteBufferAllocator = HeapByteBufferAllocator,
    bufsize: Int = 256,
    onreceive: suspend ByteWriteChannel.(buf: ByteBuffer) -> Unit
) {
    val input = from.openReadChannel()
    val output = to.openWriteChannel()
    var cursize = bufsize
    var buf = allocator.malloc(bufsize)
    var packet = StringBuilder
    var readPosition = 0

    try {
        reading@ while (!from.isClosed) {
            if (!buf.hasRemaining()) {
                cursize *= 2
            }
            when (input.readFully(buf)) {
                0 -> continue@reading
                bufsize -> continue@reading // only received part of packet
            }

            readPosition = readPacket(buf, packet, delim, readPosition)

            if (packet.position() == 0) continue

            output.onreceive(packet)
            output.flush()
            buf = buf.slice()
        }
    }
    catch(e: Exception) {}
    finally {
        allocator.free(buf)
        from.awaitClosed()
        output.close()
    }
}

fun onShutdown(fn: () -> Unit) {
    Runtime.getRuntime().addShutdownHook(Thread(Runnable(fn)))
}

@KtorExperimentalAPI
fun main(args: Array<String>) = runBlocking(Dispatchers.Default) {
    NativeAPI.patchProxyPort(5555)
    BotManager.connect(ProfileManager.getDefaultProfile().accounts.values.toList())

    val log by logger()
    val upaddr = InetSocketAddress("34.251.172.139", 443)
    val bindAddr = InetSocketAddress("127.0.0.1", 5555)
    val msgcharset = Charsets.UTF_8

    log.good("Hello, World!")

    val sock = aSocket().tcp().bind(bindAddr) {
        //        reuseAddress = true
//        reusePort = true
        typeOfService = TypeOfService.IPTOS_RELIABILITY
    }

    onShutdown {
        sock.close()
    }

    log.info("listening %s:%s", bindAddr.hostName, bindAddr.port)

    while (!sock.isClosed) {
        // connection to Dofus client
        val downstream = sock.accept()
        val downaddr = downstream.remoteAddress as InetSocketAddress

        launch {
            val log = log.moreContext("ip" to downaddr.address.toString())

            // connection to Dofus server
            val upstream = repeat {
                try {
                    aSocket().tcp().connect(upaddr)
                } catch (exc: Exception) {
                    log.error("retry to connect to %s:%s in 3s", upaddr.hostName, upaddr.port)
                    delay(Duration.ofSeconds(3))
                    null
                }
            }

            val upAddr = upstream.remoteAddress as InetSocketAddress
            val uplog = log.moreContext("ip" to upAddr.address.toString())

            log.good(
                "connected to upstream %s:%s",
                upaddr.hostName, upaddr.port
            )

            val downjob = async {
                log.debug("start copying downstrean to upstream")
                copy(downstream, upstream, LFNL) {
                    val packet = msgcharset.decode(it).toString()
                    uplog.debug("RECV[%04d] %s %s", it.limit(), packet, Thread.currentThread().name)
                    if (!packet.startsWith(".{")) {
                        writeStringUtf8(packet) // it could be writeFully() but we need to it.flip() before because .decode consumes the buffer
                        writeFully(ByteBuffer.wrap(LFNL))
                    }
                }
                log.info("downstream closed")
            }

            val upjob = async {
                log.debug("start copying upstream to downstream")
                copy(upstream, downstream, NL) {
                    val packet = msgcharset.decode(it).toString()
                    uplog.debug("SENT[%04d] %s %s", it.limit(), packet, Thread.currentThread().name)
                    writeStringUtf8(packet) // it could be writeFully() but we need to it.flip() before because .decode consumes the buffer
                    writeFully((ByteBuffer.wrap(NL)))
                }
                log.info("upstream closed")
            }

            try {
                joinAll(upjob, downjob)

                log.info(
                    "gracefully closed",
                    downaddr.hostName, downaddr.port,
                    upaddr.hostName, upaddr.port
                )
            } catch (exc: Exception) {
                exc.printStackTrace(log.printWriter(LoggingLevel.ERROR))
            }
        }
    }

    log.good("See ya!")
}
        **/