# Guinness Bot
## Introduction
A dofus retro MITM event-driven bot written in `C++` and `Kotlin JVM`.  
Client modifications are not needed, the redirection is done by an injected module.  

The choice to make a `MITM` is justified by several reasons: 
 * Take advantages of a clean client (without modifications)
 * Easy to understand the network protocol
 * Bypass anti-bot features 
    - Like the integrity of the client verified by the launcher
    - As well as packets hidden in the client.
  * The fact that dofus is launched with electron allows ankama to make futher integrity checks than before
  
At this moment, the bot works fully on `Windows XP` up to `Windows 10`. 

## Global Overview 
|                |Stack                        |Used for                         |
|----------------|-------------------------------|-----------------------------|
|guinness-dll			 |C++ - Minhook            		| Injected in dofus.exe: Hook winsock connect-recv        |
|guinness-native          |C++ - Hadesmem - JNI           |Native extension of Kotlin for injection & low-level stuffs         |
|guinness-bot          |Kotlin JVM - Netty - TornadoFX	| Bot core, proxy

## Process

Well this diagram could have been reviewed, but it should explains basics
![diagram](https://i.imgur.com/7TtCisP.png)

# Features
## Network logging

Available in console  
```
Sun, 20 Oct 2019 15:43:32 GMT -- Proxy/tcp [.]  [DOWNSTREAM] [SENT]      ---> [PROXY]        : Af
Sun, 20 Oct 2019 15:43:32 GMT -- Proxy/tcp [.]  [PROXY]      [FORWARDED] ---> [UPSTREAM]     : Af
Sun, 20 Oct 2019 15:43:32 GMT -- Proxy/tcp [.]  [UPSTREAM]   [SENT]      ---> [PROXY]        : Af1|2|0||-1
Sun, 20 Oct 2019 15:43:32 GMT -- Proxy/tcp [.]  [PROXY]      [FORWARDED] ---> [DOWNSTREAM]   : Af1|2|0||-1
```

## Network serializer/deserializer

An easy way to parse and write packets. When the bot starts, all classes found on a given package are scanned and parsed automatically.  See [protocol/messages](https://github.com/Romain-P/Guinness-Bot/tree/master/guinness-bot/guinness-core/src/main/kotlin/com/guiness/bot/protocol/messages)  
  
Let's take as example, the character list packet

```kotlin
enum class VisibleStuffType(val index: Int) {
    WEAPON(0),
    HEAD(1),
    BACK(2),
    PET(3),
    SHIELD(4)
}

@Delimiter(";")
data class Character(
    var guid: Int,
    var name: String,
    var level: Int,
    var gfxId: Int,
    @Size(3)
    var colors: Array<@Hex Int?>,
    @Delimiter(",") @Size(5)
    var visualStuff: Array<@Hex Int?>,
    var sellMode: Int,
    var serverId: Int,
    var deathCount: Int?,
    var isLevelMax: Int?,
    var unknown: Int?
) {
    fun visibleStuffId(type: VisibleStuffType): Int? = visualStuff[type.index]

    fun setVisualStuffId(type: VisibleStuffType, newId: Int) {
        visualStuff[type.index] = newId
    }
}

@Message(header = "ALK", source = StreamSource.UPSTREAM, delimiter = "|")
data class CharacterListMessage(
    var subscriptionTime: Int,
    var charactersCount: Int,
    var characters: Array<Character>
)
```

Annotate a class with `@Message(header = "ALK", source = StreamSource.UPSTREAM, delimiter = "|")` allows the package scanner to parse the given class.  
 * `header` corresponds to the packet identifier
 * `source` corresponds to the stream source (dofus.exe (`downstream`) or ankama (`upstream`)
 * `delimiter` corresponds to the delimiter of each data in the packet
   
Nested objects can be added. Annotate a nested object with `@Delimiter` allows to define a different delimiter for this object. You can find some other annotations in this example, which are quite intuitive.  

## Network controllers

An easy way to intercept, forward, discard, or rewrite packets.  When the bot starts, controllers's handlers are loaded and mapped to the wanted packet automatically.  
See [controllers/](https://github.com/Romain-P/Guinness-Bot/tree/master/guinness-bot/guinness-core/src/main/kotlin/com/guiness/bot/controllers/)  
  
Let's take the `CharacterListMessage` as example.  The following code intercept the `CharacterListMessage` and reponds automatically to the server with the chosen character (auto-login).

```kotlin
@Controller // annotate a class with @Controller so handlers will be retrieved and parsed automatically
class LoginController {

    @FromUpstream(then = StreamOperation.FORWARD)
    fun onCharacterList(ctx: ProxyClientContext, msg: CharacterListMessage) {
        val botCharacterName = ctx.botMightBeNull()?.account?.defaultCharacter?.name ?: return

        val character = msg.characters.find { it.name == botCharacterName }

        if (character != null)
            ctx.upstream().write(SelectCharacterMessage(character.guid)) //auto select character
    }
}
```

In this example, we intercept the `CharacterListMessage` using a handler. All handlers have the same signature.  
`fun {handlerName}(ctx: ProxyClientContext, msg: {MessageClassToIntercept}) {}`  

Each handler must be annotated with one of the following annotation:
 * `@FromUpstream`: intercept a message coming from ankama servers
 * `@FromDownstream`: intercept a message coming from dofus.exe
 * `@FromAnyStream`: intercept a message that can come from any side
   
Each annotation must define a `then` operation, which corresponds to the action done once the handler has been executed.  
 * `then = StreamOperation.FORWARD`: the packet will be transmitted to the other stream
 * `then = StreamOperation.DISCARD`: the packet won't be trasmitted
 * `then = StreamOperation.MIRROR`: the packet will be resent to the source (like an echo)
 
Console output for this example:
```
Wed, 30 Oct 2019 23:22:09 GMT -- Proxy/tcp [.]  [UPSTREAM]   [SENT]      ---> [PROXY]        : CharacterListMessage(subscriptionTime=2213488584, charactersCount=1, characters=[Character(guid=80000000, name=JohnDoe, level=51, gfxId=90, colors=[16580608, 0, 15663104], visualStuff=[2416, 6926, 6927, null, null], sellMode=0, serverId=608, deathCount=null, isLevelMax=null, unknown=null)])
Wed, 30 Oct 2019 23:22:09 GMT -- Proxy/tcp [.]  [PROXY]      [SENT]      ---> [DOWNSTREAM]   : CharacterListMessage(subscriptionTime=2213488584, charactersCount=1, characters=[Character(guid=80000000, name=JohnDoe, level=51, gfxId=90, colors=[16580608, 0, 15663104], visualStuff=[2416, 6926, 6927, null, null], sellMode=0, serverId=608, deathCount=null, isLevelMax=null, unknown=null)])
Wed, 30 Oct 2019 23:22:09 GMT -- Proxy/tcp [.]  [PROXY]      [SENT]      ---> [UPSTREAM]     : SelectCharacterMessage(characterGuid=80025524)
```
`Note:` any packet without handler is forwarded by default  
 
 ### Edit packet data --on the fly
 
 The packets that are intercepted from a handler can be edited. In order:  
  * The incoming packet is deserialized (converted to the associated data class if found)
  * The handler (if found) is executed passing the instance of this deserialized packet
  * Once the handler is executed, the deserialized packet is serialized (converted back to a buffer of bytes)

Now you can have fun with the packets

```kotlin
    @FromUpstream(then = StreamOperation.FORWARD)
    fun onCharacterList(ctx: ProxyClientContext, msg: CharacterListMessage) {
        /* apply changes on all characters */
        msg.characters.forEach {
            it.name = "Ankama" /* changes the name to ankama */
            it.level = 150 /* changes the level to 150 */
            it.setVisualStuffId(VisibleStuffType.HEAD, 7143) /* set a solomonk skin */
        }
    }
```

![example](https://i.imgur.com/dHHRl7F.png)
 
 ## Streams Overview
 
 A [Bot](https://github.com/Romain-P/Guinness-Bot/blob/master/guinness-bot/guinness-core/src/main/kotlin/com/guiness/bot/entities/Bot.kt) holds 1 proxy context aka [ProxyClientContext](https://github.com/Romain-P/Guinness-Bot/blob/master/guinness-bot/guinness-core/src/main/kotlin/com/guiness/bot/netwotk/ProxyClientContext.kt) which corresponds to the class holding and managing stream flux.  
In order to send packets to dofus.exe or ankama (`downstream` or `upstream`), this context must be etablished (by a connection).  
Once the connection is done, you can write to any stream at any time from any scope.  
  
As we have seen previously, we can send packets using `Stream#write`.

```kotlin
    @FromUpstream(then = StreamOperation.FORWARD)
    fun onCharacterList(ctx: ProxyClientContext, msg: CharacterListMessage) {
        /* prevents auto-login if no default character **/
        val name = ctx.botMightBeNull()?.account?.defaultCharacter?.name ?: return

        /* prevents mistakes */
        val character = msg.characters.find { it.name == name }

        if (character != null)
            ctx.upstream().write(SelectCharacterMessage(character.guid)) /** sends a message to ankama **/
    }
```

### Write wrapped vs unwrapped

The stream API allows to send a structured packet or simply a suit of bytes (UTF8 chars in the case of the dofus protocol)
```
ctx.upstream().write("BD10|10", unwrapped = true) /* sends packet to upstream */
ctx.downstream().write(AnyWrappedPacketClass(param1, param2)) /* sends packet to downstream */
```
I would like to remind you that using wrapped packet is the preferred way, since the logging system will convert the packet in a well and human-readable representation.  
Also, packet delimiters are automatically handled, so don't worry about it.  
  
### Delayed writes

It is also possible to delay some packets, still without any effort.  
The stream api is non-blocking. So calling any function won't block the current thread.

```
ctx.upstream().post("AxK0000", 5.seconds, unwrapped = true) /* will be sent after 5 seconds **/

/* transaction used for send packets following the same timeline */
ctx.upstream().delayedTransaction { 
    later("BN", 500.milliseconds, unwrapped = true) /* sent after 500 ms */
    later("BN2", 2.seconds, unwrapped = true)       /* sent after 2500 ms */
    later("BN3", unwrapped = true)                  /* sent after 2500 ms */
    later(SelectCharacterMessage(0), 1.seconds)     /* sent after 3500 ms */ 
}
```

## Script engine

I tried to get involved in the general design of the bot. I wanted to make possible the implementation of new game features without polluting the code of the core, and also make possible to edit some behaviour without recompiling the bot. --on the fly  
So it is possible to write `kotlin-script` using the [bot-api](https://github.com/Romain-P/Guinness-Bot/tree/master/guinness-bot/guinness-api/src/main/kotlin/com/guinness/api)

Scripts are loaded when the application starts and must be located where the executable of the bot is, in the folder `scripts`.  
Here is the folder: [click-me](https://github.com/Romain-P/Guinness-Bot/tree/master/guinness-bot/scripts)

A script looks like the following code:
```kotlin
import com.guinness.api.AIScript
import com.guinness.api.IBot
import com.guinness.api.entities.IMap

class ScriptTest(val bot: IBot): AIScript(bot) {
    override fun onMapChanged(previous: IMap?, new: IMap) {
        println("message from script")
        this.bot.session./** whatever you want to access **/
    }
}

ScriptTest::class
```

I did not work a lot of the API because I don't have added enough game feature at this moment.  
The list of event can be found in the [AIScript class](https://github.com/Romain-P/Guinness-Bot/blob/master/guinness-bot/guinness-api/src/main/kotlin/com/guinness/api/AIScript.kt). The list of available events should grow up in the future.  

```kotlin
open class AIScript(private val bot: IBot) {
    open fun onWorldEnter() {}
    open fun onMapChanged(previous: IMap?, new: IMap) {}
}
```

### Add a script event

Adding a new event would simply be done adding a new function in the `AIScript` and call it from a controller.  
For example, this is how is implemeted the `onMapChanged` event:

```kotlin
@Controller
class MovementController {

    @FromUpstream(then = StreamOperation.FORWARD)
    fun onMapChanged(ctx: ProxyClientContext, msg: MapDataMessage) {
        val map = MapService.loadMap(msg.mapId, msg.date, msg.privateKey)

        val previous = ctx.bot().session.map
        ctx.bot().session.map = map
        ctx.bot().spreadEvents { onMapChanged(previous, map) } /* fires this event to all scripts subscribed by the bot */
    }
}
```
  
## DEMO

Auto-login without any manipulation.  
Account and password must be written in the source code for now. There is no GUI yet.  
You can edit auto-login information [HERE](https://github.com/Romain-P/Guinness-Bot/blob/master/guinness-bot/guinness-core/src/main/kotlin/com/guiness/bot/core/ProfileManager.kt#L10)

![demo](https://i.imgur.com/Q6vOjaB.gif)

## Incoming features - experimental

I'm currently working on the movement. Since we can take profits of the client, here's how I'm doing:
 * Redirecting dns of ankama (for intercept http requests)
 * Catch and infect a swf with a script
 * Compute path from the client on the request of the MITM
 
Current injected code:
```actionscript
   if(_global.API.network._oDataProcessor.aks.Basics.$_onDate == undefined)
   {
      _global.API.network._oDataProcessor.aks.Basics.$_onDate = _global.API.network._oDataProcessor.aks.Basics.onDate;
   }
   _global.API.network._oDataProcessor.aks.Basics.onDate = function(data)
   {
      if(data.charAt(0) != "#")
      {
         _global.API.network._oDataProcessor.aks.Basics.$_onDate(data);
      }
      else
      {
         var _loc3_ = data.substr(1);
         _global.API.datacenter.Game.setInteractionType("move");
         _global.API.gfx.onCellRelease({num:Number(_loc3_)});
         _global.API.kernel.showMessage(_loc3_,_loc4_,"ERROR_BOX");
      }
   };
```
It simply hook `onDate` handler (`BD` packet) and changes the behaviour of the handler. If the packet starts with a `#`, a custom code is executed. In this case, I simply call `onCellRelease` which is going to compute the path for the targetted cell, and sends the packet to the MITM, that will forward it.  

From the bot:
```kotlin
ctx.downstream().write("BD#${randomCell.id}", unwrapped = true)
```

By this way, there is no need to implement the path finder, we simply take advantages of the client ;)  
This code's currently working, now I have to edit the windows `host` file for redirect the ankama dns to the mitm local ip, and infect a random swf.  
