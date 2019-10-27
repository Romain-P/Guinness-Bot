package com.guiness.bot.entities

import com.guiness.bot.core.AccountName
import com.guiness.bot.core.CharacterName

class Account(
    var username: AccountName,
    var password: String
) {
    val characters: MutableMap<CharacterName, Character> = HashMap()
    var defaultCharacter: Character? = null

    fun addCharacter(character: Character) {
        characters[character.name] = character
    }
}