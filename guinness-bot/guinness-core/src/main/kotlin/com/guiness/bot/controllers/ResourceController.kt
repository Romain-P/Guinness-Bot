package com.guiness.bot.controllers

import com.guiness.bot.netwotk.ProxyClientContext
import com.guiness.bot.netwotk.shared.StreamOperation
import com.guiness.bot.netwotk.shared.annotations.Controller
import com.guiness.bot.netwotk.shared.annotations.FromUpstream
import com.guiness.bot.protocol.messages.game.basics.ChatMessageArgs
import com.guiness.bot.protocol.messages.game.basics.ChatReceiveMessage
import com.guiness.bot.protocol.messages.game.harvest.ResourceMessage
import com.kizitonwose.time.seconds

@Controller
class ResourceController {
    @FromUpstream(then = StreamOperation.FORWARD)
    fun onResourceUpdate(ctx: ProxyClientContext, msg: ResourceMessage): Boolean {
        if (msg.cellInfo == null)
            return false

        for (cellInfo in msg.cellInfo!!) {
            val cellId = cellInfo.cellId;
            val unknown = cellInfo.unknown;
            val status = cellInfo.status;

            if (status == 1)
                ctx.downstream().post(ChatReceiveMessage("@", 0, "Mineur", "Ressource récoltable en $cellId", null), 0.seconds);
        }
        return false
    }
}