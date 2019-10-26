package com.guiness.bot.services.util

import com.guiness.bot.core.intValue
import com.guiness.bot.protocol.entities.map.CELL
import com.guiness.bot.protocol.entities.map.MAP
import kotlin.math.ceil
import kotlin.math.cos
import kotlin.math.sin

object MapUtil {
    private val HASH = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_".toCharArray()
    private val HEX_CHAR = "0123456789ABCDEF".toCharArray()
    private val PId4 = Math.PI / 4
    private val COS_PId4 = cos(PId4)
    private val SIN_PId4 = sin(PId4)
    private val COS_mPId4 = COS_PId4
    private val SIN_mPId4 = -SIN_PId4

    fun decryptMapData(data: String, privateKey: String): String {
        val preparedKey = prepareKey(privateKey)
        val checksum = checksum(preparedKey).toString().intValue(true) * 2
        val sb = StringBuilder()
        var i = 0
        while (i < data.length) {
            val a = data.substring(i, i + 2).intValue(true)
            val b = preparedKey[(i / 2 + checksum) % preparedKey.length].toInt()
            sb.append((a xor b).toChar())
            i += 2
        }
        return decode(sb.toString())
    }

    fun uncompressCells(map: MAP, d: String): Array<CELL> {
        val data = IntArray(d.length)
        for (i in data.indices)
            data[i] = indexOfHash(d[i])

        val active = data[0] and 32 shr 5 == 1
        val cells = arrayOfNulls<CELL>(data.size / 10)
        for (i in 0 until data.size / 10)
            cells[i] = uncompressCell(map, i, data, active)
        return cells as Array<CELL>
    }

    private fun prepareKey(key: String): String {
        val sb = StringBuilder()
        var i = 0
        while (i < key.length) {
            sb.append(key.substring(i, i + 2).intValue(true).toChar())
            i += 2
        }
        return decode(sb.toString())
    }

    private fun checksum(s: String): Char {
        var v = 0
        for (c in s)
            v += c.toInt() and 15
        return HEX_CHAR[v and 15]
    }

    private fun indexOfHash(ch: Char): Int {
        for (i in HASH.indices)
            if (HASH[i] == ch)
                return i
        throw ArrayIndexOutOfBoundsException("$ch is not in hash array")
    }

    private fun uncompressCell(map: MAP, id: Int, data: IntArray, active: Boolean): CELL {
        val index = id * 10
        val lineOfSight = data[index] and 1 == 1
        val layerGroundRot = data[index + 1] and 48 shr 4
        val groundLevel = data[index + 1] and 15
        val movement = data[index + 2] and 56 shr 3
        val layerGroundNum = (data[index] and 24 shl 6) + (data[index + 2] and 7 shl 6) + data[index + 3]
        val groundSlope = data[index + 4] and 60 shr 2
        val layerGroundFlip = data[index + 4] and 2 shr 1 == 1
        val layerObject1Num =
            (data[index] and 4 shl 11) + (data[index + 4] and 1 shl 12) + (data[index + 5] shl 6) + data[index + 6]
        val layerObject1Rot = data[index + 7] and 48 shr 4
        val layerObject1Flip = data[index + 7] and 8 shr 3 == 1
        val layerObject2Flip = data[index + 7] and 4 shr 2 == 1
        val layerObject2Interactive = data[index + 7] and 2 shr 1 == 1
        val layerObject2Num =
            (data[index] and 2 shl 12) + (data[index + 7] and 1 shl 12) + (data[index + 8] shl 6) + data[index + 9]

        return CELL(
            map,
            id,
            active,
            lineOfSight,
            layerGroundRot,
            groundLevel,
            movement,
            layerGroundNum,
            groundSlope,
            layerGroundFlip,
            layerObject1Num,
            layerObject1Rot,
            layerObject1Flip,
            layerObject2Flip,
            layerObject2Interactive,
            layerObject2Num,
            getX(id, map.width),
            getY(id, map.width),
            getXRotated(id, map.width, map.height),
            getYRotated(id, map.width, map.height)
        )
    }

    private fun decode(s: String): String {
        if (s.indexOf('%') == -1 && s.indexOf('+') == -1)
            return s

        val len = s.length.toLong()
        val sb = StringBuilder()
        var ch: Char

        var i = 0
        while (i < len) {
            ch = s[i]
            if (ch == '%' && i + 2 < len && s[i + 1] != '%') {
                if (s[i + 1] == 'u' && i + 5 < len) {
                    // unicode hex sequence
                    try {
                        sb.append(s.substring(i + 2, i + 4).intValue(true).toChar())
                        i += 2
                    } catch (e: NumberFormatException) {
                        sb.append('%')
                    }

                } else {
                    try {
                        sb.append(s.substring(i + 1, i + 3).intValue(true).toChar())
                        i += 2
                    } catch (e: NumberFormatException) {
                        sb.append('%')
                    }
                }
                i++
                continue
            }

            if (ch == '+')
                sb.append(' ')
            else
                sb.append(ch)
            i++
        }
        return sb.toString()
    }

    fun getId(x: Int, y: Int, width: Int) = (y * (width - 0.5) + x / 2.0).toInt()
    fun getY(id: Int, width: Int) = (id / (width - 0.5)).toInt()
    fun getX(id: Int, width: Int) = (id % (width - 0.5) * 2).toInt()

    fun getXRotated(id: Int, width: Int, height: Int): Int {
        val x = getX(id, width) - width
        val y = getY(id, width) - height
        return ceil((x * COS_PId4 - y * SIN_PId4 - 0.25) * 0.7).toInt() + width
    }

    fun getYRotated(id: Int, width: Int, height: Int): Int {
        val x = getX(id, width) - width
        val y = getY(id, width) - height
        return ceil((x * SIN_PId4 + y * COS_PId4 - 1.75) * 0.7).toInt() + height
    }
}