package com.guiness.bot.protocol

import kotlin.reflect.KClass
import kotlin.reflect.full.primaryConstructor

data class MetaObject(
    val delim: String,
    val klass: KClass<*>,
    val fields: MutableList<MetaMessageField> = mutableListOf()
) {
    fun createInstance(vararg args: Any?) = when(args.size) {
        0    -> klass.primaryConstructor!!.call()
        else -> klass.primaryConstructor!!.call(*args)
    }
}