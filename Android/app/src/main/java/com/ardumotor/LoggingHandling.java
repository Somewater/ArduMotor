package com.ardumotor;

import android.graphics.Color;
import android.os.Build;
import android.text.Spannable;
import android.text.style.BackgroundColorSpan;
import android.text.style.ForegroundColorSpan;
import android.widget.TextView;

import java.text.SimpleDateFormat;
import java.util.ArrayDeque;
import java.util.Date;
import java.util.Queue;

/**
 * Handle logging from any threads: first move log entry to queue and handle it later in UI thread
 */
abstract public class LoggingHandling extends ArrowsHandling {

    SimpleDateFormat dataFormat = new SimpleDateFormat("hh:mm:ss");
    Queue<LogEntry> logQueue = new ArrayDeque<LogEntry>();

    @Override
    protected void loop(int deltaMs) {
        processLogQueue();
        super.loop(deltaMs);
    }

    public void log(String level, String msg) {
        String data = dataFormat.format(new Date());
        synchronized (logQueue) {
            logQueue.add(new LogEntry(data, level, msg));
        }
    }

    void processLogQueue() {
        TextView tv = (TextView) findViewById(R.id.console_text);
        if (tv == null) {
            return;
        }

        int i = 0;
        synchronized (logQueue) {
            while (!logQueue.isEmpty() && i < 100) {
                LogEntry entry = logQueue.remove();
                int start, end;
                Spannable spannableText;

                start = tv.getText().length();
                tv.append(entry.data);
                end = tv.getText().length();

                spannableText = (Spannable) tv.getText();
                spannableText.setSpan(new BackgroundColorSpan(Color.LTGRAY), start, end, 0);
                spannableText.setSpan(new ForegroundColorSpan(Color.BLACK), start, end, 0);

                start = tv.getText().length();
                tv.append(" " + entry.msg);
                end = tv.getText().length();

                spannableText = (Spannable) tv.getText();
                if (entry.level.equals("warn"))
                    spannableText.setSpan(new ForegroundColorSpan(Color.YELLOW), start, end, 0);
                else if (entry.level.equals("error"))
                    spannableText.setSpan(new ForegroundColorSpan(Color.RED), start, end, 0);
                else
                    spannableText.setSpan(new ForegroundColorSpan(Color.BLACK), start, end, 0);

                tv.append("\n");
                i++;
            }
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD) {
            while (tv.canScrollVertically(1)) {
                tv.scrollBy(0, 10);
            }
        }
    }

    private static class LogEntry {
        public String data;
        public String level;
        public String msg;

        public LogEntry(String data, String level, String msg) {
            this.data = data;
            this.level = level;
            this.msg = msg;
        }
    }
}
