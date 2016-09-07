package com.ardumotor;

import de.tavendo.autobahn.WebSocketConnection;
import de.tavendo.autobahn.WebSocketException;

import java.lang.reflect.Array;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Queue;

/**
 * WS helper methods
 */
public class WebSocketClient extends WebSocketConnection {

    private HashMap<String, Handler> listeners = new HashMap<String, Handler>();
    private StatusChange statusChange = null;
    private Queue<String> sendQueue = new ArrayDeque<String>();

    public void send(String cmd, String payload) {
        String msg = cmd + ":" + payload;
        if (isConnected()) {
            sendTextMessage(msg);
        } else {
            sendQueue.add(msg);
        }
    }

    public void addStatusChangeHandler(StatusChange handler) {
        if (statusChange != null)
            throw new IllegalArgumentException("StatusChange listener already added");
        statusChange = handler;
    }

    public void on(String cmd, Handler handler) {
        if (listeners.containsKey(cmd))
            throw new IllegalArgumentException(cmd + " already added");
        listeners.put(cmd, handler);
    }

    public void start(String wsUri) {
        if (isConnected()) {
            onStatusChanged();
            disconnect();
        }
        try {
            connect(wsUri, new ConnectionHandler() {
                @Override
                public void onOpen() {
                    onStatusChanged();
                }

                @Override
                public void onClose(int i, String s) {
                    onStatusChanged();
                }

                @Override
                public void onTextMessage(String s) {
                    int idx = s.indexOf(':');
                    String cmd = s.substring(0, idx);
                    String payload = s.substring(idx + 1);
                    dispatch(cmd, payload);
                }

                @Override
                public void onRawTextMessage(byte[] bytes) {

                }

                @Override
                public void onBinaryMessage(byte[] bytes) {

                }
            });
        } catch (WebSocketException e) {
            e.printStackTrace();
        }
    }

    protected void dispatch(String cmd, String payload) {
        Handler handler = listeners.get(cmd);
        if (handler != null)
            handler.handle(cmd, payload);
    }

    protected void onStatusChanged() {
        if (statusChange != null)
            statusChange.onWsStatusChanged(isConnected());
        while (isConnected() && !sendQueue.isEmpty())
            sendTextMessage(sendQueue.remove());
    }

    public interface Handler {
        void handle(String cmd, String payload);
    }

    public interface StatusChange {
        void onWsStatusChanged(boolean connected);
    }
}
