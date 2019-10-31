package com.guiness.bot.netwotk

import com.guiness.bot.core.*
import com.guiness.bot.protocol.DofusProtocol
import com.guiness.bot.protocol.annotations.StreamSource
import com.guiness.bot.protocol.utf8
import io.netty.channel.ChannelOption
import io.netty.handler.codec.DelimiterBasedFrameDecoder
import io.netty.handler.codec.string.LineEncoder
import io.netty.handler.codec.string.LineSeparator
import io.netty.handler.codec.string.StringDecoder
import io.netty.handler.codec.string.StringEncoder
import org.reactivestreams.Publisher
import reactor.core.publisher.Flux
import reactor.netty.Connection
import reactor.netty.DisposableServer
import reactor.netty.NettyInbound
import reactor.netty.NettyOutbound
import reactor.netty.resources.LoopResources
import reactor.netty.tcp.TcpClient
import reactor.netty.tcp.TcpServer
import kotlin.concurrent.thread


object Proxy {
    private lateinit var server: DisposableServer
    private var config: TcpServer
    private val clients: MutableMap<ChannelID, ProxyClientContext>
    private val logFactory by logger()
    private val log = logFactory.appendName("tcp")

    init {
        clients = HashMap()

        config = TcpServer.create()
            .option(ChannelOption.SO_REUSEADDR, true)
            .doOnBound {
                /** TODO: remove when GUI done **/
                NativeAPI.patchProxyPort(it.port())
                BotManager.connect(ProfileManager.getDefaultProfile().accounts.values.toList())
            }
            .doOnConnection {
                addHandlers(it, downstream = true)
                onConnect(it)
            }
            .handle(::downstreamHandler)
    }

    private fun downstreamHandler(inbound: NettyInbound, outbound: NettyOutbound): Publisher<Void> {
        inbound.withConnection {con ->
            con.inbound().receiveObject().subscribe {
                ProxyMessageHandler.onDownstreamReceive(clients[con.channel().id().asLongText()]!!, it as String)
            }
        }
        return Flux.never()
    }

    fun upstreamHandler(inbound: NettyInbound, ctx: ProxyClientContext): Publisher<Void> {
        inbound.withConnection {con ->
            con.inbound().receiveObject().subscribe {
                ProxyMessageHandler.onUpstreamReceive(ctx, it as String)
            }
        }
        return Flux.never()
    }

    private fun onConnect(connection: Connection) {
        val ctx = ProxyClientContext.of(StreamSource.DOWNSTREAM, connection)
        clients[connection.channel().id().asLongText()] = ctx
        log("DOWNSTREAM", "CONNECTED", Log.LoggingLevel.GOOD)

        connection.onDispose {
            log("DOWNSTREAM", "DISCONNECTED", Log.LoggingLevel.BAD)
            ctx.upstreamMightBeNull()?.close()
            ctx.state = ProxyClientState.DISCONNECTED
            clients.remove(ctx.uuid())
        }
    }

    fun connectToUpstream(ctx: ProxyClientContext, ip: String, port: Int) {
        TcpClient.create()
            .host(ip)
            .port(port)
            .doOnConnected {
                addHandlers(it, upstream = true)
                ctx.attach(it)
                log("UPSTREAM", "CONNECTED", Log.LoggingLevel.GOOD)
            }
            .doOnDisconnected {
                log("UPSTREAM", "DISCONNECTED", Log.LoggingLevel.BAD)
                ctx.downstream().close()
            }
            .handle { inbound, _ -> upstreamHandler(inbound, ctx) }
            .connect()
            .subscribe()
    }

    private fun addHandlers(connection: Connection, upstream: Boolean = false, downstream: Boolean = false) {
        val decoder = if (upstream) DofusProtocol.SERVER_DELIMITER else DofusProtocol.CLIENT_DELIMITER
        val encoder = if (upstream) DofusProtocol.CLIENT_DELIMITER else DofusProtocol.SERVER_DELIMITER

        connection.addHandlerFirst(DelimiterBasedFrameDecoder(Integer.MAX_VALUE, decoder))
        connection.addHandlerLast(StringDecoder(Charsets.UTF_8))
        connection.addHandlerLast(StringEncoder(Charsets.UTF_8))
        connection.addHandlerLast(LineEncoder(LineSeparator(encoder.utf8())))
    }

    fun withHost(host: String, port: Int = 0): Proxy {
        config = config.host(host)

        if (port != 0)
            config = config.port(port)
        return this
    }

    fun withWorker(selectThreads: Int, workerThreads: Int): Proxy {
        config = config.runOn(LoopResources.create("worker", selectThreads, workerThreads, true))
        return this
    }

    fun start(async: Boolean = false) {
        if (async) {
            thread(start = true) {
                start()
            }
        } else {
            server = config.bindNow()
            server.onDispose().block()
        }
    }

    fun log(msg: Any, source: ProxyClientStream? = null, target: ProxyClientStream? = null, forwarded: Boolean = false) {
        val from = when (source) {
            null -> "proxy"
            else -> source.label
        }

        val to = when (target) {
            null -> "proxy"
            else -> target.label
        }

        log(from, to, msg, forwarded ?: false)
    }

    private fun log(from: String, to: String, msg: Any, forwarded: Boolean = false) {
        val sendType = when(forwarded) {
            true -> "FORWARDED"
            false -> "SENT"
        }

        log.info("%-12s %-11s ---> %-15s: %s",
            "[${from.toUpperCase()}]", "[${sendType.toUpperCase()}]", "[${to.toUpperCase()}]", msg)
    }

    fun log(who: String, info: String, level: Log.LoggingLevel) {
        val str = String.format("%-12s %-11s", "[${who.toUpperCase()}]", "[${info.toUpperCase()}]")

        when (level) {
            Log.LoggingLevel.DEBUG  -> log.debug(str)
            Log.LoggingLevel.INFO   -> log.info(str)
            Log.LoggingLevel.GOOD   -> log.good(str)
            Log.LoggingLevel.BAD    -> log.bad(str)
            Log.LoggingLevel.WARN   -> log.warn(str)
            Log.LoggingLevel.ERROR  -> log.error(str)
            Log.LoggingLevel.TODO   -> log.todo(str)
        }
    }
}