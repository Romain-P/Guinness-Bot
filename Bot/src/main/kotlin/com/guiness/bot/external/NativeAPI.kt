package com.guiness.bot.external

import com.guiness.bot.core.ProcessID
import java.io.File

class NativeAPI {

    /**
     * Returns a list of processes named by the given argument
     */
    private external fun availableProcesses(processName: String): IntArray

    /**
     * Injects a given process id with a given dll
     */
    private external fun inject(id: ProcessID, dllPath: String)

    /**
     * Auto login on an available process already injected
     */
    external fun login(id: ProcessID, username: String, password: String)

    /**
     * Auto relogin if the player is disconnected
     */
    external fun reLogin(id: ProcessID, username: String, password: String)

    fun injectDofus(id: ProcessID) = inject(id, patcherLibraryName)
    fun allDofusProccesses() = availableProcesses(dofusExecutableName)

    companion object {
        const val dofusExecutableName = "Dofus.exe"
        const val nativeDirectory = "native"
        val apiLibraryName = nativeLibraryPath("guinness-native-api")
        val patcherLibraryName = nativeLibraryPath("guinness-native-patcher")

        init {
            System.load(apiLibraryName)
        }

        fun nativeLibraryPath(dllName: String) = System.getProperty("user.dir") + File.separator +
                nativeDirectory + File.separator + dllName + "-x" + System.getProperty("sun.arch.data.model") + ".dll"
    }
}