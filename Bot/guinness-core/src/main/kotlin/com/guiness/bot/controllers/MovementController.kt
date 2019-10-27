package com.guiness.bot.controllers

import com.guiness.bot.netwotk.ProxyClientContext
import com.guiness.bot.netwotk.shared.StreamOperation
import com.guiness.bot.netwotk.shared.annotations.Controller
import com.guiness.bot.netwotk.shared.annotations.FromUpstream
import com.guiness.bot.protocol.messages.game.*
import com.guiness.bot.services.MapService
import com.guiness.bot.services.SwfService

@Controller
class MovementController {

    @FromUpstream(then = StreamOperation.FORWARD)
    fun onMapChanged(ctx: ProxyClientContext, msg: MapDataMessage) {
        val map = MapService.loadMap(msg.mapId, msg.date, msg.privateKey)
        val previous = ctx.bot().session.map

        ctx.bot().session.map = map

        if (previous != null)
            ctx.bot().spreadEvent { it.onMapChanged(previous, map) }
    }
}