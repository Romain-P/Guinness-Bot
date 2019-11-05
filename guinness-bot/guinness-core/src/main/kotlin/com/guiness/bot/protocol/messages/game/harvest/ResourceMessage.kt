package com.guiness.bot.protocol.messages.game.harvest

import com.guiness.bot.protocol.annotations.Delimiter
import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.Size
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = "GD", source = StreamSource.UPSTREAM, delimiter = "|")
data class ResourceRepop(
    var type : String?,
    @Size(3) @Delimiter(";")
    var cellInfo : Array<Int>?
)