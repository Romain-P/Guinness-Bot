package com.guiness.bot.core

import java.lang.RuntimeException
import kotlin.reflect.KClass

typealias ProcessID = Int
typealias AccountName = String
typealias CharacterName = String
typealias ProfileAlias = String
typealias ChannelID = String
typealias ScriptName = String
typealias MessageKClass = KClass<*>

fun String.longValue(hex: Boolean) = when (hex) {
    true -> this.toLong(radix = 16)
    false -> this.toLong()
}

fun String.intValue(hex: Boolean) = when (hex) {
    true -> this.toInt(radix = 16)
    false -> this.toInt()
}

fun Int.stringValue(hex: Boolean) = when (hex) {
    true -> this.toString(radix = 16)
    false -> this.toString()
}

fun Long.stringValue(hex: Boolean) = when (hex) {
    true -> this.toString(radix = 16)
    false -> this.toString()
}

inline fun <reified V> Map<*, *>.value(key: String): V {
    return when (val value = this[key]) {
        is V    -> value
        else    -> throw RuntimeException("Invalid cast")
    }
}