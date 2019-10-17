package com.guiness.bot.protocol.messages.custom

import com.guiness.bot.protocol.annotations.Message

@Message(header = ".{AH}.", delimiter = """|""")
data class AuthServerInfoMsg(
    val ip: String,
    val port: Int
)