package com.guiness.bot.entities

import com.guinness.api.entities.ICharacter

class Character(
    override var name: String,
    override var serverId: Int
): ICharacter