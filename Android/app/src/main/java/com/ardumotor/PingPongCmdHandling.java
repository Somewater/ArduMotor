package com.ardumotor;

import android.app.Activity;
import android.support.v7.app.ActionBarActivity;
import android.util.Pair;
import android.widget.TextView;

import java.util.HashMap;
import java.util.Set;

/**
 * Trait for pong-pong protocol handling
 * @see "https://docs.google.com/drawings/d/1WH1TgprjyF7VVq3dk5nvMNvsJ5YJpwrI-7L2EGXhmCc"
 */
abstract public class PingPongCmdHandling extends ActionBarActivity {

    protected WebSocketClient client;

    private HashMap<Integer, Long> pingSend = new HashMap<Integer, Long>();
    private int lastPingAccumMs = 0;
    private int prevPing = 0;
    private Pair<Integer, Integer> pongReceived = null;
    private Pair<Integer, Integer> pongReceivedArduino = null;

    private TextView espPingTxt = null;
    private TextView arduinoPingTxt = null;

    protected int ping = -1;

    abstract public void log(String level, String message);

    protected void initializeWs() {
        client.on("pong", new WebSocketClient.Handler() {
            @Override
            public void handle(String cmd, String payload) {
                int num = Integer.parseInt(payload);
                if (pongReceived == null || pongReceived.first < num) {
                    Long pingSendTime = pingSend.get(num);
                    if (pingSendTime != null) {
                        int durationMs = (int) (System.currentTimeMillis() - pingSendTime);
                        pongReceived = new Pair<Integer, Integer>(num, durationMs);
                        refreshPingPongUI();
                    } else {
                        log("error", "Ancient pong received: " + payload);
                    }
                }
            }
        });
        client.on("pong_arduino", new WebSocketClient.Handler() {
            @Override
            public void handle(String cmd, String payload) {
                int num = Integer.parseInt(payload);
                if (pongReceivedArduino == null || pongReceivedArduino.first < num) {
                    int durationMs = (int)(System.currentTimeMillis() - pingSend.get(num));
                    pongReceivedArduino = new Pair<Integer, Integer>(num, durationMs);
                    refreshPingPongUI();
                }
            }
        });
    }

    protected void loop(int deltaMs) {
        lastPingAccumMs += deltaMs;
        if (lastPingAccumMs > 3000) {
            prevPing += 1;
            client.send("ping", Integer.toString(prevPing));
            lastPingAccumMs = 0;
            if (prevPing > 10)
                pingSend.remove(prevPing - 10);
            pingSend.put(prevPing, System.currentTimeMillis());
            refreshPingPongUI();
        }
    }

    void refreshPingPongUI() {
        int pong = 0;
        int pongArduino = 0;

        if (pongReceived != null)
            pong = pongReceived.first;

        if (pongReceivedArduino != null)
            pongArduino = pongReceivedArduino.first;

        Set<Integer> sendedNums = pingSend.keySet();
        int pingWoAnswer = -1;
        for (int key : sendedNums) {
            if (key > pong && (pingWoAnswer == -1 || pingWoAnswer > key))
                pingWoAnswer = key;
        }
        if (pingWoAnswer == -1)
            pingWoAnswer = pong;

        int pingWoAnswerArduino = -1;
        for (int key : sendedNums) {
            if (key > pongArduino && (pingWoAnswerArduino == -1 || pingWoAnswerArduino > key))
                pingWoAnswerArduino = key;
        }
        if (pingWoAnswerArduino == -1)
            pingWoAnswerArduino = pongArduino;

        long now = System.currentTimeMillis();
        ping = pingWoAnswer == pong ? this.pongReceived.second : (int)(now - this.pingSend.get(pingWoAnswer));
        int pingArduino = pingWoAnswerArduino == pongArduino ? this.pongReceivedArduino.second : (int)(now - this.pingSend.get(pingWoAnswerArduino));

        // UI refresh in fact
        if (espPingTxt == null)
            espPingTxt = (TextView) findViewById(R.id.espPingTxt);
        if (arduinoPingTxt == null)
            arduinoPingTxt = (TextView) findViewById(R.id.arduinoPingTxt);

        espPingTxt.setText(dd(ping, 5));
        arduinoPingTxt.setText(dd(pingArduino, 5));
    }

    // return digit as fixed length string with leading zeros
    private String dd(int number, int fixSize) {
        StringBuilder sb = new StringBuilder(fixSize);
        int digits = number > 0 ? (int) Math.log10((double) number) + 1 : 1;
        int l = fixSize - digits;
        int i = 0;
        while (i < l) {
            sb.append('0');
            i += 1;
        }
        sb.append(number);
        return sb.toString();
    }
}
