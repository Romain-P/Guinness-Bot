package com.guiness.bot.netwotk

import com.guiness.bot.core.Packet
import com.guiness.bot.core.Unwrapped
import com.guiness.bot.netwotk.shared.PendingPacket
import com.guiness.bot.protocol.DofusProtocol
import io.netty.channel.Channel
import io.netty.channel.ChannelFuture
import reactor.netty.Connection
import java.util.*

class ProxyClientStream(
    private val connection: Connection,
    private val channel: Channel = connection.channel(),
    val buffer: MutableList<PendingPacket> = ArrayList(),
    val uuid: String = channel.id().asLongText()
) {
    fun write(message: Any, unwrapped: Boolean = false) : ProxyClientStream {
        buffer.add(PendingPacket(message, unwrapped))
        return this
    }

    fun flush() {
        for (pending in buffer) {
            when (pending.unwrapped) {
                true -> channel.writeAndFlush(pending.packet)
                false -> channel.writeAndFlush(DofusProtocol.deserialize(pending.packet as String))
            }
        }
        buffer.clear()
    }
}