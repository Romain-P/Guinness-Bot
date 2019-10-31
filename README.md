# Guinness Bot
## Introduction
A dofus retro MITM bot written in `C++` and `Kotlin JVM`.  
The choice to make a `MITM` is justified by several reasons: 
 * Easy to understand the network protocol
 * Bypass anti-bot features 
    - Like the integrity of the client verified by the launcher
    - As well as packets hidden in the client.
  * The fact that dofus is launched with electron allows ankama to make further checks than before

## Global Overview 
|                |Stack                        |Used for                         |
|----------------|-------------------------------|-----------------------------|
|guinness-dll			 |C++ - Minhook            		| Injected in dofus.exe: Hook winsock connect|recv        |
|guinness-native          |C++ - Hadesmem - JNI           |Native extension of Kotlin for injection & low-level stuffs         |
|guinness-bot          |Kotlin JVM - Netty - TornadoFX	| Bot core, proxy

## Process

Well this diagram could have been reviewed, but it should explains basics
![diagram](https://i.imgur.com/7TtCisP.png)
