import com.guinness.api.AIScript
import com.guinness.api.IBot
import com.guinness.api.entities.IMap

class ScriptTest(val bot: IBot): AIScript(bot) {
    override fun onMapChanged(previous: IMap?, new: IMap) {
        println("message from script")
    }
}

ScriptTest::class