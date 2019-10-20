package com.guiness.bot.controllers

import com.guiness.bot.netwotk.Proxy
import com.guiness.bot.netwotk.ProxyClientContext
import com.guiness.bot.netwotk.shared.HandlerPriority
import com.guiness.bot.netwotk.shared.PipelineOperation
import com.guiness.bot.netwotk.shared.StreamOperation
import com.guiness.bot.netwotk.shared.annotations.Controller
import com.guiness.bot.netwotk.shared.annotations.FromDownstream
import com.guiness.bot.protocol.messages.custom.AuthServerInfoMsg
import reactor.netty.tcp.TcpClient

@Controller
class NativeController {

    @FromDownstream(then = StreamOperation.DISCARD, priority = HandlerPriority.HIGHTEST, pipeline = PipelineOperation.BREAK)
    fun onLoginConnection(ctx: ProxyClientContext, msg: AuthServerInfoMsg) {
        TcpClient.create()
            .host(msg.ip)
            .port(msg.port)
            .doOnConnected {
                Proxy.addHandlers(it, upstream = true)
                ctx.attach(it)
            }
            .handle { inbound, _ -> Proxy.upstreamHandler(inbound, ctx) }
            .connect()
            .subscribe()
    }
}