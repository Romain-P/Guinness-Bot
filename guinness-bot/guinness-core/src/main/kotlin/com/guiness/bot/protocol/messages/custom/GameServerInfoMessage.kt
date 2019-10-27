package com.guiness.bot.protocol.messages.custom

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = ".{GH}.", source = StreamSource.DOWNSTREAM, delimiter = ":")
data class GameServerInfoMessage(
    val ip: String,
    val port: Int,
    val ticket: String
)