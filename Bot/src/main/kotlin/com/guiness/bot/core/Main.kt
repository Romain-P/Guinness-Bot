package com.guiness.bot.core

import com.guiness.bot.netwotk.Proxy
import com.guiness.bot.protocol.DofusProtocol


class Main

fun main(args: Array<String>) {
    //NativeAPI.patchProxyPort(5555)

    /**Proxy.withHost("127.0.0.1", 5555)
        .withWorker(1, 4)
        .start()**/
    DofusProtocol.initNodes()
    println("ok")
}