package com.guiness.bot.protocol.messages.custom

import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.annotations.Delimiter
import com.guiness.bot.protocol.annotations.Size
import com.guiness.bot.protocol.annotations.StreamSource

data class Item(
    val id: Int?,
    val quantity: Int?
)

data class Perco(
    val name: String,
    @Delimiter(".") @Size(2)
    val items: Array<Item?>,
    val level: Int
)

@Delimiter(",")
data class GuildMember(
    val name: String,
    @Delimiter("|")
    val item: Item,
    val level: Int,
    @Size(2)
    val percos: Array<Perco>,
    val itemBis: Item
)

@Message(header = "TEST", source = StreamSource.ANYSTREAM, delimiter = """|""")
data class TestMsg(
    val ip: String,
    val port: Int,
    val item: Item,
    val guildMember: GuildMember,
    val item2: Item,
    val item3: Item,
    val randomValue: Int,
    val endArray: Array<Item>
)

@Message(header = "AAA", source = StreamSource.ANYSTREAM, delimiter = " ")
data class TestP(
    val char: Char
)