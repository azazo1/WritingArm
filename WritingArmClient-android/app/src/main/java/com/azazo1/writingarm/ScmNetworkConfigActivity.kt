package com.azazo1.writingarm

import android.os.Bundle
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.wrapContentSize
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.Button
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Switch
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

class ScmNetworkConfigActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            WritingArmClientandroidTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    NetConfigApp(
                        modifier = Modifier
                            .padding(innerPadding)
                            .fillMaxSize()
                    ) inner@{ ssid, pwd, ap ->
                        if (pwd.length < 8 || ssid.isEmpty()) {
                            Toast.makeText(this, getString(R.string.failed), Toast.LENGTH_SHORT)
                                .show()
                            return@inner
                        }
                        if (ap) {
                            ConnectionActivity.client?.modeAP(ssid, pwd)
                        } else {
                            ConnectionActivity.client?.modeSTA(ssid, pwd)
                        }
                    }
                    AutoNetworkAlert {
                        finish()
                    }
                }
            }
        }
    }
}

@Composable
fun NetConfigApp(
    modifier: Modifier = Modifier,
    onConfirm: (ssid: String, pwd: String, ap: Boolean) -> Unit,
) {
    var ssid by remember {
        mutableStateOf("")
    }
    var pwd by remember {
        mutableStateOf("")
    }
    var ap by remember {
        mutableStateOf(false)
    }
    Column(
        modifier = modifier,
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        OutlinedTextField(
            value = ssid,
            onValueChange = { ssid = it },
            maxLines = 1,
            keyboardOptions = KeyboardOptions.Default.copy(keyboardType = KeyboardType.Text), // 可以输入小数点.
            label = { Text(stringResource(R.string.ssid)) }
        )
        OutlinedTextField(
            value = pwd,
            onValueChange = { pwd = it },
            maxLines = 1,
            keyboardOptions = KeyboardOptions.Default.copy(keyboardType = KeyboardType.Password),
            label = { Text(stringResource(R.string.password)) }
        )
        Row(
            modifier = Modifier
                .padding(10.dp)
                .wrapContentSize(Alignment.Center),
            horizontalArrangement = Arrangement.Center,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                if (ap) {
                    stringResource(R.string.ap)
                } else {
                    stringResource(R.string.sta)
                },
            )
            Spacer(modifier = Modifier.width(10.dp))
            Switch(checked = ap, onCheckedChange = { ap = it })
        }
        Button(onClick = { onConfirm(ssid, pwd, ap) }) {
            Text(stringResource(R.string.confirm))
        }
    }
}