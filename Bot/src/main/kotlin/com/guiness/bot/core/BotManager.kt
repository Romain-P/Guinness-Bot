package com.guiness.bot.core

import com.guiness.bot.entities.Account
import com.guiness.bot.entities.Bot
import com.guiness.bot.external.NativeAPI

object BotManager {
    val instances: MutableMap<ProcessID, Bot> = HashMap()

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
        val processes = NativeAPI.allDofusProccesses().filter { instances[it] == null }

        if (processes.isEmpty()) return accounts

        val limit = if (accounts.size > processes.size) processes.size else accounts.size

        processes.take(limit).forEachIndexed { index, pid ->
            val account = accounts[index]

            instances[pid] = Bot(pid, account, "", "")
            NativeAPI.injectDofus(pid)
            NativeAPI.login(pid, account.username, account.password)
            remainingAccounts.remove(account)
        }

        return remainingAccounts
    }
}