package com.guiness.bot.core

import com.guiness.bot.protocol.DofusProtocol
import com.guiness.bot.protocol.MetaMessage
import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.messages.custom.TestMsg
import kotlin.reflect.full.findAnnotation


class Main

fun main(args: Array<String>) {
    //NativeAPI.patchProxyPort(5555)

    /**Proxy.withHost("127.0.0.1", 5555)
        .withWorker(1, 4)
        .start()**/
    DofusProtocol.initNodes()
    val message = DofusProtocol.deserialize(""".{AH}.127.0.0.1|443|1000:541|1-2-3-4-5|romain,1000:1.1001:12;maxime,1000:21""") as TestMsg
    println(message)
}