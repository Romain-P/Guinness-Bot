package com.guiness.bot.protocol.entities.map

class MAP(
    var id: Int,
    var x: Int,
    var y: Int,
    var area: Int,
    var subArea: Int,
    var width: Int,
    var height: Int,
    var capabilities: Int,
    var outdoor: Boolean
) {
    lateinit var cells: Array<CELL>
}