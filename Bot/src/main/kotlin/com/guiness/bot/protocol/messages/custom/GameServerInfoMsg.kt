package com.guiness.bot.protocol.messages.custom

import com.guiness.bot.protocol.annotations.Message

@Message(".{GH}.")
data class GameServerInfoMsg(
    val ip: String,
    val port: Int
)