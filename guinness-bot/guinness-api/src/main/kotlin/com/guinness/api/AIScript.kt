package com.guinness.api

import com.guinness.api.entities.IMap

open class AIScript(private val bot: IBot) {
    open fun onWorldEnter() {}
    open fun onMapChanged(previous: IMap?, new: IMap) {}
}