package com.guiness.bot.protocol

import kotlin.reflect.KClass
import kotlin.reflect.full.primaryConstructor

data class MetaObject(
    val delim: String,
    val klass: KClass<*>,
    val fields: MutableList<MetaMessageField> = mutableListOf()
) {
    fun createInstance(vararg args: Any?) = klass.primaryConstructor!!.call(*args)
}