package com.guiness.bot.protocol.messages.game.basics

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

/* BM*|Salut| */
@Message("BM", StreamSource.DOWNSTREAM, delimiter = "|")
data class ChatRequestMessage(
    var channel: String,
    var message: String
)