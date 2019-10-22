package com.guiness.bot.protocol

import java.lang.reflect.Field
import kotlin.reflect.KClass

data class MetaMessageField(
    val javaField: Field,
    val type: MetaMessageFieldType,
    val nullable: Boolean,
    val genericType: MetaMessageFieldType?,
    val genericTypeNullable: Boolean?,
    val genericTypeClass: KClass<*>?,
    val metaObject: MetaObject?, /** present if type or genericType == OBJECT **/
    val delimiter: String?,
    val arraySize: Int?
)