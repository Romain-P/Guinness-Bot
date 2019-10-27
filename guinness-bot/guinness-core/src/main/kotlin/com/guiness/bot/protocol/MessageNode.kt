package com.guiness.bot.protocol

class MessageNode(
    var message: MetaMessage? = null,
    val nodes: MutableMap<Char, MessageNode> = HashMap()
)