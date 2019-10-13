package com.guiness.bot.entities

import com.guiness.bot.core.ProcessID

class Bot(
    val processId: ProcessID,
    val account: Account,
    val dofusSocket: String,
    val ankamaSocket: String
) {

}