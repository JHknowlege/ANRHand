package com.jhzl.anrhandle

import android.os.Bundle
import android.os.SystemClock
import android.util.Log
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        // Example of a call to a native method
        val pid = android.os.Process.myPid()
        Log.d(TAG,"pid =>"+pid)
        findViewById<Button>(R.id.start_anr_btn).setOnClickListener {
           waitTest()
        }

    }

    val lock = java.lang.Object()
    fun waitTest() {
        synchronized(lock) {
            try {
                lock.wait()
            } catch (e: InterruptedException) {
                e.printStackTrace()
            }
        }
    }


    companion object {
        val TAG = "onReceiveANRInfo"
        fun onReceiveANRInfo(anrInfo: String) {
            Log.d(TAG, "onReceiveANRInfo = >$anrInfo")
        }

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}