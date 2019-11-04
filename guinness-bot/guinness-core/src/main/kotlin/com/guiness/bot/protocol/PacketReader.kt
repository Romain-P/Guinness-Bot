package com.guiness.bot.protocol

class PacketReader {
    private val sharedIndex: Array<Int>
    private val packet: String
    private val stack: List<Char>
    private var consumed: Boolean = false
    private var hasNext: Boolean = true

    constructor(packet: String, delim: Char) {
        this.sharedIndex = arrayOf(0)
        this.stack = listOf(delim)
        this.packet = packet

        /** Old quick fix, should be un-necessary now
         *
         *   if (packet.isNotEmpty() && packet[0] == delim)
         *       ++sharedIndex[0]
        **/
    }

    constructor(packet: String, delim: String): this(packet, delim[0])

    private constructor(sharedIndex: Array<Int>, packet: String, stack: List<Char>) {
        this.sharedIndex = sharedIndex
        this.stack = stack
        this.packet = packet
    }

    fun child(delim: Char, shift: Int = 0): PacketReader {
        val newStack = stack.toMutableList()
        newStack.add(delim)
        if (shift != 0)
            sharedIndex[0] += shift
        return PacketReader(sharedIndex, packet, newStack)
    }

    fun child(delim: String, shift: Int = 0) = child(delim[0], shift)

    fun readNext(readExactly: Int = 0): String? {
        if (consumed || packet.isEmpty() || sharedIndex[0] == packet.length)
            return null

        var action = ReadAction.CONTINUE
        val startIndex = sharedIndex[0]

        if (readExactly == 0) {
            while (action == ReadAction.CONTINUE) {
                action = delimiterReached()
                ++sharedIndex[0]
            }
        } else
            sharedIndex[0] += readExactly

        val endIndex = when(readExactly) {
            0       -> sharedIndex[0] - 1
            else    -> readExactly
        }

        return when (startIndex) {
            sharedIndex[0] - 1  -> null
            else                -> packet.substring(startIndex, endIndex)
        }
    }

    fun hasNext(): Boolean {
        return this.hasNext && packet.isNotEmpty() && sharedIndex[0] < packet.length;
    }

    private fun delimiterReached(): ReadAction {
        val char = when (sharedIndex[0]) {
            packet.length -> '\u0000'
            else          -> packet[sharedIndex[0]]
        }

        if (char == '\u0000') {
            consumed = true
            hasNext = false
            return ReadAction.STOP_NO_NEXT
        }

        for (i in stack.lastIndex downTo 0) {
            if (stack[i] != char) continue

            return when (i) {
                stack.lastIndex -> ReadAction.STOP_HAS_NEXT
                else            -> {
                    consumed = true
                    hasNext = false
                    ReadAction.STOP_NO_NEXT
                }
            }
        }

        return ReadAction.CONTINUE
    }

    private enum class ReadAction {
        CONTINUE,
        STOP_HAS_NEXT,
        STOP_NO_NEXT
    }
}