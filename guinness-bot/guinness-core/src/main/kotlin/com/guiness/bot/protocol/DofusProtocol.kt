package com.guiness.bot.protocol

import com.guiness.bot.core.intValue
import com.guiness.bot.core.longValue
import com.guiness.bot.core.stringValue
import com.guiness.bot.protocol.annotations.*
import io.netty.buffer.ByteBuf
import io.netty.buffer.Unpooled
import java.lang.RuntimeException
import kotlin.reflect.KClass
import kotlin.reflect.full.*
import org.reflections.Reflections


fun ByteBuf.utf8(): String = this.toString(Charsets.UTF_8)

object DofusProtocol {
    val CLIENT_DELIMITER: ByteBuf = Unpooled.wrappedBuffer(byteArrayOf(0xa, 0x0))
    val SERVER_DELIMITER: ByteBuf = Unpooled.wrappedBuffer(byteArrayOf(0x0))

    private val fromUpstreamMessages: Map<Char, MessageNode>
    private val fromDownstreamMessages: Map<Char, MessageNode>

    init {
        val reflections = Reflections("com.guiness.bot.protocol.messages")
        val classes = reflections.getTypesAnnotatedWith(Message::class.java).map { it.kotlin }

        fromUpstreamMessages = buildMessageTree(classes, StreamSource.UPSTREAM)
        fromDownstreamMessages = buildMessageTree(classes, StreamSource.DOWNSTREAM)
    }

    fun deserialize(packet: String, source: StreamSource): Any? {
        val meta = findMessage(packet, source) ?: return null

        val delim = meta.annot.delimiter
        val data = packet.substring(meta.annot.header.length)
        val reader = PacketReader(data, delim)

        return deserialize(reader, msg = meta)
    }

    fun serialize(message: Any): String? {
        val annot: Message = message::class.findAnnotation() ?: return null
        val header = annot.header
        val meta = findMessage(header, annot.source) ?: return null
        val packet = StringBuilder(header)

        serialize(packet, message, msg = meta)

        return packet.toString()
    }

    private fun serialize(packet: StringBuilder, instance: Any, msg: MetaMessage? = null, obj: MetaObject? = null) {
        val fields = msg?.fields ?: obj?.fields ?: throw RuntimeException("so bad")
        val delim = msg?.annot?.delimiter ?: obj?.delim ?: throw RuntimeException("so bad")

        for (i in fields.indices) {
            val field = fields[i]
            val value = field.javaField.get(instance)
            val isHexadecimal = field.hex

            if (i > 0)
                packet.append(delim)

            when (field.type) {
                MetaMessageFieldType.INT    -> packet.append((value as Int?)?.stringValue(isHexadecimal) ?: "")
                MetaMessageFieldType.LONG   -> packet.append((value as Long?)?.stringValue(isHexadecimal) ?: "")
                MetaMessageFieldType.OBJECT -> serialize(packet, value ?: "", obj = field.metaObject)
                MetaMessageFieldType.ARRAY  -> serializeArray(packet, field, value ?: "")
                else                        -> packet.append(value ?: "")
            }
        }
    }

    private fun serializeArray(packet: StringBuilder, field: MetaMessageField, value: Any) {
        val array = value as Array<Any?>

        for (i in array.indices) {
            val elem = array[i]
            val isHexadecimal = field.genericTypeHex!!

            if (i > 0)
                packet.append(field.delimiter)

            when(field.genericType) {
                MetaMessageFieldType.INT    -> packet.append((elem as Int?)?.stringValue(isHexadecimal) ?: "")
                MetaMessageFieldType.LONG   -> packet.append((elem as Long?)?.stringValue(isHexadecimal) ?: "")
                MetaMessageFieldType.OBJECT -> serialize(packet, elem ?: "", obj = field.metaObject)
                MetaMessageFieldType.ARRAY  -> {/** should not happen, array of array not supported **/}
                else                        -> packet.append(elem ?: "")
            }
        }
    }

