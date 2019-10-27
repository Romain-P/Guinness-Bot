package com.guiness.bot.game

import com.guinness.api.entities.ICell
import com.guinness.api.entities.IMap

class MapCell(
    override val map: IMap,
    override val id: Int,
    override val active: Boolean,
    override val lineOfSight: Boolean,
    override val layerGroundRotate: Int,
    override val groundLevel: Int,
    override val movement: Int,
    override val layerGroundNum: Int,
    override val groundSlope: Int,
    override val layerGroundFlip: Boolean,
    override val layerObject1Num: Int,
    override val layerObject1Rotate: Int,
    override val layerObject1Flip: Boolean,
    override val layerObject2Flip: Boolean,
    override val layerObject2Interactive: Boolean,
    override val layerObject2Num: Int,
    override val x: Int,
    override val y: Int,
    override val xRotate: Int,
    override val yRotate: Int
): ICell