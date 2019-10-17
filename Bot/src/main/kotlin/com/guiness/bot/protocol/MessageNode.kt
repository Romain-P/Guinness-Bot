package com.guiness.bot.protocol

import kotlin.reflect.KClass

class MessageNode(
    var message: KClass<*>? = null,
    val nodes: MutableMap<Char, MessageNode> = HashMap()
)