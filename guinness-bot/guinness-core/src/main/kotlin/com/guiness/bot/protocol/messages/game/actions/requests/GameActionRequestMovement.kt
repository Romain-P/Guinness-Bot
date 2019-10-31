package com.guiness.bot.protocol.messages.game.actions.requests

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = "GA001", source = StreamSource.DOWNSTREAM)
data class GameActionRequestMovement(
    var cell: String
)