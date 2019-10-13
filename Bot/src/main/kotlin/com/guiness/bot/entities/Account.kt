package com.guiness.bot.entities

import com.guiness.bot.core.AccountName

data class Account(
    val username: AccountName,
    val password: String
)