package com.guiness.bot.netwotk.shared

import kotlin.reflect.KFunction

class MessageHandler(
    val controller: Any,
    val handler: KFunction<*>,
    val priority: Int,
    val then: StreamOperation,
    val pipeline: PipelineOperation
)