package com.guiness.bot.services

import com.guiness.bot.core.Config
import com.guiness.bot.services.util.SwfExtractor
import java.io.BufferedReader
import java.io.IOException
import java.net.URL
import java.io.FileInputStream
import java.io.InputStreamReader
import java.util.HashMap

object SwfService {
    const val localization = "fr"
    val langVersions = loadLangVersions(localization)
    val cache = HashMap<Int, Map<String, Any>>()

    fun mapData(id: Int, date: String): Map<String, Any> {
        cache.putIfAbsent(id, swfData("${id}_${date}X", Config.localMapDataPath(), Config.remoteMapPath))
        return cache[id]!!
    }

    fun swfData(fileName: String, localPath: String, remoteUrlOnFailure: String): Map<String, Any> {
        val stream = try {
            FileInputStream("$localPath$fileName.swf")
        } catch (e: Exception) {
            URL("$remoteUrlOnFailure$fileName.swf").openStream()
        }

        return SwfExtractor.extractData(stream)
    }

    inline fun <reified T> swfLangData(dataType: String): T {
        val version = langVersions[dataType]!!
        val file = "${dataType}_${localization}_${version}.swf"
        val input = URL(Config.remoteLangPath + file).openStream()

        return SwfExtractor.extractData(input) as T
    }

    private fun loadLangVersions(lang: String): Map<String, Int> {
        val data = getText(
            URL(Config.remoteLangVersionsPath + "versions_$lang.txt")
        ).substring(3)/* Remove &f= */.split("\\|".toRegex()
        ).dropLastWhile { it.isEmpty() }.toTypedArray()

        val versions = HashMap<String, Int>()
        for (elem in data) {
            val ld = elem.split(",".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()
            versions[ld[0]] = ld[2].toInt()
        }

        return versions
    }

    private fun getText(url: URL): String {
        val input = BufferedReader(InputStreamReader(url.openStream()))
        val response = StringBuilder()

        var inputLine: String?
        do {
            inputLine = input.readLine()
            if (inputLine == null)
                break
            response.append(inputLine)
        } while (true)

        input.close()
        return response.toString()
    }
}