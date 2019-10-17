package com.guiness.bot.protocol.annotations

@Target(AnnotationTarget.CLASS)
@Retention(AnnotationRetention.RUNTIME)
annotation class Message(
    val header: String,
    val delimiter: String = """|"""
)