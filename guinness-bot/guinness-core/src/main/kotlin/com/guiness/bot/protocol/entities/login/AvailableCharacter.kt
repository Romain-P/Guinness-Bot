package com.guiness.bot.protocol.entities.login

import com.guiness.bot.protocol.annotations.Delimiter
import com.guiness.bot.protocol.annotations.Hex
import com.guiness.bot.protocol.annotations.Size
import com.guiness.bot.protocol.enums.VisibleStuffType

@Delimiter(";")
data class AvailableCharacter(
    var guid: Int,
    var name: String,
    var level: Int,
    var gfxId: Int,
    @Size(3)
    var colors: Array<@Hex Int?>,
    @Delimiter(",") @Size(5)
    var visualStuff: Array<@Hex Int?>,
    var sellMode: Int,
    var serverId: Int,
    var deathCount: Int?,
    var isLevelMax: Int?,
    var unknown: Int?
) {
    fun visibleStuffId(type: VisibleStuffType): Int? = visualStuff[type.index]

    fun setVisualStuffId(type: VisibleStuffType, newId: Int) {
        visualStuff[type.index] = newId
    }
}