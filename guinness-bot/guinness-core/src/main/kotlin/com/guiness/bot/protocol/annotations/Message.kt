package com.guiness.bot.protocol.annotations

annotation class Message(
    val header: String,
    val source: StreamSource,
    val delimiter: String = "\u0000"
)