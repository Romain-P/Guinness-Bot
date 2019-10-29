package com.guiness.bot.protocol.messages.game.actions

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = "GAF", source = StreamSource.UPSTREAM, delimiter = "|")
data class GameActionFinish(
    var actionId: Int,
    var characterGuid: Long
)