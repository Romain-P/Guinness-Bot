package com.guiness.bot.protocol.entities.map

class MAP(
    var id: Int,
    var width: Int,
    var height: Int,
    var capabilities: Int,
    var outdoor: Boolean
) {
    lateinit var cells: Array<CELL>
}