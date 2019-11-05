package com.guiness.bot.protocol.messages.game.harvest

import com.guiness.bot.protocol.annotations.Delimiter
import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.Size
import com.guiness.bot.protocol.annotations.StreamSource

@Delimiter(";")
data class ResourceCellInfo(
    var cellId : Int,
    var unknown : Int,
    var status : Int
)

@Message(header = "GDF|", source = StreamSource.UPSTREAM, delimiter = "|")
data class ResourceMessage(
    var cellInfo : Array<ResourceCellInfo>?
)