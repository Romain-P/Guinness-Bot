package com.guiness.bot.core

import flash.swf.Action
import flash.swf.ActionHandler
import flash.swf.TagDecoder
import flash.swf.TagHandler
import flash.swf.actions.ConstantPool
import flash.swf.actions.Push
import flash.swf.tags.DoAction

import java.io.IOException
import java.io.InputStream
import java.util.HashMap
import java.util.LinkedHashMap
import java.util.Stack

class SwfExtractor : TagHandler() {
    val variables = LinkedHashMap<String, Any>()

    override fun doAction(tag: DoAction) {
        tag.actionList.visitAll(SwfActionVariableExtractor())
    }

    private inner class SwfActionVariableExtractor : ActionHandler() {
        private val stack = Stack<Any>()
        private var pool: Array<String>? = null

        override fun push(action: Push) {
            if (pool != null && action.value is Short)
                stack.push(pool!![(action.value as Short).toInt()])
            else
                stack.push(action.value)
        }

        override fun constantPool(action: ConstantPool) {
            this.pool = action.pool
        }

        override fun getMember(action: Action) = putPath()

        override fun getVariable(action: Action) = putPath()

        private fun putPath() {
            if (stack.size > 1) {
                val first = stack.pop()
                stack.push(stack.pop().toString() + "." + first)
            }
        }

        override fun setMember(action: Action) = putVariable()

        override fun setVariable(action: Action) = putVariable()

        override fun callMethod(action: Action) {
            putPath()
            val first = stack.pop()
            val second = stack.pop()
            stack.push(first)
            stack.push(second)
            putVariable()
        }

        private fun putVariable() {
            val value = stack.pop()
            val name = stack.pop().toString()
            variables[name] = value
        }

        override fun newObject(action: Action) {
            val type = stack.pop()
            stack.pop()
            putPath()
            stack.push(type)
        }

        override fun initObject(action: Action) {
            val members = HashMap<String, Any>()
            val size = (stack.pop() as Number).toInt()
            for (i in 0 until size) {
                val data = stack.pop()
                val name = stack.pop().toString()
                members[name] = data
            }
            putPath()
            stack.push(members)
        }

        override fun initArray(action: Action) {
            val size = (stack.pop() as Number).toInt()
            val array = arrayOfNulls<Any>(size)
            for (i in 0 until size)
                array[i] = stack.pop()
            stack.push(array)
        }
    }

    companion object {
        @Throws(IOException::class)
        fun extractData(`in`: InputStream): Map<String, Any> {
            val decoder = TagDecoder(`in`)
            val extractor = SwfExtractor()
            decoder.parse(extractor)
            return extractor.variables
        }
    }
}
