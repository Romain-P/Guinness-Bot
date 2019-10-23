package com.guiness.bot.protocol.messages.login

import com.guiness.bot.protocol.annotations.Delimiter
import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Delimiter(",")
data class KnownServer(
    var serverId: Int,
    var charactersCount: Int
)

@Message(header = "AxK", source = StreamSource.UPSTREAM, delimiter = "|")
data class ServersKnownMessage(
    var subscriptionTime: Int,
    var servers: Array<KnownServer>
)