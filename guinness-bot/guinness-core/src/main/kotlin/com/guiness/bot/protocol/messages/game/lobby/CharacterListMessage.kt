package com.guiness.bot.protocol.messages.game.lobby

import com.guiness.bot.protocol.annotations.*
import com.guiness.bot.protocol.entities.login.AvailableCharacter

@Message(header = "ALK", source = StreamSource.UPSTREAM, delimiter = "|")
data class CharacterListMessage(
    var subscriptionTime: Long,
    var charactersCount: Int,
    var characters: Array<AvailableCharacter>
)