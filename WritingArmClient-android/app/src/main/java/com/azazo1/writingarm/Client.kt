package com.azazo1.writingarm

import android.util.Log
import androidx.annotation.FloatRange
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import com.google.gson.Gson
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.Response
import okhttp3.WebSocket
import okhttp3.WebSocketListener
import okio.ByteString
import java.net.InetSocketAddress
import java.time.Duration
import kotlin.math.pow
import kotlin.math.round

data class Action(val x: Float, val y: Float, val pressure: Float)

enum class CommandType(val s: String) {
    MOVE("m"), DROP("d"), LIFT("l"), MODE_AP("p"), MODE_STA("s"), ACTION_SEQUENCE("a")
}

fun Float.roundTo(digit: Int): Float {
    val power = 10f.pow(digit)
    return round(this * power) / power
}

class Client(
    address: InetSocketAddress
) {
    private val client = OkHttpClient.Builder().connectTimeout(Duration.ofSeconds(2)).build()
    private val req =
        Request.Builder().url("ws://${address.address.hostAddress}:${address.port}").build()
    var alive by mutableStateOf(true)
        internal set
    var connected by mutableStateOf(false)
        internal set
    private val listener = object : WebSocketListener() {
        override fun onClosed(webSocket: WebSocket, code: Int, reason: String) {
            Log.i("websocket-client", "closed")
            alive = false;
            connected = false;
        }

        override fun onClosing(webSocket: WebSocket, code: Int, reason: String) {
            Log.i("websocket-client", "closing")
        }

        override fun onFailure(webSocket: WebSocket, t: Throwable, response: Response?) {
            Log.e("websocket-client", "failure: ${t.cause}/${t.message}-$response")
            alive = false;
            connected = false;
        }

        override fun onMessage(webSocket: WebSocket, text: String) {
            Log.i("websocket-client", "Message: $text")
        }

        override fun onMessage(webSocket: WebSocket, bytes: ByteString) {
            Log.i("websocket-client", "Message: $bytes")
        }

        override fun onOpen(webSocket: WebSocket, response: Response) {
            Log.i("websocket-client", "opened")
            connected = true;
        }
    }
    private val websocket = client.newWebSocket(req, listener)

    private fun sendCommand(type: CommandType, args: Map<String, Any>) {
        websocket.send(gson.toJson(mapOf("type" to type.s, "args" to args)))
    }

    fun movePen(
        @FloatRange(from = 0.0, to = 148.5) x: Float,
        @FloatRange(from = 0.0, to = 210.0) y: Float
    ) {
        sendCommand(CommandType.MOVE, mapOf("x" to x, "y" to y))
    }

    fun liftPen() {
        sendCommand(CommandType.LIFT, mapOf())
    }

    fun dropPen(@FloatRange(from = 0.0, to = 1.0) strength: Float) {
        sendCommand(CommandType.DROP, mapOf("strength" to strength))
    }

    fun modeAP(ssid: String, pwd: String) {
        sendCommand(CommandType.MODE_AP, mapOf("ssid" to ssid, "pwd" to pwd))
    }

    fun modeSTA(ssid: String, pwd: String) {
        sendCommand(CommandType.MODE_STA, mapOf("ssid" to ssid, "pwd" to pwd))
    }

    fun sendActionSequence(actions: List<Action>) {
        sendCommand(
            CommandType.ACTION_SEQUENCE,
            mapOf("actions" to actions.map {
                listOf(
                    it.x.roundTo(1),
                    it.y.roundTo(1),
                    it.pressure.roundTo(1)
                )
            })
        )
    }

    companion object {
        private val gson = Gson()
    }
}