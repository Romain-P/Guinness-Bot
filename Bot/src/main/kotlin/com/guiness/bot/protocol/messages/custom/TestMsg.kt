package com.guiness.bot.protocol.messages.custom

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.Parse

@Parse(delimiter = ":")
data class Item(
    val id: Int,
    val quantity: Int
)

@Parse(",")
data class GuildMember(
    val name: String,
    @Parse(".")
    val items: Array<Item>
)

@Message(header = ".{AH}.", delimiter = """|""")
data class TestMsg(
    val ip: String,
    val port: Int,
    @Parse(";")
    val guildMembers: Array<GuildMember>
)