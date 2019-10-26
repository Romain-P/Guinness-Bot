package com.guiness.bot.services

import com.guiness.bot.core.value
import com.guiness.bot.protocol.entities.map.MAP
import com.guiness.bot.services.util.MapUtil
import java.lang.RuntimeException

object MapService {
    private val mapLangs: Map<String, HashMap<String, Int>> = SwfService.swfLangData("maps")

    fun loadMap(id: Int, date: String, key: String): MAP {
        val swfData = SwfService.mapData(id, date)

        val worldMapInfo = mapLangs.filter { it.key.endsWith(".$id") }.iterator().next().value
        val x: Int = worldMapInfo.value("x")
        val y: Int = worldMapInfo.value("y")
        val area: Int = worldMapInfo.value("ep")
        val subArea: Int = worldMapInfo.value("sa")

        val map = MAP(id, x, y, area, subArea,
            swfData.value("width"),
            swfData.value("height"),
            swfData.value("capabilities"),
            swfData.value("bOutdoor")
        )

        val mapData: String = swfData.value("mapData")
        val cellData = MapUtil.decryptMapData(mapData, key)
        val cells = MapUtil.uncompressCells(map, cellData)

        map.cells = cells
        return map
    }
}