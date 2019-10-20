package com.guiness.bot.netwotk.shared.annotations

import com.guiness.bot.netwotk.shared.HandlerPriority
import com.guiness.bot.netwotk.shared.PipelineOperation
import com.guiness.bot.netwotk.shared.StreamOperation

/**
 * Annotate a handler with this annotation if the message comes from the upstream (ankama servers)
 */
annotation class FromUpstream(
    val then: StreamOperation,
    val pipeline: PipelineOperation = PipelineOperation.CONTINUE,
    val priority: Int = HandlerPriority.DEFAULT
)