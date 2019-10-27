package com.guiness.bot.protocol.messages.game

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = "AS", source = StreamSource.DOWNSTREAM)
data class SelectCharacterMessage(
    var characterGuid: Int
)