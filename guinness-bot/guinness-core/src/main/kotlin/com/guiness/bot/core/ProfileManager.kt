package com.guiness.bot.core

import com.guiness.bot.entities.Account
import com.guiness.bot.entities.Character
import com.guiness.bot.entities.Profile

object ProfileManager {
    val profiles: MutableMap<ProfileAlias, Profile> = hashMapOf()

    init {
        val defaultProfile =  Profile("default")
        val account = Account("yourAccount", "yourPassword", mutableListOf("ScriptTest"))
        val character = Character("YourCharacterName", 608 /** Ayuto **/)

        account.addCharacter(character)
        account.defaultCharacter = character
        defaultProfile.accounts["yourAccount"] = account
        profiles["default"] = defaultProfile
    }

    fun getDefaultProfile(): Profile {
        return profiles["default"]!!
    }

    fun getProfile(alias: ProfileAlias): Profile? {
        return profiles[alias]
    }
}