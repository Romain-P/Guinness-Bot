package com.guiness.bot.protocol.messages.game

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Message(header = "GDM|", source = StreamSource.UPSTREAM, delimiter = "|")
data class MapDataMessage(
    var mapId: Int,
    var date: String, /** must be as string since it can begin with 0 **/
    var privateKey: String
)