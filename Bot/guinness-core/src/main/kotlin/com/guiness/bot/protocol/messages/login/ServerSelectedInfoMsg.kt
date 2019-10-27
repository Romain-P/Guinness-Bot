package com.guiness.bot.protocol.messages.login

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = "AXK", source = StreamSource.UPSTREAM)
data class ServerSelectedInfoMsg(
    var compressedData: String
)