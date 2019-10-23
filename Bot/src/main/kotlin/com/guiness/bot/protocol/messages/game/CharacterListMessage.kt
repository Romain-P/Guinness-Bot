package com.guiness.bot.protocol.messages.game

import com.guiness.bot.protocol.annotations.Delimiter
import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.Size
import com.guiness.bot.protocol.annotations.StreamSource

typealias HexItemTemplateID = String?
typealias HexColorCode = String?

enum class VisibleStuffType(val index: Int) {
    WEAPON(0),
    HEAD(1),
    BACK(2),
    PET(3),
    SHIELD(4)
}

@Delimiter(";")
data class Character(
    var guid: Int,
    var name: String,
    var level: Int,
    var gfxId: Int,
    @Size(3)
    var colors: Array<HexColorCode>,
    @Delimiter(",") @Size(5)
    var visualStuff: Array<HexItemTemplateID>,
    var sellMode: Int,
    var serverId: Int,
    var deathCount: Int?,
    var isLevelMax: Int?,
    var unknown: Int?
) {
    fun visibleStuffId(type: VisibleStuffType): String? = visualStuff[type.index]
}

@Message(header = "ALK", source = StreamSource.UPSTREAM, delimiter = "|")
data class CharacterListMessage(
    var subscriptionTime: Int,
    var charactersCount: Int,
    var characters: Array<Character>
)