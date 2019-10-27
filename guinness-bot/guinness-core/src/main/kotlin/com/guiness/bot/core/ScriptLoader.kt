package com.guiness.bot.core

import com.guinness.api.AIScript
import org.jetbrains.kotlin.cli.common.environment.setIdeaIoUseFallback
import java.io.File
import java.io.Reader
import java.lang.RuntimeException
import java.nio.file.Files
import javax.script.ScriptEngine
import javax.script.ScriptEngineManager
import kotlin.reflect.KClass

object ScriptLoader {
    private val engine: ScriptEngine
    val loadedScripts: Map<ScriptName, KClass<AIScript>>

    init {
        setIdeaIoUseFallback()
        val classLoader = Thread.currentThread().contextClassLoader
        engine = ScriptEngineManager(classLoader).getEngineByExtension("kts")
        loadedScripts = loadScripts().associateBy ({it.simpleName!! }, {it})
    }

    private fun loadScripts(): List<KClass<AIScript>> {
        return File(Config.scriptFolder).walkTopDown()
            .filter { it.isFile }
            .map {
                val input = Files.newBufferedReader(it.toPath())

                try {
                    load<KClass<AIScript>>(input)
                } catch (e: Exception) {
                    throw RuntimeException("The following script is invalid and won't be loaded: ${it.path}")
                }
            }
            .toList()
    }

    private inline fun <R> safeEval(evaluation: () -> R?) = try {
        evaluation()
    } catch (e: Exception) {
        throw RuntimeException("Cannot load script", e)
    }

    private inline fun <reified T> Any?.castOrError() = takeIf { it is T }?.let { it as T }
        ?: throw IllegalArgumentException("Cannot cast $this to expected type ${T::class}")

    private inline fun <reified T> load(reader: Reader): T = safeEval {
        engine.eval(reader)
    }.castOrError()
}
