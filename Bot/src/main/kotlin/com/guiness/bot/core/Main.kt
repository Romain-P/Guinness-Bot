package com.guiness.bot.core

import com.guiness.bot.external.NativeAPI
import com.guiness.bot.netwotk.Proxy
import com.guiness.bot.netwotk.ProxyMessageHandler
import com.guiness.bot.protocol.DofusProtocol
import com.guiness.bot.protocol.MetaMessage
import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.messages.custom.GuildMember
import com.guiness.bot.protocol.messages.custom.Item
import com.guiness.bot.protocol.messages.custom.TestMsg
import kotlin.reflect.full.findAnnotation


class Main

/**
 * Loads meta data when app starts
 */
fun loadSingletonsEagerly() {
    DofusProtocol
    ProxyMessageHandler
}

fun main(args: Array<String>) {
    loadSingletonsEagerly()
    NativeAPI.patchProxyPort(5555)
    BotManager.connect(ProfileManager.getDefaultProfile().accounts.values.toList())

    Proxy.withHost("127.0.0.1", 5555)
        .withWorker(1, 4)
        .start()
}