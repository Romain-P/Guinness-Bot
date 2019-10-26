package com.guiness.bot.services

import com.guiness.bot.core.Config
import com.guiness.bot.services.util.SwfExtractor
import java.io.IOException
import java.net.URL
import java.io.FileInputStream

//TODO: improve it (see https://github.com/Romain-P/Guinness-Bot/issues/6)
object SwfService {
    @Throws(IOException::class)
    fun downloadMap(id: Int, date: String): Map<String, Any> {
        val swf = "${id}_${date}X.swf"

        val stream = try {
            FileInputStream(Config.localMapDataPath() + swf)
        } catch (e: Exception) {
            URL(Config.remoteMapDataPath + swf).openStream()
        }

        return SwfExtractor.extractData(stream)
    }
}