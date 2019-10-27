package com.guiness.bot.entities

import com.guiness.bot.core.ProcessID
import com.guiness.bot.core.ScriptLoader
import com.guiness.bot.core.ScriptName
import com.guiness.bot.netwotk.ProxyClientContext
import com.guinness.api.AIScript
import com.guinness.api.IBot
import com.guinness.api.ISession
import java.lang.RuntimeException
import kotlin.reflect.full.primaryConstructor

class Bot(
    val processId: ProcessID,
    val account: Account
): IBot {
    private val scriptInstances = ArrayList<AIScript>()
    private var context: ProxyClientContext? = null
    override var session: ISession = Session()
    var ticket: String? = null

    init {
        reloadScripts()
    }

    fun contextMightBeNull() = context
    fun context() = context!!

    @Synchronized fun reloadScripts() {
        this.scriptInstances.clear()

        val scriptInstances = account.scripts.map {
            val script = ScriptLoader.loadedScripts[it] ?:  throw RuntimeException("Script $it does not exist")

            script.primaryConstructor?.call(this)
                ?: throw RuntimeException("Script ${script.qualifiedName} has invalid constructor")
        }

        this.scriptInstances.addAll(scriptInstances)
    }

    @Synchronized fun spreadEvents(event: AIScript.() -> Unit) {
        for (ai in scriptInstances)
            event(ai)
    }

    fun attach(context: ProxyClientContext) {
        this.context = context
    }
}