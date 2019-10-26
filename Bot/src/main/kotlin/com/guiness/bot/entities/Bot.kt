package com.guiness.bot.entities

import com.guiness.bot.core.ProcessID
import com.guiness.bot.netwotk.ProxyClientContext

class Bot(
    val processId: ProcessID,
    val account: Account
) {
    private var context: ProxyClientContext? = null

    var session = Session()
    var ticket: String? = null

    fun contextMightBeNull() = context
    fun context() = context!!

    fun attach(context: ProxyClientContext) {
        this.context = context
    }
}