    private fun deserialize(reader: PacketReader, msg: MetaMessage? = null, obj: MetaObject? = null): Any {
        val deserialized = mutableListOf<Any?>()

        val fields = msg?.fields ?: obj?.fields ?: throw RuntimeException("so bad")

        for (i in fields.indices) {
            val param = fields[i]
            val isHexadecimal = param.hex

            val serializedParam = when(param.type) {
                MetaMessageFieldType.ARRAY -> null
                MetaMessageFieldType.OBJECT -> null
                else -> reader.readNext()
                    ?: when (param.genericTypeNullable) {
                        false -> "01234567890"
                        else -> null
                    }
            }

            when (param.type) {
                MetaMessageFieldType.STRING -> deserialized.add(serializedParam)
                MetaMessageFieldType.LONG   -> deserialized.add(serializedParam?.longValue(isHexadecimal))
                MetaMessageFieldType.INT    -> deserialized.add(serializedParam?.intValue(isHexadecimal))
                MetaMessageFieldType.FLOAT  -> deserialized.add(serializedParam?.toFloat())
                MetaMessageFieldType.CHAR   -> deserialized.add(serializedParam?.get(0))
                MetaMessageFieldType.OBJECT -> deserialized.add(deserialize(reader.child(param.delimiter!!), obj = param.metaObject))
                MetaMessageFieldType.ARRAY  -> deserialized.add(deserializeArray(reader.child(param.delimiter!!), param))
            }
        }

        return msg?.createInstance(*deserialized.toTypedArray()) ?: obj!!.createInstance(*deserialized.toTypedArray())
    }

    private fun deserializeArray(reader: PacketReader, param: MetaMessageField): Array<*> {
        val size = param.arraySize
        val deserialized = ArrayList<Any?>()
        var i = 0

        do {
            val element = when(param.genericType) {
                MetaMessageFieldType.ARRAY  -> null
                MetaMessageFieldType.OBJECT -> null
                else  -> reader.readNext()
                    ?: when (param.genericTypeNullable) {
                        false -> "01234567890"
                        else -> null
                    }
            }

            val isHexadecimal = param.genericTypeHex!!

            when (param.genericType) {
                MetaMessageFieldType.STRING -> deserialized.add(element)
                MetaMessageFieldType.LONG   -> deserialized.add(element?.longValue(isHexadecimal))
                MetaMessageFieldType.INT    -> deserialized.add(element?.intValue(isHexadecimal))
                MetaMessageFieldType.FLOAT  -> deserialized.add(element?.toFloat())
                MetaMessageFieldType.CHAR   -> deserialized.add(element?.get(0))
                MetaMessageFieldType.OBJECT -> deserialized.add(deserialize(reader.child(param.metaObject!!.delim), obj = param.metaObject))
                MetaMessageFieldType.ARRAY  -> {/** should not happen, array of array not supported **/}
            }
        } while (if (size != null) (++i < size) else reader.hasNext())

        val cast = java.lang.reflect.Array.newInstance(param.genericTypeClass!!.java, deserialized.size) as Array<Any?>
        System.arraycopy(deserialized.toTypedArray(), 0, cast, 0, deserialized.size)

        return cast
    }

    private fun findMessage(packet: String, source: StreamSource): MetaMessage? {
        var msg: MetaMessage? = null
        var node: MessageNode? = null

        val messages = if (source == StreamSource.DOWNSTREAM)
            fromDownstreamMessages
        else
            fromUpstreamMessages

        for (char in packet) {
            node = (if (node == null) messages[char] else node.nodes[char]) ?: break

            if (node.message != null)
                msg = node.message
        }

        return msg
    }

