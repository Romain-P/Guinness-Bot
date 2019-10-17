package com.guiness.bot.protocol.annotations

@Target(AnnotationTarget.FIELD, AnnotationTarget.CLASS)
@Retention(AnnotationRetention.RUNTIME)
annotation class Parse(
    val delimiter: String
)