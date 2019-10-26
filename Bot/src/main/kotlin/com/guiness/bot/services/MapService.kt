package com.guiness.bot.services

import com.guiness.bot.protocol.entities.map.MAP
import com.guiness.bot.services.util.MapUtil
import java.lang.RuntimeException

object MapService {
    fun loadMap(id: Int, date: String, key: String) {
        val swfData = SwfService.downloadMap(id, date)
        val map = MAP(id,
            value(swfData, "width"),
            value(swfData, "height"),
            value(swfData, "capabilities"),
            value(swfData, "bOutdoor"))

        val mapData: String = value(swfData, "mapData")
        val cellData = MapUtil.decryptMapData(mapData, key)
        val cells = MapUtil.uncompressCells(map, cellData)

        map.cells = cells
    }

    inline fun <reified T> value(data: Map<String, Any>, key: String): T {
        return when (val value = data[key]) {
            is T    -> value
            else    -> throw RuntimeException("Invalid key request in swf data #MapService")
        }
    }
}