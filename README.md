# Guinness Bot
## Introduction
A dofus retro MITM event-driven bot written in `C++` and `Kotlin JVM`.  
The choice to make a `MITM` is justified by several reasons: 
 * Take advantages of a clean client (without modifications)
 * Easy to understand the network protocol
 * Bypass anti-bot features 
    - Like the integrity of the client verified by the launcher
    - As well as packets hidden in the client.
  * The fact that dofus is launched with electron allows ankama to make futher integrity checks than before

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

Available in console  
## Network logging
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

It is also possible to delay some packets, still without any effort

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
