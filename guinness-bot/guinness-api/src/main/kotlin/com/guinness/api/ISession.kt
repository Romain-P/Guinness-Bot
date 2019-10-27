package com.guinness.api

import com.guinness.api.entities.ICharacter
import com.guinness.api.entities.IMap

interface ISession {
    var character: ICharacter?
    var map: IMap?
}