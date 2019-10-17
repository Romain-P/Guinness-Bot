package com.guiness.bot.netwotk

import com.guiness.bot.core.ChannelID
import com.guiness.bot.protocol.DofusProtocol
import com.guiness.bot.protocol.utf8
import io.netty.buffer.Unpooled
import io.netty.channel.ChannelOption
import io.netty.handler.codec.DelimiterBasedFrameDecoder
import io.netty.handler.codec.string.LineEncoder
import io.netty.handler.codec.string.LineSeparator
import io.netty.handler.codec.string.StringDecoder
import org.reactivestreams.Publisher
import reactor.core.publisher.Flux
import reactor.netty.DisposableServer
import reactor.netty.NettyInbound
import reactor.netty.NettyOutbound
import reactor.netty.resources.LoopResources
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
                it.addHandlerFirst(DelimiterBasedFrameDecoder(Integer.MAX_VALUE, DofusProtocol.CLIENT_DELIMITER))
                it.addHandlerLast(StringDecoder(Charsets.UTF_8))
                it.addHandlerLast(LineEncoder(LineSeparator(DofusProtocol.SERVER_DELIMITER.utf8())))

                clients[it.channel().id().asLongText()] = ProxyClientContext.of(it)
            }
            .handle(::clientHandler)
    }

    fun clientHandler(inbound: NettyInbound, outbound: NettyOutbound): Publisher<Void> {
        inbound.withConnection {ctx ->
            ctx.inbound().receiveObject().subscribe {
                onReceive(clients[ctx.channel().id().asLongText()]!!, it as String)
            }
        }
        return Flux.never()
    }

    fun onReceive(ctx: ProxyClientContext, packet: String) {
        println("[Channel ${ctx.uuid}] Received $packet")
        ctx.send("Ok j'ai reçu tho")
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