package com.guiness.bot.protocol.messages.custom

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = ".{AGTH}.", source = StreamSource.DOWNSTREAM)
data class GameTicketMessage(
    val ticket: String
)