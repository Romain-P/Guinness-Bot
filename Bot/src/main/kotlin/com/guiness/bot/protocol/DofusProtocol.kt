package com.guiness.bot.protocol

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.messages.custom.TestMsg
import io.netty.buffer.ByteBuf
import io.netty.buffer.Unpooled
import java.lang.RuntimeException
import kotlin.reflect.KClass

fun ByteBuf.utf8(): String = this.toString(Charsets.UTF_8)

object DofusProtocol {
    val CLIENT_DELIMITER: ByteBuf = Unpooled.wrappedBuffer(byteArrayOf(0xa, 0x0))
    val SERVER_DELIMITER: ByteBuf = Unpooled.wrappedBuffer(byteArrayOf(0x0))

    val messages: MutableMap<Char, MessageNode> = HashMap()
    val classes: List<KClass<*>> = listOf(TestMsg::class)

    fun initNodes() {
        classes.forEach {msg ->
            val meta = msg.annotations.find {it is Message }!! as Message
            var node: MessageNode? = null

            for (i in meta.header.indices) {
                val c = meta.header[i]

                when {
                    ( node == null ) -> {
                        when(val firstNode = messages[c]) {
                            null -> {
                                node = MessageNode()
                                messages[c] = node
                            }
                            else -> node = firstNode
                        }
                    }
                    ( !node.nodes.containsKey(c) ) -> {
                        val newNode = MessageNode()
                        node.nodes[c] = newNode
                        node = newNode
                    }
                    else -> node = node.nodes[c]!!
                }

                if (i == meta.header.length - 1) {
                    if (node.message != null)
                        throw RuntimeException("Found several packet with same header: " +
                                "\n${msg.qualifiedName}\n${node.message!!.qualifiedName}\n")
                    node.message = msg
                }
            }
        }
    }
}