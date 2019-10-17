package com.guiness.bot.netwotk

import io.netty.channel.Channel
import io.netty.channel.ChannelFuture
import reactor.netty.Connection

class ProxyClientContext(
    private val downstream: ProxyClientStream,
    private var upstream: ProxyClientStream? = null
) {
    fun uuid() = downstream.uuid

    fun downstream() = downstream
    fun upstream() : ProxyClientStream = upstream!!

    fun attach(upstreamConnection: Connection) {
        this.upstream = ProxyClientStream(upstreamConnection)
    }

    companion object {
        fun of(connection: Connection) = ProxyClientContext(ProxyClientStream(connection))
    }
}