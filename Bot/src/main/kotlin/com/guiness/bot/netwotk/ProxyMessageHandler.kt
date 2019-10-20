package com.guiness.bot.netwotk

import com.guiness.bot.core.MessageKClass
import com.guiness.bot.log.Log
import com.guiness.bot.log.logger
import com.guiness.bot.netwotk.shared.MessageHandler
import com.guiness.bot.netwotk.shared.PipelineOperation
import com.guiness.bot.netwotk.shared.StreamOperation
import com.guiness.bot.netwotk.shared.annotations.Controller
import com.guiness.bot.netwotk.shared.annotations.FromAnyStream
import com.guiness.bot.netwotk.shared.annotations.FromDownstream
import com.guiness.bot.netwotk.shared.annotations.FromUpstream
import com.guiness.bot.protocol.DofusProtocol
import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.StreamSource
import org.reflections.Reflections
import java.lang.RuntimeException
import kotlin.reflect.KClass
import kotlin.reflect.full.createInstance
import kotlin.reflect.full.declaredMemberFunctions
import kotlin.reflect.full.findAnnotation

object ProxyMessageHandler {
    private val upstreamHandlers: Map<MessageKClass, List<MessageHandler>>
    private val downStreamHandlers: Map<MessageKClass, List<MessageHandler>>
    private val log by logger()

    init {
        val reflections = Reflections("com.guiness.bot.controllers")
        val classes = reflections.getTypesAnnotatedWith(Controller::class.java).map { it.kotlin }

        val upstreamHandlers = HashMap<MessageKClass, MutableList<MessageHandler>>()
        val downStreamHandlers = HashMap<MessageKClass, MutableList<MessageHandler>>()

        resolveHandlers(classes, upstreamHandlers, downStreamHandlers)

        this.upstreamHandlers = upstreamHandlers
        this.downStreamHandlers = downStreamHandlers
    }

    fun onDownstreamReceive(ctx: ProxyClientContext, packet: String) {
        onReceive(packet, ctx, ctx.downstream(), ctx.upstreamMightBeNull(), downStreamHandlers)
    }

    fun onUpstreamReceive(ctx: ProxyClientContext, packet: String) {
        onReceive(packet, ctx, ctx.upstream(), ctx.downstream(), upstreamHandlers)
    }

    private fun onReceive(packet: String, ctx: ProxyClientContext, sourceStream: ProxyClientStream,
                          targetStream: ProxyClientStream?, handlersGroup: Map<MessageKClass, List<MessageHandler>>)
    {
        val message = DofusProtocol.deserialize(packet)

        Proxy.log(message ?: packet, source = sourceStream)

        if (message == null) {
            targetStream?.write(packet, true, true)?.flush()
            return
        }

        val handlers = handlersGroup[message::class]
        if (handlers == null || handlers.isEmpty()) {
            targetStream?.write(message, true, true)?.flush()
            return
        }

        var discard = false
        for (handler in handlers) {
            if (!discard && handler.then == StreamOperation.DISCARD)
                discard = true
            val returned = handler.handler.call(handler.controller, ctx, message)

            discard = discard || when (returned) {
                is Boolean -> returned
                else       -> handler.then == StreamOperation.DISCARD
            }
        }
        if (!discard && targetStream != null)
            targetStream.write(DofusProtocol.serialize(message)!!).flush()

        if (targetStream != null)
            ctx.upstream().flush()
        ctx.downstream().flush()
    }

    private fun resolveHandlers(classes: List<MessageKClass>,
                        upstreamHandlers: MutableMap<MessageKClass, MutableList<MessageHandler>>,
                        downStreamHandlers: MutableMap<MessageKClass, MutableList<MessageHandler>>)
    {
        for (klass in classes) {
            val controllerInstance = klass.createInstance()
            val methods = klass.declaredMemberFunctions

            for (method in methods) {
                val up = method.findAnnotation<FromUpstream>()
                val down = method.findAnnotation<FromDownstream>()
                val any = method.findAnnotation<FromAnyStream>()

                val priority = up?.priority ?: down?.priority ?: any?.priority ?: continue
                val then = up?.then ?: down?.then ?: any?.then ?: continue
                val pipeline = up?.pipeline ?: down?.pipeline ?: any?.pipeline ?: continue

                val parameters = method.parameters
                val messageClass: KClass<*> = when (parameters.size) {
                    3    -> method.parameters[2].type.classifier as KClass<*>
                    else -> throw RuntimeException("A packet handler must have 2 arguments. " +
                            "First argument is the context, the second is the handled packet " +
                            "(handler: ${klass.qualifiedName}#${method.name}")
                }

                val msgAnnot = messageClass.findAnnotation<Message>()
                    ?: throw RuntimeException("The handled message's class must be annotated " +
                            "(msg: ${messageClass.qualifiedName} - handler: ${klass.qualifiedName}#${method.name})")

                val exception = "Packet StreamSource must match with handler annotation From{Source} " +
                        "(msg: ${messageClass.qualifiedName} - handler: ${klass.qualifiedName}#${method.name})"

                when (msgAnnot.source) {
                    StreamSource.DOWNSTREAM -> down ?: any ?: throw RuntimeException(exception)
                    StreamSource.UPSTREAM   -> up ?: any ?: throw RuntimeException(exception)
                }


                if (up != null || any != null) {
                    upstreamHandlers.putIfAbsent(messageClass, ArrayList())
                    upstreamHandlers[messageClass]!!.add(MessageHandler(controllerInstance, method, priority, then, pipeline))
                    upstreamHandlers[messageClass]!!.sortByDescending { it.priority }
                }
                if (down != null || any != null) {
                    downStreamHandlers.putIfAbsent(messageClass, ArrayList())
                    downStreamHandlers[messageClass]!!.add(MessageHandler(controllerInstance, method, priority, then, pipeline))
                    downStreamHandlers[messageClass]!!.sortByDescending { it.priority }
                }
            }
        }
        removeUnusedHandlers(upstreamHandlers)
        removeUnusedHandlers(downStreamHandlers)
    }

    private fun removeUnusedHandlers(handlersGroup: MutableMap<MessageKClass, MutableList<MessageHandler>>) {
        for (handlers in handlersGroup.values) {
            val cpy = handlers.toMutableList()

            run loop@ {
                cpy.forEachIndexed { index, handler ->
                    if (handler.pipeline == PipelineOperation.BREAK && index != handlers.lastIndex) {
                        val unusedHandlers = handlers.subList(index + 1, handlers.size)

                        println("Warning: handler ${handler.controller::class.qualifiedName}#${handler.handler.name} " +
                                "is breaking pipeline. The following handlers won't be reached:")
                        for (unused in unusedHandlers)
                            println("[skipped] ${unused.controller::class.qualifiedName}#${unused.handler.name}")

                        unusedHandlers.clear()
                        return@loop
                    }
                }
            }
        }
    }
}