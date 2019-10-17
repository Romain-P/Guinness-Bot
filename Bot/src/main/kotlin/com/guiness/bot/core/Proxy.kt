package com.guiness.bot.core

import io.netty.buffer.ByteBuf
import io.netty.buffer.Unpooled
import io.netty.channel.ChannelOption
import io.netty.handler.codec.DelimiterBasedFrameDecoder
import io.netty.handler.codec.string.LineEncoder
import io.netty.handler.codec.string.LineSeparator
import io.netty.handler.codec.string.StringDecoder
import reactor.core.publisher.Flux
import reactor.netty.Connection
import reactor.netty.DisposableServer
import reactor.netty.resources.LoopResources
import reactor.netty.tcp.TcpServer
import java.time.Duration


val NL: ByteBuf = Unpooled.wrappedBuffer(byteArrayOf(0x0))
val LFNL: ByteBuf = Unpooled.wrappedBuffer(byteArrayOf(0xa, 0x0))

object Proxy {
    private lateinit var server: DisposableServer
    private var config: TcpServer

    init {
        config = TcpServer.create()
            .option(ChannelOption.SO_REUSEADDR, true)
            .doOnConnection {
                it.addHandlerFirst("frame-decoder",
                    DelimiterBasedFrameDecoder(Integer.MAX_VALUE, Unpooled.wrappedBuffer(byteArrayOf('A'.toByte()))))
                it.addHandlerLast("decoder", StringDecoder(Charsets.UTF_8))
                it.addHandlerLast("frame-encoder", LineEncoder(LineSeparator(".PUTE")))
            }
            .handle { inbound, outbound ->
                inbound.withConnection(::onReceive)
                Flux.never()
            }
    }

    fun onReceive(ctx: Connection) {
        ctx.inbound().receiveObject()
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