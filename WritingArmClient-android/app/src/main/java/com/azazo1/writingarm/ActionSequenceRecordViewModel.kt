package com.azazo1.writingarm

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel


class ActionSequenceRecordViewModel : ViewModel() {
    var recording by mutableStateOf(false)
        internal set
    private val actionSequence: MutableList<Action> = mutableStateListOf()

    fun startRecording() {
        recording = true
    }

    fun stopRecording() {
        recording = false
    }

    fun cancelRecording() {
        actionSequence.clear()
        recording = false
    }

    fun commit() {
        if (!recording) {
            ConnectionActivity.client?.sendActionSequence(actionSequence)
            actionSequence.clear()
        } else {
            throw IllegalStateException("Commit should be done after recording.")
        }
    }

    fun recordHover(x: Float, y: Float) {
        if (recording) {
            actionSequence.add(Action(x, y, 0f))
        }
    }

    fun recordDraw(x: Float, y: Float, pressure: Float) {
        if (recording) {
            actionSequence.add(Action(x, y, pressure))
        }
    }
}