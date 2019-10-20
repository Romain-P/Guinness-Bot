package com.guiness.bot.protocol.messages.custom

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = ".{AH}.", source = StreamSource.DOWNSTREAM, delimiter = ":")
data class AuthServerInfoMsg(
    val ip: String,
    val port: Int
)