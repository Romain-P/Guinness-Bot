package com.guiness.bot.netwotk

import io.netty.channel.Channel
import io.netty.channel.ChannelFuture
import reactor.netty.Connection

class ProxyClientStream(
    private val connection: Connection,
    private val channel: Channel = connection.channel(),
    val uuid: String = channel.id().asLongText()
) {
    fun send(packet: String): ChannelFuture = channel.writeAndFlush(packet)
}