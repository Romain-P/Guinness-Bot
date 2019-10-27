package com.guiness.bot.protocol.messages.game

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = "Rx", source = StreamSource.UPSTREAM)
data class MountGivenXpMessage(
    var givenXp: Int
)