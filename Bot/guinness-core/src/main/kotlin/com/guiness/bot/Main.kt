package com.guiness.bot

import com.guiness.bot.core.NativeAPI
import com.guiness.bot.netwotk.Proxy
import com.guiness.bot.netwotk.ProxyMessageHandler
import com.guiness.bot.protocol.DofusProtocol
import com.guiness.bot.core.ScriptLoader
import com.guinness.api.AIScript
import java.nio.file.Files
import java.nio.file.Paths
import kotlin.reflect.KClass

class Main

/**
 * Loads meta data when app starts
 */
fun loadSingletonsEagerly() {
    DofusProtocol
    ProxyMessageHandler
    ScriptLoader
}


fun main(args: Array<String>) {
    loadSingletonsEagerly()
    NativeAPI.patchProxyPort(5555)

    Proxy.withHost("127.0.0.1", 5555)
        .withWorker(1, 4)
        .start()

    //TODO: Application.launch(UserInterface::class.java)
}