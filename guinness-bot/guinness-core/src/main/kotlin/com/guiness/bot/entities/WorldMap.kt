package com.guiness.bot.entities

import com.guinness.api.entities.ICell
import com.guinness.api.entities.IMap

class WorldMap(
    override val id: Int,
    override val x: Int,
    override val y: Int,
    override val area: Int,
    override val subArea: Int,
    override val width: Int,
    override val height: Int,
    override val capabilities: Int,
    override val outdoor: Boolean
): IMap {
    override lateinit var cells: Array<ICell>
}