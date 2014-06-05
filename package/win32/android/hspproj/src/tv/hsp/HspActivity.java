package tv.hsp;

import android.app.NativeActivity;
import android.app.Activity;
import android.app.AlertDialog;

import android.content.pm.ActivityInfo;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.content.DialogInterface;
import android.content.ComponentName;

import android.net.Uri;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.FontMetrics;
import android.graphics.Paint.Style;
import android.graphics.Typeface;

import android.os.AsyncTask;
import android.os.Vibrator;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.widget.Toast;
import android.view.Display;
import android.view.WindowManager;
import android.view.View.OnClickListener;

import java.util.List;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Random;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.UnsupportedEncodingException;

public class HspActivity extends NativeActivity {

  // string code
  // public static final String CHAR_CODE = "Shift-JIS";
  public static final String CHAR_CODE = "UTF8";

  public String hello() {
    return "JNITest";
  }
  public String getInfo_Device() {
    return android.os.Build.DEVICE;
  }
  public String getInfo_Version() {
    return android.os.Build.VERSION.RELEASE;
  }
  public String getInfo_Locale() {
    return Locale.getDefault().getISO3Language();
  }
  public String getInfo_FilesDir() {
    File f=this.getApplicationContext().getFilesDir();
    String path=f.toString();
    return path;
  }

  public int callVibrator( int val ) {
    Vibrator vibrator = (Vibrator)getSystemService(VIBRATOR_SERVICE);
    if (vibrator != null) vibrator.vibrate(val);
    return 0;
  }
  public int ui_dispDialog( String msg1, String msg2, int type ) {

    AlertDialog.Builder alert = new AlertDialog.Builder(this);
    alert.setMessage( msg1 );
    alert.setTitle( msg2 );
    alert.setPositiveButton( "OK", null );
    if (( type & 1 ) > 0 ) {
        alert.setIcon(android.R.drawable.ic_dialog_alert);
    } else {
        alert.setIcon(android.R.drawable.ic_dialog_info);
    }
    alert.show();
    return 0;
  }

  public int callNamedActivity( String msg1, String msg2, int mode ) {

    Intent shareIntent = new Intent(Intent.ACTION_SEND);
    shareIntent.setType("text/plain");
    shareIntent.putExtra(Intent.EXTRA_TEXT, msg2 );
    String appName = msg1; //"twitter";
    int res = -1;

    PackageManager pm = getPackageManager();
    List<?> activityList = pm.queryIntentActivities(shareIntent, 0);
    int len = activityList.size();
    for (int i = 0; i < len; i++) {
	ResolveInfo app = (ResolveInfo) activityList.get(i);
	if ((app.activityInfo.name.contains(appName))) {
		if ( mode > 0 ) {
			ActivityInfo activity = app.activityInfo;
			ComponentName name = new ComponentName(activity.applicationInfo.packageName, activity.name);
			shareIntent.setComponent(name);
			startActivity(shareIntent);
		}
		res = 0;
		break;
	}
    }
    return res;
  }


  public int callActivity( String msg1, String msg2, int type ) {
    if ( type == 16 ) {
    	Uri uri = Uri.parse( msg1 );
	Intent i = new Intent();
	i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
	i.setAction(Intent.ACTION_VIEW);
	i.setData(uri);
   	startActivity(i);
   	return 0;
    }

    if ( type == 48 ) {
	return callNamedActivity( msg1, msg2, 1 );
    }

    Intent intent = new Intent();
    intent.setClassName( msg1, msg2 );
    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
    startActivity(intent);
    return 0;
  }

    public int dispDialog( String msg1, String msg2, int type ) {
	final int addtype;
	final String s_msg1;
	final String s_msg2;

	addtype = type;
	s_msg1 = msg1;
	s_msg2 = msg2;
        this.runOnUiThread( new Runnable() {
    		public void run() 
                {
			ui_dispDialog( s_msg1, s_msg2, addtype );
                }
        } );
   	return 0;
    }
    
    public int addWindowFlag( int type ) {
	final int addtype;
	if ( type == 1 ) {
		addtype = WindowManager.LayoutParams.FLAG_FULLSCREEN;
	} else {
		addtype = WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON;
	}
        this.runOnUiThread( new Runnable() {
    		public void run() 
                {
			getWindow().addFlags( addtype );
                }
        } );
   	return 0;
    }
    
    public int clearWindowFlag( int type ) {
	final int clrtype;
	if ( type == 1 ) {
		clrtype = WindowManager.LayoutParams.FLAG_FULLSCREEN;
	} else {
		clrtype = WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON;
	}
        this.runOnUiThread( new Runnable() {
	    	public void run() {
	        	getWindow().clearFlags( clrtype );
                }
        } );
   	return 0;
    }


  public native void nativepoke(int val, int val2);


	//	conversion ByteText->String
	public String ByteTextToString( byte[] byteText )
	{
		try {
			if( byteText == null ) return "";
			return new String(byteText, CHAR_CODE);
		}
		catch (UnsupportedEncodingException e) {
			e.printStackTrace();
			return "";
		}
	}
	

	//	retrieve font text by bitmap data
	public Bitmap getFontBitmap( byte[] byteText, int fontsize, boolean bBold )
	{
		String text = ByteTextToString( byteText );

	        Paint paint = new Paint();
	        paint.setStyle(Paint.Style.FILL);
	        paint.setColor(Color.WHITE);
	        paint.setTextSize(fontsize);
	        paint.setAntiAlias(true);
	        paint.setSubpixelText(true);
	        paint.setTypeface( Typeface.create( Typeface.SANS_SERIF, ( bBold ) ? Typeface.BOLD : Typeface.NORMAL ) );

	        FontMetrics fontMetrics = paint.getFontMetrics();

	        int width = (int)Math.round(paint.measureText(text));
	        if (width <= 0) {
			width = (int)Math.round(paint.measureText("0"));
	       	}

	        int offsetY = (int)Math.ceil(-fontMetrics.top);
	        int height = offsetY + (int)(Math.ceil(fontMetrics.bottom));
	        Bitmap bitmap = Bitmap.createBitmap(width,height,Bitmap.Config.ALPHA_8);
	        Canvas canvas = new Canvas(bitmap);
	        canvas.drawText(text,0, offsetY,paint);
	        return bitmap;
	}


}

