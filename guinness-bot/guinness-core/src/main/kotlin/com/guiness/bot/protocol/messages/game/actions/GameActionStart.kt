package com.guiness.bot.protocol.messages.game.actions

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = "GAS", source = StreamSource.UPSTREAM)
data class GameActionStart(
    var characterGuid: Long
)