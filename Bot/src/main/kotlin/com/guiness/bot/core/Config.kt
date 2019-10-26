package com.guiness.bot.core

object Config {
    /* will be retrieved by native api **/
    lateinit var dofusPath: String
    var remoteMapDataPath = "http://dl.ak.ankama.com/games/dofusretro/maps/"

    fun localMapDataPath() = "$dofusPath\\data\\maps\\"

    fun isDofusPathInitialized() = ::dofusPath.isInitialized
}