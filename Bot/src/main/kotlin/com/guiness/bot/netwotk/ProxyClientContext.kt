package com.guiness.bot.netwotk

import io.netty.channel.Channel
import io.netty.channel.ChannelFuture
import reactor.netty.Connection

class ProxyClientContext(
    private val channel: Channel,
    private val connection: Connection,
    val uuid: String = channel.id().asLongText()
) {
    fun send(packet: String): ChannelFuture = channel.writeAndFlush(packet)

    companion object {
        fun of(connection: Connection) = ProxyClientContext(connection.channel(), connection)
    }
}