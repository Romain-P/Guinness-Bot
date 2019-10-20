package com.guiness.bot.netwotk

import com.guiness.bot.log.logger
import com.guiness.bot.netwotk.shared.PendingPacket
import com.guiness.bot.protocol.DofusProtocol
import io.netty.channel.Channel
import reactor.netty.Connection
import java.util.*
import java.net.InetSocketAddress

class ProxyClientStream(
    val label: String,
    private val connection: Connection,
    private val channel: Channel = connection.channel(),
    private val buffer: MutableList<PendingPacket> = ArrayList(),
    val uuid: String = channel.id().asLongText()
) {
    fun write(message: Any, unwrapped: Boolean = false, forwarded: Boolean = false) : ProxyClientStream {
        buffer.add(PendingPacket(message, unwrapped, forwarded))
        return this
    }

    fun flush() {
        for (pending in buffer) {
            when (pending.unwrapped) {
                true -> {
                    channel.writeAndFlush(pending.packet)
                    Proxy.log(pending.packet, target = this, forwarded = pending.forwarded)
                }
                false -> {
                    val message = DofusProtocol.deserialize(pending.packet as String)!!
                    channel.writeAndFlush(message)
                    Proxy.log(message, target = this, forwarded = pending.forwarded)
                }
            }
        }
        buffer.clear()
    }

    fun hostAddress() = (channel.remoteAddress() as InetSocketAddress).address.hostAddress

    fun close() {
        if (channel.isOpen)
            channel.close()
    }

    companion object {
        val log by logger()
        val proxyLog = log.appendName("proxy")
    }
}