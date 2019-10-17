package com.guiness.bot.protocol

import io.netty.buffer.ByteBuf
import io.netty.buffer.Unpooled

fun ByteBuf.utf8() = this.toString(Charsets.UTF_8)

object DofusProtocol {
    val CLIENT_DELIMITER: ByteBuf = Unpooled.wrappedBuffer(byteArrayOf(0xa, 0x0))
    val SERVER_DELIMITER: ByteBuf = Unpooled.wrappedBuffer(byteArrayOf(0x0))


}