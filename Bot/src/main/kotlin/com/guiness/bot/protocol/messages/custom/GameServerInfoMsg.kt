package com.guiness.bot.protocol.messages.custom

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = ".{GH}.", source = StreamSource.DOWNSTREAM, delimiter = ":")
data class GameServerInfoMsg(
    val ip: String,
    val port: Int
)