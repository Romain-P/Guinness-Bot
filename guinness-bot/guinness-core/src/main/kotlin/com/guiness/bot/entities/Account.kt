package com.guiness.bot.entities

import com.guiness.bot.core.AccountName
import com.guiness.bot.core.CharacterName
import com.guiness.bot.core.ScriptName

class Account(
    var username: AccountName,
    var password: String,
    val scripts: MutableList<ScriptName> = ArrayList()
) {
    val characters: MutableMap<CharacterName, Character> = HashMap()
    var defaultCharacter: Character? = null

    fun addCharacter(character: Character) {
        characters[character.name] = character
    }
}