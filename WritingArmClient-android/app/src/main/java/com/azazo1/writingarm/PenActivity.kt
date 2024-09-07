package com.azazo1.writingarm

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.gestures.awaitEachGesture
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.wrapContentSize
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.PointerType.Companion.Stylus
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import androidx.lifecycle.ViewModelProvider
import com.azazo1.writingarm.ui.theme.WritingArmClientandroidTheme
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlin.coroutines.CoroutineContext

class Throttler(
    private val interval: Long,
    private val coroutineContext: CoroutineContext = Dispatchers.Main
) {
    private var isThrottled = false;
    fun throttle(action: () -> Unit) {
        if (isThrottled) return
        action()
        isThrottled = true
        CoroutineScope(coroutineContext).launch {
            delay(interval)
            isThrottled = false
        }
    }
}

class PenActivity : ComponentActivity() {
    private lateinit var recorder: ActionSequenceRecordViewModel
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        recorder = ViewModelProvider(this)[ActionSequenceRecordViewModel::class]
        setContent {

            WritingArmClientandroidTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    PenApp(modifier = Modifier.padding(innerPadding), recorder)
                    AutoNetworkAlert {
                        finish()
                    }
                }
            }
        }
    }
}

@Composable
fun RecordButtons(modifier: Modifier = Modifier, recorder: ActionSequenceRecordViewModel) {
    var showAlert by remember {
        mutableStateOf(false)
    }
    val rcShape = remember {
        RoundedCornerShape(3.dp)
    }
    Row(
        modifier = modifier
            .fillMaxWidth()
            .padding(30.dp),
        horizontalArrangement = Arrangement.Center,
        verticalAlignment = Alignment.CenterVertically
    ) {
        // "记录和完成记录"按钮.
        Button(onClick = {
            if (!recorder.recording) {
                recorder.startRecording()
            } else {
                recorder.stopRecording()
                showAlert = true
            }
        }) {
            if (!recorder.recording) {
                Text(stringResource(R.string.record))
            } else {
                Text(stringResource(R.string.stop))
            }
        }
        // "取消"按钮.
        if (recorder.recording) {
            Button(onClick = {
                recorder.cancelRecording()
            }) {
                Text(stringResource(R.string.cancel))
            }
        }
    }
    if (showAlert) {
        AlertDialog(
            onDismissRequest = {
                showAlert = false
                recorder.cancelRecording()
            },
            shape = rcShape,
            title = {
                Text(text = stringResource(R.string.send))
            },
            text = {
                Text(text = stringResource(R.string.send_to_scm))
            },
            confirmButton = {
                Button(onClick = {
                    showAlert = false
                    recorder.commit()
                }) {
                    Text(text = stringResource(R.string.yes))
                }
            },
            dismissButton = {
                Button(onClick = {
                    showAlert = false
                    recorder.cancelRecording()
                }) {
                    Text(text = stringResource(R.string.no))
                }
            })
    }
}

@Composable
fun PenApp(modifier: Modifier = Modifier, recorder: ActionSequenceRecordViewModel) {
    val throttler = remember {
        Throttler(100)
    }
    var redCursor by remember {
        mutableStateOf(false)
    }
    Column(
        modifier = modifier
            .fillMaxSize()
            .wrapContentSize(Alignment.Center),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally,
    ) {
        PenDrawingCanvas(
            modifier = Modifier
                .weight(1f)
                .fillMaxWidth()
                .padding(10.dp),
            onPenHover = {
                ConnectionActivity.client?.apply {
                    val y = it.y * 210f
                    val x = it.x * 148.5f
                    throttler.throttle {
                        if (!recorder.recording) {
                            liftPen()
                            movePen(x, y)
                        } else {
                            recorder.recordHover(x, y)
                            redCursor = true
                            CoroutineScope(Dispatchers.Main).launch {
                                delay(20)
                                redCursor = false
                            }
                        }
                    }
                }
            },
            onPenDraw = { pointer, pressure ->
                ConnectionActivity.client?.apply {
                    val x = pointer.x * 148.5f
                    val y = pointer.y * 210f
                    throttler.throttle {
                        if (!recorder.recording) {
                            dropPen(pressure)
                            movePen(x, y)
                        } else {
                            recorder.recordDraw(x, y, pressure)
                            redCursor = true
                            CoroutineScope(Dispatchers.Main).launch {
                                delay(20)
                                redCursor = false
                            }
                        }
                    }
                }
            },
            cursorColor = if (redCursor) {
                Color.Red
            } else {
                null
            }
        )
        RecordButtons(recorder = recorder)
    }
}

/**
 * @param onPenHover 当笔在屏幕上悬浮移动时, 反复触发此回调函数.
 * @param onPenDraw 当笔触摸屏幕, 并在屏幕上移动时, 反复触发此回调函数.
 */
@Composable
fun PenDrawingCanvas(
    modifier: Modifier = Modifier,
    onPenHover: (Offset) -> Unit = {},
    onPenDraw: (xy: Offset, pressure: Float) -> Unit = { _, _ -> },
    cursorColor: Color? = null
) {
    var pointer by remember {
        mutableStateOf(Offset(0f, 0f))
    }
    var radius by remember {
        mutableFloatStateOf(1f)
    }
    val colorScheme = MaterialTheme.colorScheme
    Canvas(modifier = modifier.pointerInput(Unit) {
        awaitEachGesture {
            while (true) {
                val e = awaitPointerEvent()
                e.changes.forEach {
                    if (it.type == Stylus) {
                        pointer = it.position
                        val ratioPointer = Offset(pointer.x / size.width, pointer.y / size.height)
                        radius = 3 + 100 * it.pressure
                        if (it.pressure < 0.1) {
                            onPenHover(ratioPointer)
                        } else {
                            onPenDraw(
                                ratioPointer, when {
                                    it.pressure < 0 -> 0f
                                    it.pressure > 1 -> 1f
                                    else -> it.pressure
                                }
                            )
                        }
                    }
                }
            }
        }
    }) {
        drawCircle(cursorColor ?: colorScheme.primary, radius, pointer)
        drawRect(colorScheme.primaryContainer, style = Stroke(3f))
    }
}