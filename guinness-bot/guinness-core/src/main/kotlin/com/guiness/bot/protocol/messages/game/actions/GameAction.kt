package com.guiness.bot.protocol.messages.game.actions

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = "GA", source = StreamSource.UPSTREAM, delimiter = ";")
data class GameAction(
    var unknown: Long?,
    var actionId: Int,
    var entityGuid: Long, /* character guid or mob group guid */
    var data: Array<String>
)