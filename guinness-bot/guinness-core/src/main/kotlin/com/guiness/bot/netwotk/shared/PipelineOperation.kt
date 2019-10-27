package com.guiness.bot.netwotk.shared

/**
 * A message can be handled by several handlers
 * One handler can decide to stop the pipeline so that the other handlers won't intercept the packet
 */
enum class PipelineOperation {
    /** continues the pipeline, default value **/
    CONTINUE,
    /** stops the pipeline **/
    BREAK
}