package com.guiness.bot.protocol.annotations

annotation class Message(
    val header: String,
    val delimiter: String = """|"""
)