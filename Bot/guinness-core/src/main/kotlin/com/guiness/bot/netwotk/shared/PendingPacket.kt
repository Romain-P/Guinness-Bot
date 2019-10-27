package com.guiness.bot.netwotk.shared

class PendingPacket(
    val packet: Any,
    val unwrapped: Boolean,
    val forwarded: Boolean
)