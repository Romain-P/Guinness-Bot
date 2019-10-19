package com.guiness.bot.protocol.messages.custom

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.MessageTarget

@Message(header = ".{AH}.", target = MessageTarget.MITM, delimiter = ";")
data class AuthServerInfoMsg(
    val ip: String,
    val port: Int
)