package com.othellosensei.app

import android.app.Activity
import android.content.Intent
import android.os.Bundle

class IntentBouncerActivity : Activity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        if (intent?.action == Intent.ACTION_SEND) {
            val forwardIntent = Intent(this, MainActivity::class.java).apply {
                action = intent.action
                type = intent.type
                clipData = intent.clipData
                intent.extras?.let { putExtras(it) }
                addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            }
            startActivity(forwardIntent)
        }

        finish()
    }
}