package com.guinness.api.entities

interface IMap {
    val id: Int
    val x: Int
    val y: Int
    val area: Int
    val subArea: Int
    val width: Int
    val height: Int
    val capabilities: Int
    val outdoor: Boolean
    var cells: Array<ICell>
}