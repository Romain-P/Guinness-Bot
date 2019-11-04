package com.guiness.bot.protocol.messages.game.actions

import com.guiness.bot.protocol.annotations.Bytes
import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = "GA", source = StreamSource.DOWNSTREAM)
data class GameActionRequest(
    @Bytes(3)
    var actionId: Int,
    var data: String?
)