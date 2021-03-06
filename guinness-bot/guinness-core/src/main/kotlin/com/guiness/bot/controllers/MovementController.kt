package com.guiness.bot.controllers

import com.guiness.bot.netwotk.ProxyClientContext
import com.guiness.bot.netwotk.shared.StreamOperation
import com.guiness.bot.netwotk.shared.annotations.Controller
import com.guiness.bot.netwotk.shared.annotations.FromUpstream
import com.guiness.bot.protocol.messages.game.*
import com.guiness.bot.protocol.messages.game.actions.requests.GameActionRequestMovement
import com.guiness.bot.services.MapService
import com.kizitonwose.time.seconds

@Controller
class MovementController {

    @FromUpstream(then = StreamOperation.FORWARD)
    fun onMapChanged(ctx: ProxyClientContext, msg: MapDataMessage) {
        val map = MapService.loadMap(msg.mapId, msg.date, msg.privateKey)

        val previous = ctx.bot().session.map
        ctx.bot().session.map = map
        ctx.bot().spreadEvents { onMapChanged(previous, map) } /* spread event to all scripts subscribed by the bot */

        val randomCell = map.cells.find { it.movement != 0 }

        //ctx.downstream().post("BD#${randomCell!!.id}", 2.seconds, unwrapped = true)
    }
}