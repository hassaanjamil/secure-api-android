package com.app.sample.secureapi

object SecureKeyLibrary {
    init {
        System.loadLibrary("secure-values")
    }

    external fun getAPIKey(flavor:Int): String
}