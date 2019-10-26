package com.guiness.bot.core

object Config {
    /* will be retrieved by native api **/
    lateinit var dofusPath: String
    var remoteDataPath = "http://dl.ak.ankama.com/games/dofusretro/"
    var remoteMapPath = "${remoteDataPath}maps/"
    val remoteLangVersionsPath = "${remoteDataPath}lang/"
    val remoteLangPath = "${remoteDataPath}lang/swf/"

    fun localMapDataPath() = "$dofusPath\\data\\maps\\"

    fun isDofusPathInitialized() = ::dofusPath.isInitialized
}