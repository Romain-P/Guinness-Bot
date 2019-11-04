package com.guiness.bot.protocol.messages.game.basics

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

/* BM*|Salut| */
@Message("cM", StreamSource.UPSTREAM, delimiter = "|")
data class ChatReceiveMessage(
    var type: String,
    var unknown: Long,
    var sender: String,
    var message: String
)