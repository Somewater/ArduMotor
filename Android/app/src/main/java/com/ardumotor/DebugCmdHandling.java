package com.ardumotor;

/**
 * Trait for debug log handling
 * @see "https://docs.google.com/drawings/d/1WH1TgprjyF7VVq3dk5nvMNvsJ5YJpwrI-7L2EGXhmCc"
 */
abstract public class DebugCmdHandling extends PingPongCmdHandling {
    @Override
    protected void initializeWs() {
        super.initializeWs();
        client.on("debug", new WebSocketClient.Handler() {
            @Override
            public void handle(String cmd, String payload) {
                log("warn", "Debug: " + payload);
            }
        });
        client.on("debug_arduino", new WebSocketClient.Handler() {
            @Override
            public void handle(String cmd, String payload) {
                log("warn", "DebugArduino: " + payload);
            }
        });
    }
}
