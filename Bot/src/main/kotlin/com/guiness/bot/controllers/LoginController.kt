package com.guiness.bot.controllers

import com.guiness.bot.netwotk.ProxyClientContext
import com.guiness.bot.netwotk.ProxyClientState
import com.guiness.bot.netwotk.shared.StreamOperation
import com.guiness.bot.netwotk.shared.annotations.Controller
import com.guiness.bot.netwotk.shared.annotations.FromUpstream
import com.guiness.bot.protocol.messages.game.CharacterListMessage
import com.guiness.bot.protocol.messages.game.MountGivenXpMessage
import com.guiness.bot.protocol.messages.game.SelectCharacterMessage
import com.guiness.bot.protocol.messages.login.SelectServerMessage
import com.guiness.bot.protocol.messages.login.ServersKnownMessage
import com.kizitonwose.time.seconds

@Controller
class LoginController {
    @FromUpstream(then = StreamOperation.FORWARD)
    fun onServerList(ctx: ProxyClientContext, msg: ServersKnownMessage) {
        /* prevents auto-login if no default character **/
        val serverId = ctx.botMightBeNull()?.account?.defaultCharacter?.serverId ?: return

        /* prevents mistakes */
        if (msg.servers.any { it.serverId == serverId })
            ctx.upstream().write(SelectServerMessage(serverId))
    }

    @FromUpstream(then = StreamOperation.FORWARD)
    fun onCharacterList(ctx: ProxyClientContext, msg: CharacterListMessage) {
        /* prevents auto-login if no default character **/
        val name = ctx.botMightBeNull()?.account?.defaultCharacter?.name ?: return

        /* prevents mistakes */
        val character = msg.characters.find { it.name == name }

        if (character != null)
            ctx.upstream().write(SelectCharacterMessage(character.guid))
    }

    @FromUpstream(then = StreamOperation.FORWARD)
    fun onCharacterSelected(ctx: ProxyClientContext, msg: MountGivenXpMessage) {
        ctx.state = ProxyClientState.IN_GAME
    }
}