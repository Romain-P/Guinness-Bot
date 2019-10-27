package com.guinness.api.entities

interface ICell {
    val map: IMap
    val id: Int
    val active: Boolean
    val lineOfSight: Boolean
    val layerGroundRotate: Int
    val groundLevel: Int
    val movement: Int
    val layerGroundNum: Int
    val groundSlope: Int
    val layerGroundFlip: Boolean
    val layerObject1Num: Int
    val layerObject1Rotate: Int
    val layerObject1Flip: Boolean
    val layerObject2Flip: Boolean
    val layerObject2Interactive: Boolean
    val layerObject2Num: Int
    val x: Int
    val y: Int
    val xRotate: Int
    val yRotate: Int
}