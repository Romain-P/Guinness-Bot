package com.guiness.bot

import com.guiness.bot.core.NativeAPI
import com.guiness.bot.netwotk.Proxy
import com.guiness.bot.netwotk.ProxyMessageHandler
import com.guiness.bot.protocol.DofusProtocol


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

    Proxy.withHost("127.0.0.1", 5555)
        .withWorker(1, 4)
        .start()

    //TODO: Application.launch(UserInterface::class.java)
}