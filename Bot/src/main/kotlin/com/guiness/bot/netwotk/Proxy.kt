package com.guiness.bot.netwotk

import com.guiness.bot.core.ChannelID
import com.guiness.bot.protocol.DofusProtocol
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


object Proxy {
    private lateinit var server: DisposableServer
    private var config: TcpServer
    private val clients: MutableMap<ChannelID, ProxyClientContext>

    init {
        clients = HashMap()

        config = TcpServer.create()
            .option(ChannelOption.SO_REUSEADDR, true)
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
        val ctx = ProxyClientContext.of(connection)
        clients[connection.channel().id().asLongText()] = ctx

        TcpClient.create()
            .host("127.0.0.1")
            .port(5558)
            .doOnConnected {
                addHandlers(it, upstream = true)
                ctx.attach(it)
            }
            .handle { inbound, _ -> upstreamHandler(inbound, ctx) }
            .connect()
            .subscribe()
    }

    fun addHandlers(connection: Connection, upstream: Boolean = false, downstream: Boolean = false) {
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

    fun start() {
        server = config.bindNow()
        server.onDispose().block()
    }
}