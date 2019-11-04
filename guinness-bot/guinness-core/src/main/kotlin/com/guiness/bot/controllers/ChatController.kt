package com.guiness.bot.controllers

import com.guiness.bot.netwotk.ProxyClientContext
import com.guiness.bot.netwotk.shared.StreamOperation
import com.guiness.bot.netwotk.shared.annotations.Controller
import com.guiness.bot.netwotk.shared.annotations.FromDownstream
import com.guiness.bot.netwotk.shared.annotations.FromUpstream
import com.guiness.bot.protocol.messages.game.basics.ChatReceiveMessage
import com.guiness.bot.protocol.messages.game.basics.ChatRequestMessage
import com.kizitonwose.time.seconds

@Controller
class ChatController {
    @FromDownstream(then = StreamOperation.DISCARD) //on intercepte et renvoie le packet
    fun onChatRequest(ctx: ProxyClientContext, msg: ChatRequestMessage) {
        //on envoie un second message 5 secondes plus tard
        ctx.upstream().post(ChatRequestMessage(msg.channel, msg.message), 0.seconds)
        //ctx.downstream().post()
    }

    @FromUpstream(then = StreamOperation.FORWARD) //on intercepte et renvoie le packet
    fun onChatReceive(ctx: ProxyClientContext, msg: ChatReceiveMessage) {
        println("Message from" + msg.type + " " + msg.sender + "(" + msg.unknown + ") : " + msg.message);
    }
}