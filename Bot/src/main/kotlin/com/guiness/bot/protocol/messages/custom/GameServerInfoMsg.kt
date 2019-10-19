package com.guiness.bot.protocol.messages.custom

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.MessageTarget

@Message(header = ".{GH}.", target = MessageTarget.MITM, delimiter = ";")
data class GameServerInfoMsg(
    val ip: String,
    val port: Int
)