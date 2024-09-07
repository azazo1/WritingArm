package com.azazo1.writingarm

import android.content.Intent
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.wrapContentSize
import androidx.compose.material3.Button
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import com.azazo1.writingarm.ui.theme.WritingArmClientandroidTheme

class ModeSelectingActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            WritingArmClientandroidTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    ModeSelector(modifier = Modifier.padding(innerPadding))
                    AutoNetworkAlert {
                        finish()
                    }
                }
            }
        }
    }
}

@Composable
fun ModeSelector(modifier: Modifier = Modifier) {
    val ctx = LocalContext.current
    Column(
        modifier = modifier
            .padding(10.dp)
            .fillMaxSize()
            .wrapContentSize(Alignment.Center),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.SpaceBetween,
    ) {
        Button(onClick = {
            ctx.startActivity(Intent(ctx, PenActivity::class.java))
        }) {
            Text(text = stringResource(R.string.pen_drawing))
        }
        Button(onClick = {
            ctx.startActivity(Intent(ctx, ScmNetworkConfigActivity::class.java))
        }) {
            Text(text = stringResource(R.string.scm_network_config))
        }
    }
}