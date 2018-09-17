package com.guidongyuan.studyndk;

import android.util.Log;

public class Bean {

    private static final String TAG = "StudyNDK";

    private int i = 0;

    public int getI() {
        Log.i(TAG, "getI i = "+i);
        return i;
    }

    public void setI(int i) {
        Log.i(TAG, "setI i = "+i);
        this.i = i;
    }
}
