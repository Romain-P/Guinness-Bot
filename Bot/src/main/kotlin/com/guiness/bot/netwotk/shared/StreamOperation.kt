package com.guiness.bot.netwotk.shared

/**
 * Controls the workflow of an incoming packet
 * (downstream|dofus.exe) <=> MITM <=> (upstream|ankama)
 */
enum class StreamOperation {
    /** Forwards the message received from Upstream to Downstream and vice-versa  */
    FORWARD,
    /** Stops the received message: it won't be sent to the other stream  */
    DISCARD,
    /** Returns the message to the source stream  */
    MIRROR
}