package com.guiness.bot.protocol.messages.game.basics

import com.guiness.bot.protocol.annotations.Delimiter
import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

/* BM*|Salut| */
@Message("dqsdBM", StreamSource.DOWNSTREAM, delimiter = "|")
data class ChatRequestMessage(
    var channel: String,
    var message: String,
    /*
        Should be
        var messageArgs: Array<ChatMessageArgs>?
    */
    @Delimiter("!")
    var messageArgs : Array<String>?
)