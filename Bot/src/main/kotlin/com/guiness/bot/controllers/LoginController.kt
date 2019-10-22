package com.guiness.bot.controllers

import com.guiness.bot.netwotk.ProxyClientContext
import com.guiness.bot.netwotk.shared.StreamOperation
import com.guiness.bot.netwotk.shared.annotations.Controller
import com.guiness.bot.netwotk.shared.annotations.FromUpstream
import com.guiness.bot.protocol.messages.login.ServersKnownMsg

@Controller
class LoginController {
    @FromUpstream(then = StreamOperation.DISCARD) /** On bloque le packet, il sera pas retransmit au Downstream automatiquement **/
    fun onServerKnownMsg(ctx: ProxyClientContext, msg: ServersKnownMsg) {
        ctx.downstream().write(msg) /** on envoie le message reçu manuellement au downstream **/
        ctx.upstream().write(msg) /** on renvoie le packet à l'upstream, effet echo **/

        /** Une fois qu'on connait un packet que l'on veut envoyer, on procède ainsi **/
    }
}