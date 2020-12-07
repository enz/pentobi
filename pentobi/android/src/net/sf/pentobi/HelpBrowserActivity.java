package net.sf.pentobi;

import android.app.Activity;
import android.os.Bundle;
import android.net.Uri;
import android.webkit.WebView;
import android.webkit.WebViewClient;

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
            webView.setWebViewClient(new WebViewClient() {
                @Override
                public boolean shouldOverrideUrlLoading(WebView view,
                                                        String url)
                {
                    view.loadUrl(url);
                    return true;
                }
            });
            webView.loadUrl(getIntent().getData().toString());
        }
        catch (Exception e)
        {
        }
    }
}
