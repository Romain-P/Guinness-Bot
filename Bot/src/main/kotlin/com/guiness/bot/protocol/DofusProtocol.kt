package com.guiness.bot.protocol

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.Delimiter
import com.guiness.bot.protocol.messages.custom.TestMsg
import io.netty.buffer.ByteBuf
import io.netty.buffer.Unpooled
import java.lang.RuntimeException
import kotlin.reflect.KClass
import kotlin.reflect.full.*

fun ByteBuf.utf8(): String = this.toString(Charsets.UTF_8)

object DofusProtocol {
    val CLIENT_DELIMITER: ByteBuf = Unpooled.wrappedBuffer(byteArrayOf(0xa, 0x0))
    val SERVER_DELIMITER: ByteBuf = Unpooled.wrappedBuffer(byteArrayOf(0x0))

    val messages: MutableMap<Char, MessageNode> = HashMap()
    val classes: List<KClass<*>> = listOf(TestMsg::class)

    fun deserialize(packet: String): Any? {
        val meta = findMessage(packet) ?: return null

        val delim = meta.annot.delimiter
        val data = packet.substring(meta.annot.header.length)

        return deserialize(data, delim, msg = meta)
    }

    fun serialize(message: Any): String? {
        val annot: Message = message::class.findAnnotation() ?: return null
        val header = annot.header
        val meta = findMessage(header) ?: return null
        val packet = StringBuilder(header)

        serialize(packet, message, msg = meta)

        return packet.toString()
    }

    fun serialize(packet: StringBuilder, instance: Any, msg: MetaMessage? = null, obj: MetaObject? = null) {
        val fields = msg?.fields ?: obj?.fields ?: throw RuntimeException("so bad")
        val delim = msg?.annot?.delimiter ?: obj?.delim ?: throw RuntimeException("so bad")

        for (i in fields.indices) {
            val field = fields[i]
            val value = field.javaField.get(instance)

            if (i > 0)
                packet.append(delim)

            when (field.type) {
                MetaMessageFieldType.STRING -> packet.append(value)
                MetaMessageFieldType.INT -> packet.append(value as Int)
                MetaMessageFieldType.FLOAT -> packet.append(value as Float)
                MetaMessageFieldType.CHAR -> packet.append(value as Char)
                MetaMessageFieldType.OBJECT -> serialize(packet, value, obj = field.metaObject)
                MetaMessageFieldType.ARRAY -> serializeArray(packet, field, value)
            }
        }
    }

    private fun serializeArray(packet: StringBuilder, field: MetaMessageField, value: Any) {
        val array = value as Array<Any>

        for (i in array.indices) {
            val elem = array[i]

            if (i > 0)
                packet.append(field.delimiter)

            when(field.genericType) {
                MetaMessageFieldType.STRING -> packet.append(elem)
                MetaMessageFieldType.INT -> packet.append(elem as Int)
                MetaMessageFieldType.FLOAT -> packet.append(elem as Float)
                MetaMessageFieldType.CHAR -> packet.append(elem as Char)
                MetaMessageFieldType.OBJECT -> serialize(packet, elem, obj = field.metaObject)
                MetaMessageFieldType.ARRAY  -> {/** should not happen, array of array not supported **/}
            }
        }
    }

    private fun deserialize(data: String, delim: String, msg: MetaMessage? = null, obj: MetaObject? = null): Any {
        val deserialized = mutableListOf<Any>()
        val serialized = data.split(delim)

        val fields = msg?.fields ?: obj?.fields ?: throw RuntimeException("so bad")

        for (i in fields.indices) {
            val serializedParam = if (i < serialized.size) serialized[i] else "0123456789" /** not enough data **/
            val param = fields[i]

            when (param.type) {
                MetaMessageFieldType.STRING -> deserialized.add(serializedParam)
                MetaMessageFieldType.INT    -> deserialized.add(serializedParam.toInt())
                MetaMessageFieldType.FLOAT  -> deserialized.add(serializedParam.toFloat())
                MetaMessageFieldType.CHAR   -> deserialized.add(serializedParam[0])
                MetaMessageFieldType.OBJECT -> deserialized.add(deserialize(serializedParam, param.metaObject!!.delim, obj = param.metaObject))
                MetaMessageFieldType.ARRAY  -> deserialized.add(deserializeArray(serializedParam, param))
            }
        }

        return msg?.createInstance(*deserialized.toTypedArray()) ?: obj!!.createInstance(*deserialized.toTypedArray())
    }

    private fun deserializeArray(serializedParam: String, param: MetaMessageField): Array<*> {
        val delimiter = param.delimiter!!
        val data = serializedParam.split(delimiter)

        val deserialized = java.lang.reflect.Array.newInstance(param.genericTypeClass!!.java, data.size) as Array<Any>
        for (i in data.indices) {
            val element = data[i]

            when (param.genericType) {
                MetaMessageFieldType.STRING -> deserialized[i] = element
                /** INT and CHAR arrays are automatically converted to Array<Integer> and Array<Char>
                 *  TODO: improve it with native types (check @generateMetadata#genericTypeClass)
                 */
                MetaMessageFieldType.INT    -> deserialized[i] = element.toInt()
                MetaMessageFieldType.FLOAT  -> deserialized[i] = element.toFloat()
                MetaMessageFieldType.CHAR   -> deserialized[i] = element[0]
                MetaMessageFieldType.OBJECT -> deserialized[i] = deserialize(element, param.metaObject!!.delim, obj = param.metaObject)
                MetaMessageFieldType.ARRAY  -> {/** should not happen, array of array not supported **/}
            }
        }

        return deserialized
    }

