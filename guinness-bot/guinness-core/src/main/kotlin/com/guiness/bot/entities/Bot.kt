package com.guiness.bot.entities

import com.guiness.bot.core.ProcessID
import com.guiness.bot.netwotk.ProxyClientContext
import com.guinness.api.AIScript
import com.guinness.api.IBot
import com.guinness.api.ISession

class Bot(
    val processId: ProcessID,
    val account: Account
): IBot {
    private var context: ProxyClientContext? = null
    override var session: ISession = Session()
    var ticket: String? = null
    private val scripts = ArrayList<AIScript>()

    fun contextMightBeNull() = context
    fun context() = context!!

    fun updateScripts(scripts: List<AIScript>) {
        this.scripts.clear()
        this.scripts.addAll(scripts)
    }

    fun spreadEvent(consumer: (AIScript) -> Unit) {
        for (ai in scripts)
            consumer(ai)
    }

    fun attach(context: ProxyClientContext) {
        this.context = context
    }
}