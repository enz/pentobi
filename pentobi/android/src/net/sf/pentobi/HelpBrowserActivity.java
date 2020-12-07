package net.sf.pentobi;

import android.app.Activity;
import android.os.Bundle;
import android.net.Uri;
import android.webkit.WebView;

public class HelpBrowserActivity
    extends Activity
{
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        try
        {
            super.onCreate(savedInstanceState);
            setContentView(R.layout.help_browser_activity);
            WebView webView = (WebView)findViewById(R.id.webview);
            webView.loadUrl(getIntent().getData().toString());
        }
        catch (Exception e)
        {
        }
    }
}
