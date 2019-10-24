package com.guiness.bot.netwotk

import com.guiness.bot.netwotk.shared.PendingPacket
import com.guiness.bot.protocol.DofusProtocol
import com.kizitonwose.time.Interval
import io.netty.channel.Channel
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.async
import kotlinx.coroutines.delay
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
    @Synchronized fun write(message: Any, unwrapped: Boolean = false, forwarded: Boolean = false) : ProxyClientStream {
        buffer.add(PendingPacket(message, unwrapped, forwarded))
        return this
    }

    fun writeAndFlush(message: Any, unwrapped: Boolean = false, forwarded: Boolean = false) : ProxyClientStream {
        when (unwrapped) {
            true -> {
                channel.writeAndFlush(message)
                Proxy.log(message, target = this, forwarded = forwarded)
            }
            false -> {
                val packet = DofusProtocol.serialize(message)!!
                channel.writeAndFlush(packet)
                Proxy.log(message, target = this, forwarded = forwarded)
            }
        }
        return this
    }

    fun post(message: Any, delayed: Interval<*>, unwrapped: Boolean = false): ProxyClientStream {
        delayedTransaction {
            later(message, delayed, unwrapped)
        }
        return this
    }

    fun delayedTransaction(transaction: suspend ProxyClientStream.() -> Unit) {
        GlobalScope.run {
            async {
                transaction(this@ProxyClientStream)
            }
        }
    }

    /**
     * Can be called only in a delayed transaction
     */
    suspend fun later(message: Any, delayed: Interval<*>? = null, unwrapped: Boolean = false) {
        if (delayed != null)
            delay(delayed.inMilliseconds.longValue)
        writeAndFlush(message, unwrapped)
    }

    @Synchronized fun flush() {
        for (pending in buffer)
            writeAndFlush(pending.packet, pending.unwrapped, pending.forwarded)
        buffer.clear()
    }

    fun hostAddress() = (channel.remoteAddress() as InetSocketAddress).address.hostAddress

    fun close() {
        if (channel.isOpen)
            channel.close()
    }
}