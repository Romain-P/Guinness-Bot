package com.guiness.bot.netwotk

import com.guiness.bot.entities.Bot
import com.guiness.bot.core.Log
import reactor.netty.Connection

class ProxyClientContext(
    private val downstream: ProxyClientStream,
    private var upstream: ProxyClientStream? = null
) {
    private var bot: Bot? = null

    var state = ProxyClientState.AWAIT_HELLO
        set(value) {
            Proxy.log("state", value.name, Log.LoggingLevel.GOOD)
            field = value
        }

    fun uuid() = downstream.uuid
    fun downstream() = downstream
    fun upstream() = upstream!!
    fun upstreamMightBeNull() = upstream
    fun bot() = bot!!
    fun botMightBeNull() = bot

    fun attach(upstreamConnection: Connection) {
        this.upstream = ProxyClientStream("upstream", upstreamConnection)
    }

    fun attach(bot: Bot) {
        this.bot = bot
    }

    companion object {
        fun of(label: String, connection: Connection) = ProxyClientContext(ProxyClientStream(label, connection))
    }
}