package com.guiness.bot.core

import com.guiness.bot.protocol.DofusProtocol
import com.guiness.bot.protocol.MetaMessage
import com.guiness.bot.protocol.annotations.Message
import com.guiness.bot.protocol.messages.custom.GuildMember
import com.guiness.bot.protocol.messages.custom.Item
import com.guiness.bot.protocol.messages.custom.TestMsg
import kotlin.reflect.full.findAnnotation


class Main

fun main(args: Array<String>) {
    //NativeAPI.patchProxyPort(5555)

    /**Proxy.withHost("127.0.0.1", 5555)
        .withWorker(1, 4)
        .start()**/
    val guildMembers: Array<GuildMember> = arrayOf(
        GuildMember("Romain", arrayOf(Item(1000, 2), Item(1245, 64))),
        GuildMember("Antoine", arrayOf(Item(154, 4), Item(548, 2)))
    )

    val originalMessage = TestMsg("127.0.0.1", 1, Item(1, 2), arrayOf(1,2,3), guildMembers)

    /** On transforme l'objet en packet (string) **/
    val packet = DofusProtocol.serialize(originalMessage)
    /** On transforme le packet en message (objet) **/
    val message = DofusProtocol.deserialize(packet!!)

    /** on affiche **/
    println(packet)
    println(message)
    println("Same object ? ${message.hashCode().equals(originalMessage.hashCode())}")
}