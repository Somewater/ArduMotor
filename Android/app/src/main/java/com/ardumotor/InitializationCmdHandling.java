package com.ardumotor;

/**
 * Trait for initialization protocol handling
 * @see "https://docs.google.com/drawings/d/1WH1TgprjyF7VVq3dk5nvMNvsJ5YJpwrI-7L2EGXhmCc"
 */
abstract public class InitializationCmdHandling extends DebugCmdHandling {
    @Override
    protected void initializeWs() {
        super.initializeWs();
        client.on("hi", new WebSocketClient.Handler() {
            @Override
            public void handle(String cmd, String payload) {
                log("warn", "Hi: " + payload);
            }
        });

        client.on("hi_arduino", new WebSocketClient.Handler() {
            @Override
            public void handle(String cmd, String payload) {
                log("warn", "HiArduino: " + payload);
            }
        });

        client.on("disconnected", new WebSocketClient.Handler() {
            @Override
            public void handle(String cmd, String payload) {
                log("warn", "Disconnected: " + payload);
            }
        });
    }
}
