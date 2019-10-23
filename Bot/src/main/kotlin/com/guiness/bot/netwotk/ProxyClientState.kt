package com.guiness.bot.netwotk

enum class ProxyClientState {
    AWAIT_HELLO,
    AWAIT_VERSION,
    AWAIT_ACCOUNT,
    DISCONNECTED,
    SERVER_LIST,
    CHARACTER_LIST,
    IN_GAME
}