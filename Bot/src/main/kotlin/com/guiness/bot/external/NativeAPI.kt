package com.guiness.bot.external

import java.io.File

class NativeAPI {
    /**
     * Injects a given process id with a given dll
     */
    external fun inject(processId: Int, dllPath: String)

    /**
     * Injects the first dofus process found
     */
    external fun injectDofus(dllPath: String)

    fun init() {
        injectDofus(patcherLibraryName)
    }

    companion object {
        const val nativeDirectory = "native"
        val apiLibraryName = "guinness-native-api".toAbsolutePath()
        val patcherLibraryName = "guinness-native-patcher".toAbsolutePath()

        init {
            System.load(apiLibraryName)
        }
    }
}

fun String.toAbsolutePath() = System.getProperty("user.dir") + File.separator + NativeAPI.nativeDirectory +
        File.separator + this + "-x" + System.getProperty("sun.arch.data.model") + ".dll"