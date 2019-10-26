package com.guiness.bot.protocol.entities.map

class CELL(
    val map: MAP,
    val id: Int,
    var active: Boolean,
    var lineOfSight: Boolean,
    var layerGroundRotate: Int,
    var groundLevel: Int,
    var movement: Int,
    var layerGroundNum: Int,
    var groundSlope: Int,
    var layerGroundFlip: Boolean,
    var layerObject1Num: Int,
    var layerObject1Rot: Int,
    var layerObject1Flip: Boolean,
    var layerObject2Flip: Boolean,
    var layerObject2Interactive: Boolean,
    var layerObject2Num: Int,
    var x: Int,
    var y: Int,
    var xRotate: Int,
    var yRotate: Int
)