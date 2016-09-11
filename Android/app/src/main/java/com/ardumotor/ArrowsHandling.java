package com.ardumotor;

import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.os.Vibrator;

import java.util.HashMap;

/**
 * Trait for arrow buttons handling
 */
abstract public class ArrowsHandling extends InitializationCmdHandling {

    protected Vibrator vibrator;
    private HashMap<String, Boolean> keyPressed = new HashMap<String, Boolean>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (this.getApplicationContext().checkCallingOrSelfPermission("android.permission.VIBRATE") == PackageManager.PERMISSION_GRANTED)
            vibrator = (Vibrator) this.getApplicationContext().getSystemService(Context.VIBRATOR_SERVICE);
    }

    protected void addListeners() {
        ImageButton left = (ImageButton)findViewById(R.id.arrow_left_btn);
        ImageButton right = (ImageButton)findViewById(R.id.arrow_right_btn);
        ImageButton up = (ImageButton)findViewById(R.id.arrow_up_btn);
        ImageButton down = (ImageButton)findViewById(R.id.arrow_down_btn);

        left.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                return onArrowBtnEvent((ImageButton) v, "left", event);
            }
        });

        right.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                return onArrowBtnEvent((ImageButton) v, "right", event);
            }
        });

        up.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                return onArrowBtnEvent((ImageButton) v, "up", event);
            }
        });

        down.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                return onArrowBtnEvent((ImageButton) v, "down", event);
            }
        });
    }

    private boolean onArrowBtnEvent(ImageButton btn, String key, MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                if (vibrator != null)
                    vibrator.vibrate(50);
                onArrowClicked(btn, key, true);
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                onArrowClicked(btn, key, false);
                break;
        }
        return true;
    }

    private void onArrowClicked(ImageButton btn, String key, boolean pressed) {
        Boolean curPressed0 = keyPressed.get(key);
        boolean curPressed = (curPressed0 == null ? false : curPressed0);
        if (pressed != curPressed) {
            client.send("arrow", key + ':' + (pressed ? "down" : "up"));
            keyPressed.put(key, pressed);
        }
    }
}
