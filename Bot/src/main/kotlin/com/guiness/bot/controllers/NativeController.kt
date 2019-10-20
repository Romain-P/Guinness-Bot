package com.guiness.bot.controllers

import com.guiness.bot.netwotk.Proxy
import com.guiness.bot.netwotk.ProxyClientContext
import com.guiness.bot.netwotk.shared.HandlerPriority
import com.guiness.bot.netwotk.shared.PipelineOperation
import com.guiness.bot.netwotk.shared.StreamOperation
import com.guiness.bot.netwotk.shared.annotations.Controller
import com.guiness.bot.netwotk.shared.annotations.FromDownstream
import com.guiness.bot.protocol.messages.custom.AuthServerInfoMsg
import com.guiness.bot.protocol.messages.custom.GameServerInfoMsg

@Controller
class NativeController {

    @FromDownstream(then = StreamOperation.DISCARD, priority = HandlerPriority.HIGHTEST, pipeline = PipelineOperation.BREAK)
    fun onLoginConnection(ctx: ProxyClientContext, msg: AuthServerInfoMsg) {
        Proxy.connectToUpstream(ctx, msg.ip, msg.port)
    }

    @FromDownstream(then = StreamOperation.DISCARD, priority = HandlerPriority.HIGHTEST, pipeline = PipelineOperation.BREAK)
    fun onGameConnection(ctx: ProxyClientContext, msg: GameServerInfoMsg) {
        Proxy.connectToUpstream(ctx, msg.ip, msg.port)
    }
}