    private fun findMessage(packet: String): MetaMessage? {
        var msg: MetaMessage? = null
        var node: MessageNode? = null

        for (char in packet) {
            node = (if (node == null) messages[char] else node.nodes[char]) ?: break

            if (node.message != null)
                msg = node.message
        }

        return msg
    }

    fun initNodes() {
        for (klass in classes) {
            val annot: Message = klass.findAnnotation()
                ?: throw RuntimeException("Not annotation found on packet ${klass.qualifiedName}")
            var node: MessageNode? = null

            for (i in annot.header.indices) {
                val c = annot.header[i]

                if (node == null) {
                    when (val firstNode = messages[c]) {
                        null -> {
                            node = MessageNode()
                            messages[c] = node
                        }
                        else -> node = firstNode
                    }
                } else if (!node.nodes.containsKey(c)) {
                    val newNode = MessageNode()
                    node.nodes[c] = newNode
                    node = newNode
                } else
                    node = node.nodes[c]!!

                if (i == annot.header.length - 1) {
                    if (node.message != null)
                        throw RuntimeException(
                            "Found several packet with same header: " +
                                    "\n${klass.qualifiedName}\n${node.message!!.klass.qualifiedName}\n"
                        )
                    val metaMessage = MetaMessage(annot, klass)
                    node.message = metaMessage
                    generateMetadata(klass, msg = metaMessage)
                }
            }
        }
    }

    private fun generateMetadata(klass: KClass<*>, msg: MetaMessage? = null, obj: MetaObject? = null) {
        val fields = klass.primaryConstructor?.parameters
            ?: throw RuntimeException("No constructor found for meta object or message ${klass.qualifiedName}")

        for (field in fields) {
            val typeClass = field.type.classifier as KClass<*>
            val type = metaTypeOf(typeClass)
            var genericType: MetaMessageFieldType? = null
            var genericTypeClass: KClass<*>? = null
            var metaObject: MetaObject? = null
            var fieldAnnotation: Delimiter? = null

            /** we do this call now so don't have to do it later for each packet received **/
            val javaField = klass.javaObjectType.getDeclaredField(field.name!!)
            javaField.isAccessible = true

            when (type) {
                MetaMessageFieldType.OBJECT -> metaObject = resolveNesteadMetaObject(typeClass)
                MetaMessageFieldType.ARRAY -> {
                    fieldAnnotation = field.findAnnotation()
                        ?: throw RuntimeException("Expected an annotation on type Array for meta object or message ${klass.qualifiedName}")

                    genericTypeClass = field.type.arguments.getOrNull(0)?.type?.classifier as KClass<*>?
                        ?: throw RuntimeException("Expected a generic type on type Array for meta object or message ${klass.qualifiedName}")
                    genericType = metaTypeOf(genericTypeClass)

                    when (genericType) {
                        MetaMessageFieldType.CHAR   -> genericTypeClass = Character::class
                        MetaMessageFieldType.INT    -> genericTypeClass = Integer::class
                        MetaMessageFieldType.FLOAT  -> genericTypeClass = Float::class
                        MetaMessageFieldType.OBJECT -> metaObject = resolveNesteadMetaObject(genericTypeClass)
                        MetaMessageFieldType.ARRAY  -> throw RuntimeException("Arrays of Arrays are not supported, create a meta object instead (${klass.qualifiedName})")
                    }
                }
            }

            msg?.fields?.add(MetaMessageField(javaField, type, genericType, genericTypeClass, metaObject, fieldAnnotation?.delimiter))
            obj?.fields?.add(MetaMessageField(javaField, type, genericType, genericTypeClass, metaObject, fieldAnnotation?.delimiter))
        }
    }

    private fun resolveNesteadMetaObject(metaObjectClass: KClass<*>): MetaObject {
        val classAnnotation: Delimiter = metaObjectClass.findAnnotation()
            ?: throw RuntimeException("Expected an annotation on meta object ${metaObjectClass.qualifiedName}")

        val metaObject = MetaObject(classAnnotation.delimiter, metaObjectClass)
        generateMetadata(metaObjectClass, obj = metaObject)

        return metaObject
    }

    private fun metaTypeOf(klass: KClass<*>): MetaMessageFieldType {
        return when (klass) {
            String::class       -> MetaMessageFieldType.STRING
            Int::class          -> MetaMessageFieldType.INT
            Float::class        -> MetaMessageFieldType.FLOAT
            Char::class         -> MetaMessageFieldType.CHAR
            IntArray::class     -> MetaMessageFieldType.ARRAY
            CharArray::class    -> MetaMessageFieldType.ARRAY
            else                -> {
                if (klass.typeParameters.isEmpty())       /** if no generic type found **/
                    MetaMessageFieldType.OBJECT
                else                                      /** if generic type found **/
                    MetaMessageFieldType.ARRAY
            }
        }
    }
}