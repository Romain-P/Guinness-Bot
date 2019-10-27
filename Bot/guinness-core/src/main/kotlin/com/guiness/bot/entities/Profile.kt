package com.guiness.bot.entities

import com.guiness.bot.core.AccountName

data class Profile(
    var alias: String,
    var accounts: MutableMap<AccountName, Account> = HashMap()
)