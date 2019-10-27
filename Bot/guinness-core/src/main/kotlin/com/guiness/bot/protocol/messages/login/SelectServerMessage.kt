package com.guiness.bot.protocol.messages.login

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = "AX", source = StreamSource.DOWNSTREAM)
data class SelectServerMessage(
    var serverId: Int
)