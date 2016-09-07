package com.ardumotor;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Locale;

import android.graphics.Color;
import android.os.Handler;
import android.support.v7.app.ActionBarActivity;
import android.support.v7.app.ActionBar;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.app.FragmentPagerAdapter;
import android.os.Bundle;
import android.support.v4.view.ViewPager;
import android.text.Spannable;
import android.text.method.ScrollingMovementMethod;
import android.text.style.BackgroundColorSpan;
import android.text.style.ForegroundColorSpan;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

public class MainActivity extends ActionBarActivity implements ActionBar.TabListener, WebSocketClient.StatusChange {

    /**
     * The {@link android.support.v4.view.PagerAdapter} that will provide
     * fragments for each of the sections. We use a
     * {@link FragmentPagerAdapter} derivative, which will keep every
     * loaded fragment in memory. If this becomes too memory intensive, it
     * may be best to switch to a
     * {@link android.support.v4.app.FragmentStatePagerAdapter}.
     */
    SectionsPagerAdapter mSectionsPagerAdapter;

    /**
     * The {@link ViewPager} that will host the section contents.
     */
    ViewPager mViewPager;

    final WebSocketClient client = new WebSocketClient();
    final Handler handler = new Handler();
    private int lastPingAccumMs = 0;
    private int prevPing = 0;
    private HashMap<Integer, Long> pingSend = new HashMap<Integer, Long>();
    SimpleDateFormat dataFormat = new SimpleDateFormat("hh:mm:ss");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Set up the action bar.
        final ActionBar actionBar = getSupportActionBar();
        actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_TABS);

        // Create the adapter that will return a fragment for each of the three
        // primary sections of the activity.
        mSectionsPagerAdapter = new SectionsPagerAdapter(getSupportFragmentManager());

        // Set up the ViewPager with the sections adapter.
        mViewPager = (ViewPager) findViewById(R.id.pager);
        mViewPager.setAdapter(mSectionsPagerAdapter);

        // When swiping between different sections, select the corresponding
        // tab. We can also use ActionBar.Tab#select() to do this if we have
        // a reference to the Tab.
        mViewPager.setOnPageChangeListener(new ViewPager.SimpleOnPageChangeListener() {
            @Override
            public void onPageSelected(int position) {
                actionBar.setSelectedNavigationItem(position);
            }
        });

        // For each of the sections in the app, add a tab to the action bar.
        for (int i = 0; i < mSectionsPagerAdapter.getCount(); i++) {
            // Create a tab with text corresponding to the page title defined by
            // the adapter. Also specify this Activity object, which implements
            // the TabListener interface, as the callback (listener) for when
            // this tab is selected.
            actionBar.addTab(
                    actionBar.newTab()
                            .setText(mSectionsPagerAdapter.getPageTitle(i))
                            .setTabListener(this));
        }
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onTabSelected(ActionBar.Tab tab, FragmentTransaction fragmentTransaction) {
        // When the given tab is selected, switch to the corresponding page in
        // the ViewPager.
        mViewPager.setCurrentItem(tab.getPosition());
    }

    @Override
    public void onTabUnselected(ActionBar.Tab tab, FragmentTransaction fragmentTransaction) {
    }

    @Override
    public void onTabReselected(ActionBar.Tab tab, FragmentTransaction fragmentTransaction) {
    }

    @Override
    protected void onStart() {
        super.onStart();
        initializeWs();
        Runnable loop = new Runnable() {
            long lastMs = System.currentTimeMillis();

            @Override
            public void run() {
                long nowMs = System.currentTimeMillis();
                try {
                    MainActivity.this.loop((int)(nowMs - lastMs));
                } catch (Exception ex) {
                    ex.printStackTrace();
                } finally {
                    lastMs = nowMs;
                    handler.postDelayed(this, 100);
                }
            }
        };
        handler.postDelayed(loop, 100);
    }

    private void initializeWs() {
        client.start("ws://192.168.1.1");
        client.addStatusChangeHandler(this);
        client.on("pong", new WebSocketClient.Handler() {
            @Override
            public void handle(String cmd, String payload) {

            }
        });
        client.on("pong_arduino", new WebSocketClient.Handler() {
            @Override
            public void handle(String cmd, String payload) {

            }
        });
        client.on("debug", new WebSocketClient.Handler() {
            @Override
            public void handle(String cmd, String payload) {

            }
        });
        client.on("debug_arduino", new WebSocketClient.Handler() {
            @Override
            public void handle(String cmd, String payload) {

            }
        });
        client.on("hi", new WebSocketClient.Handler() {
            @Override
            public void handle(String cmd, String payload) {

            }
        });
        client.on("hi_arduino", new WebSocketClient.Handler() {
            @Override
            public void handle(String cmd, String payload) {

            }
        });
        client.on("disconnected", new WebSocketClient.Handler() {
            @Override
            public void handle(String cmd, String payload) {

            }
        });
    }

    private void loop(int deltaMs) {
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



    private void refreshPingPongUI() {
        log("info", "Ping");
        log("warn", "PingW");
        log("error", "PingE");
    }

    @Override
    public void onWsStatusChanged(boolean connected) {
        // ???
    }

    private void log(String level, String msg) {
        String data = dataFormat.format(new Date());
        TextView tv = (TextView) findViewById(R.id.console_text);
        int start, end;
        Spannable spannableText;

        start = tv.getText().length();
        tv.append(data);
        end = tv.getText().length();

        spannableText = (Spannable) tv.getText();
        spannableText.setSpan(new BackgroundColorSpan(Color.LTGRAY), start, end, 0);
        spannableText.setSpan(new ForegroundColorSpan(Color.BLACK), start, end, 0);

        start = tv.getText().length();
        tv.append(" " + msg);
        end = tv.getText().length();

        spannableText = (Spannable) tv.getText();
        if (level.equals("warn"))
            spannableText.setSpan(new ForegroundColorSpan(Color.YELLOW), start, end, 0);
        else if (level.equals("error"))
            spannableText.setSpan(new ForegroundColorSpan(Color.RED), start, end, 0);
        else
            spannableText.setSpan(new ForegroundColorSpan(Color.BLACK), start, end, 0);

        tv.append("\n");

        while (tv.canScrollVertically(1)) {
            tv.scrollBy(0, 10);
        }
    }



    /**
     * A {@link FragmentPagerAdapter} that returns a fragment corresponding to
     * one of the sections/tabs/pages.
     */
    public class SectionsPagerAdapter extends FragmentPagerAdapter {

        public SectionsPagerAdapter(FragmentManager fm) {
            super(fm);
        }

        @Override
        public Fragment getItem(int position) {
            // getItem is called to instantiate the fragment for the given page.
            // Return a PlaceholderFragment (defined as a static inner class below).
            return PlaceholderFragment.newInstance(position + 1);
        }

        @Override
        public int getCount() {
            // Show 3 total pages.
            return 3;
        }

        @Override
        public CharSequence getPageTitle(int position) {
            Locale l = Locale.getDefault();
            switch (position) {
                case 0:
                    return getString(R.string.title_section1).toUpperCase(l);
                case 1:
                    return getString(R.string.title_section2).toUpperCase(l);
                case 2:
                    return getString(R.string.title_section3).toUpperCase(l);
            }
            return null;
        }
    }

    /**
     * A placeholder fragment containing a simple view.
     */
    public static class PlaceholderFragment extends Fragment {
        /**
         * The fragment argument representing the section number for this
         * fragment.
         */
        private static final String ARG_SECTION_NUMBER = "section_number";

        /**
         * Returns a new instance of this fragment for the given section
         * number.
         */
        public static PlaceholderFragment newInstance(int sectionNumber) {
            PlaceholderFragment fragment = new PlaceholderFragment();
            Bundle args = new Bundle();
            args.putInt(ARG_SECTION_NUMBER, sectionNumber);
            fragment.setArguments(args);
            return fragment;
        }

        public PlaceholderFragment() {
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                 Bundle savedInstanceState) {
            View rootView = inflater.inflate(R.layout.fragment_main, container, false);
            TextView textView = (TextView) rootView.findViewById(R.id.console_text);
            textView.setMovementMethod(new ScrollingMovementMethod());
            return rootView;
        }
    }

}
