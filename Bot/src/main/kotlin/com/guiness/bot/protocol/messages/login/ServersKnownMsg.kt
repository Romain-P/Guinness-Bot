package com.guiness.bot.protocol.messages.login

import com.guiness.bot.protocol.annotations.Delimiter
import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource

@Delimiter(",")
data class KnownServer(
    val serverId: Int,        /** ID of the realmd **/
    val charactersCount: Int  /** number of characters on this server **/
)

@Message(header = "AxK", source = StreamSource.UPSTREAM, delimiter = "|")
data class ServersKnownMsg(
    val subscriptionTime: Int,
    @Delimiter(";")
    val servers: Array<KnownServer>
)