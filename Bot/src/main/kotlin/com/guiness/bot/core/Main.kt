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
}