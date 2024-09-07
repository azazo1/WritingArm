package com.azazo1.writingarm

import android.content.Intent
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.wrapContentSize
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import com.azazo1.writingarm.ui.theme.WritingArmClientandroidTheme
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetSocketAddress
import java.net.NetworkInterface
import java.net.SocketTimeoutException

val DISCOVER_HEADER: ByteArray = "ping".toByteArray()
const val DISCOVERY_PORT: Int = 13301

/**
 * 发现局域网内的接收端地址
 * (设备名称, ipv4地址(带端口))
 */
suspend fun discover(): List<InetSocketAddress> {
    val rst: MutableList<InetSocketAddress> = mutableListOf()
    withContext(Dispatchers.IO) {
        val socket = DatagramSocket()
        socket.broadcast = true
        val interfaces = NetworkInterface.getNetworkInterfaces()
        for (iFace in interfaces) {
            for (address in iFace.interfaceAddresses) {
                address.address?.let { inetAddress ->
                    // 看看是不是 ipv4.
                    if (inetAddress.address.size != 4) {
                        return@let
                    }
                    val broadcastAddress = address.broadcast ?: return@let
                    Log.i("Discover", "Address: $broadcastAddress")
                    val packet = DatagramPacket(
                        DISCOVER_HEADER,
                        0,
                        DISCOVER_HEADER.size,
                        InetSocketAddress(broadcastAddress, DISCOVERY_PORT)
                    )
                    socket.send(packet)
                }
            }
        }
        socket.soTimeout = 500
        val buffer = ByteArray(1024)
        val packet = DatagramPacket(buffer, buffer.size)
        try {
            while (true) {
                socket.receive(packet)
                val received = String(packet.data, 0, packet.length, Charsets.UTF_8)
                if (received.startsWith("pong")) {
                    val port = received.substring(4..8).toInt()
                    rst.add(InetSocketAddress(packet.address, port))
                }
            }
        } catch (_: SocketTimeoutException) {
            Log.i("Discover", "timeout")
        } catch (e: Exception) {
            Log.e("Discover-Error", "Message:", e)
        }
    }
    return rst
}

class ConnectionActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            WritingArmClientandroidTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    AddressInput(
                        modifier = Modifier
                            .padding(innerPadding)
                            .fillMaxSize(),
                        onConfirm = {
                            client = Client(it)
                            startActivity(Intent(this, ModeSelectingActivity::class.java))
                        }
                    )
                }
            }
        }
    }

    companion object {
        var client: Client? = null
            internal set
    }
}

@Composable
fun AddressInput(
    modifier: Modifier = Modifier,
    onConfirm: (InetSocketAddress) -> Unit,
) {
    var ip by remember {
        mutableStateOf("")
    }
    var port by remember {
        mutableStateOf("")
    }
    var discovering by remember {
        mutableStateOf(false)
    }
    Column(
        modifier = modifier
            .padding(10.dp)
            .wrapContentSize(Alignment.Center),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        OutlinedTextField(
            value = ip,
            onValueChange = { ip = it },
            maxLines = 1,
            keyboardOptions = KeyboardOptions.Default.copy(keyboardType = KeyboardType.Decimal), // 可以输入小数点.
            label = { Text(stringResource(R.string.ip)) }
        )
        OutlinedTextField(
            value = port,
            onValueChange = { port = it },
            maxLines = 1,
            keyboardOptions = KeyboardOptions.Default.copy(keyboardType = KeyboardType.Number),
            label = { Text(stringResource(R.string.port)) }
        )
        Row(
            modifier = Modifier
                .padding(10.dp)
                .wrapContentSize(Alignment.Center),
            horizontalArrangement = Arrangement.Center,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Button(onClick = {
                CoroutineScope(Dispatchers.Main).launch {
                    discovering = true
                    val addresses = discover()
                    if (addresses.isNotEmpty()) {
                        val address = addresses[0]
                        ip = address.hostString
                        port = address.port.toString()
                    }
                    discovering = false
                }
            }) {
                Text(stringResource(R.string.discover))
            }
            Spacer(modifier = Modifier.width(10.dp))
            Button(onClick = {
                try {
                    onConfirm(InetSocketAddress(ip, port.toInt()))
                } catch (ignore: NumberFormatException) {
                }
            }) {
                Text(stringResource(R.string.connect))
            }
        }
    }
    if (discovering) {
        AlertDialog(
            onDismissRequest = { },
            confirmButton = {
                Button(onClick = { }) {
                    Text(stringResource(R.string.wait))
                }
            },
            title = { Text(stringResource(R.string.network)) },
            text = { Text(stringResource(R.string.discovering)) })
    }
}