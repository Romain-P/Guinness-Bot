package com.guiness.bot.controllers

import com.guiness.bot.netwotk.ProxyClientContext
import com.guiness.bot.netwotk.shared.StreamOperation
import com.guiness.bot.netwotk.shared.annotations.Controller
import com.guiness.bot.netwotk.shared.annotations.FromUpstream
import com.guiness.bot.protocol.messages.login.ServersKnownMessage

@Controller
class LoginController {
    @FromUpstream(then = StreamOperation.FORWARD)
    fun onServerKnownMsg(ctx: ProxyClientContext, msg: ServersKnownMessage) {

    }
}