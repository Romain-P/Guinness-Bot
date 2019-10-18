package com.guiness.bot.protocol

import com.guiness.bot.protocol.annotations.Message
import kotlin.reflect.KClass
import kotlin.reflect.full.primaryConstructor

data class MetaMessage(
    val annot: Message,
    val klass: KClass<*>,
    val fields: MutableList<MetaMessageField> = mutableListOf()
) {
    fun createInstance(vararg args: Any?) = klass.primaryConstructor!!.call(*args)
}