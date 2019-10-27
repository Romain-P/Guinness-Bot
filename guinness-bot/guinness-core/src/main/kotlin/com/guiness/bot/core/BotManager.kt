package com.guiness.bot.core

import com.guiness.bot.entities.Account
import com.guiness.bot.entities.Bot
import com.guiness.bot.netwotk.ProxyClientContext
import com.guinness.api.AIScript
import java.lang.RuntimeException
import kotlin.reflect.KClass
import kotlin.reflect.full.primaryConstructor

object BotManager {
    val instances: MutableMap<AccountName, Bot> = HashMap()

    /**
     * Finds a dofus instance and auto-logs for each account given in the list
     * If there is no enough dofus instances opened, a list of accounts that did not
     * find an instance are returned.
     * You could notify the user that he must launch more dofus instances
     *
     * @param accounts  a list of accounts you wish to connect on a dofus instance
     * @return          a list of accounts that didn't find a dofus instance
     */
    fun connect(accounts: List<Account>): List<Account> {
        val remainingAccounts = ArrayList(accounts)
        val processes = NativeAPI.allDofusProccesses()
            .filter { proc -> instances.values.find { it.processId == proc } == null }

        if (processes.isEmpty()) return accounts

        val limit = if (accounts.size > processes.size) processes.size else accounts.size

        processes.take(limit).forEachIndexed { index, pid ->
            val account = accounts[index]

            val bot = Bot(pid, account)
            instances[account.username] = bot
            initBotScripts(bot)

            NativeAPI.injectDofus(pid)
            NativeAPI.login(pid, account.username, account.password)
            remainingAccounts.remove(account)

            if (!Config.isDofusPathInitialized())
                Config.dofusPath = NativeAPI.dofusPath(pid)
        }

        return remainingAccounts
    }

    private fun initBotScripts(bot: Bot) {
        val scripts = ScriptLoader.loadedScripts.map {
            it.primaryConstructor?.call(bot) ?: throw RuntimeException("Script ${it.qualifiedName} has invalid constructor")
        }

        bot.updateScripts(scripts)
    }

    fun linkBotAndContext(context: ProxyClientContext, username: String? = null, ticket: String? = null) {
        val bot = when(username) {
            null -> instances.values.find { it.ticket?.equals(ticket) ?: false }
            else -> instances[username]
        } ?: return

        bot.attach(context)
        context.attach(bot)
    }
}