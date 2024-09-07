package com.azazo1.writingarm

import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.res.stringResource

@Composable
fun AutoNetworkAlert(finish: () -> Unit) {
    var showFinishingAlert by remember {
        mutableStateOf(false)
    }
    var showWaitingAlert by remember {
        mutableStateOf(ConnectionActivity.client?.connected != true);
    }
    LaunchedEffect(ConnectionActivity.client?.alive) {
        if (ConnectionActivity.client == null || ConnectionActivity.client?.alive == false) {
            showFinishingAlert = true
            showWaitingAlert = false
        }
    }
    LaunchedEffect(ConnectionActivity.client?.connected) {
        if (ConnectionActivity.client?.connected == true) {
            showWaitingAlert = false
        }
    }
    if (showFinishingAlert) {
        AlertDialog(onDismissRequest = {
            showFinishingAlert = false
//            finish()
        }, confirmButton = {
            Button(onClick = {
                showFinishingAlert = false
//                finish()
            }) {
                Text(text = stringResource(R.string.ok))
            }
        }, title = {
            Text(text = stringResource(R.string.network))
        }, text = {
            Text(text = stringResource(R.string.client_disconnected))
        })
    }
    if (showWaitingAlert) {
        AlertDialog(
            onDismissRequest = { },
            confirmButton = {
                Button(onClick = { }) {
                    Text(stringResource(R.string.wait))
                }
            },
            title = { Text(stringResource(R.string.network)) },
            text = { Text(stringResource(R.string.connecting)) }
        )
    }
}