package com.guiness.bot.entities

import com.guinness.api.ISession
import com.guinness.api.entities.ICharacter
import com.guinness.api.entities.IMap

class Session: ISession {
    override var character: ICharacter? = null
    override var map: IMap? = null
}