    private fun buildMessageTree(classes: List<KClass<*>>, source: StreamSource): Map<Char, MessageNode> {
        val messages = HashMap<Char, MessageNode>()

        for (klass in classes) {
            val annot: Message = klass.findAnnotation()
                ?: throw RuntimeException("Not annotation found on packet ${klass.qualifiedName}")
            if (annot.source != source && annot.source != StreamSource.ANYSTREAM) continue

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
        return messages
    }

    private fun generateMetadata(klass: KClass<*>, msg: MetaMessage? = null, obj: MetaObject? = null) {
        val fields = klass.primaryConstructor?.parameters
            ?: throw RuntimeException("No constructor found for meta object or message ${klass.qualifiedName}")

        val parentDelimiter: String = msg?.annot?.delimiter ?: obj?.delim!!

        for (i in fields.indices) {
            val field = fields[i]
            val typeClass = field.type.classifier as KClass<*>
            val nullable = field.type.isMarkedNullable
            val hex = field.type.findAnnotation<Hex>() != null
            val type = metaTypeOf(typeClass)
            var genericType: MetaMessageFieldType? = null
            var genericTypeClass: KClass<*>? = null
            var genericTypeNullable: Boolean? = null
            var genericTypeHex = false
            var metaObject: MetaObject? = null
            var fieldDelimiter: String? = null
            val arraySize: Int? = field.findAnnotation<Size>()?.size

            /** we do this call now so don't have to do it later for each packet received **/
            val javaField = klass.javaObjectType.getDeclaredField(field.name!!)
            javaField.isAccessible = true

            /** resolve delimiter of object/array **/
            if (type == MetaMessageFieldType.OBJECT || type == MetaMessageFieldType.ARRAY) {
                val annotDelimiter: String? = field.findAnnotation<Delimiter>()?.delimiter
                    ?: typeClass.findAnnotation<Delimiter>()?.delimiter

                fieldDelimiter = annotDelimiter ?: parentDelimiter
            }

            /** fetch meta data of object/array **/
            when (type) {
                MetaMessageFieldType.OBJECT -> metaObject = resolveNesteadMetaObject(typeClass, fieldDelimiter!!)
                MetaMessageFieldType.ARRAY -> {
                    val genericParam = field.type.arguments.getOrNull(0)?.type

                    genericTypeClass = field.type.arguments.getOrNull(0)?.type?.classifier as KClass<*>?
                        ?: throw RuntimeException("Expected a generic type on type Array for meta object or message ${klass.qualifiedName}")
                    genericType = metaTypeOf(genericTypeClass)
                    genericTypeNullable = genericParam?.isMarkedNullable
                    genericTypeHex = genericParam?.findAnnotation<Hex>() != null

                    val genericTypeDelimiter = genericTypeClass.findAnnotation<Delimiter>()?.delimiter ?: parentDelimiter

                    when (genericType) {
                        MetaMessageFieldType.CHAR   -> genericTypeClass = Character::class
                        MetaMessageFieldType.LONG   -> genericTypeClass = Long::class
                        MetaMessageFieldType.INT    -> genericTypeClass = Integer::class
                        MetaMessageFieldType.FLOAT  -> genericTypeClass = Float::class
                        MetaMessageFieldType.OBJECT -> metaObject = resolveNesteadMetaObject(genericTypeClass, genericTypeDelimiter)
                        MetaMessageFieldType.ARRAY  -> throw RuntimeException("Arrays of Arrays are not supported, create a meta object instead (${klass.qualifiedName})")
                    }
                }
            }

            msg?.fields?.add(MetaMessageField(javaField, type, nullable, hex, genericType, genericTypeNullable, genericTypeClass, genericTypeHex, metaObject, fieldDelimiter, arraySize))
                ?: obj?.fields?.add(MetaMessageField(javaField, type, nullable, hex, genericType, genericTypeNullable, genericTypeClass, genericTypeHex, metaObject, fieldDelimiter, arraySize))
        }
    }

    private fun resolveNesteadMetaObject(metaObjectClass: KClass<*>, delimiter: String): MetaObject {
        val metaObject = MetaObject(delimiter, metaObjectClass)
        generateMetadata(metaObjectClass, obj = metaObject)

        return metaObject
    }

    private fun metaTypeOf(klass: KClass<*>): MetaMessageFieldType {
        val i = Int

        return when (klass) {
            String::class       -> MetaMessageFieldType.STRING
            Int::class          -> MetaMessageFieldType.INT
            Long::class         -> MetaMessageFieldType.LONG
            Float::class        -> MetaMessageFieldType.FLOAT
            Char::class         -> MetaMessageFieldType.CHAR
            IntArray::class     -> MetaMessageFieldType.ARRAY
            LongArray::class    -> MetaMessageFieldType.ARRAY
            CharArray::class    -> MetaMessageFieldType.ARRAY
            FloatArray::class   -> MetaMessageFieldType.ARRAY
            else                -> {
                if (klass.typeParameters.isEmpty())       /** if no generic type found **/
                    MetaMessageFieldType.OBJECT
                else                                      /** if generic type found **/
                    MetaMessageFieldType.ARRAY
            }
        }
    }
}