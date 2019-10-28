package com.guiness.bot.protocol.messages.game

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = "GA", source = StreamSource.UPSTREAM)
data class GameAction(
    var data: String
)