package com.guiness.bot.protocol.messages.game.basics

import com.guiness.bot.protocol.annotations.Delimiter

@Delimiter("!")
data class ChatMessageArgs(
    var itemId : Int,
    var itemStats : String
)