package raxml.edu;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.app.Activity;
import android.content.Intent;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.text.Editable;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.Switch;
import android.widget.TextView;

public class RAxMLnativeActivity<CurrentActivity> extends Activity {
  NativeRAxML nativeLib;
  FileChooser chooser;
  public String dataFileName;
  public String treeFileName;
  public String outFileName;
  public int model;
  public boolean useMedian;

  /** Called when the activity is first created. */
  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.main);
    // Copy data file to /sdcard
    copyFile(this,"tiny.binary");
    copyFile(this,"RAxML_parsimonyTree.tiny.0");
    nativeLib = new NativeRAxML();
    // Setup the UI
    Button buttonCalc = (Button) findViewById(R.id.buttonCalc);
    buttonCalc.setOnClickListener(new OnClickListener() {
      TextView result = (TextView) findViewById(R.id.result);

      public void onClick(View v) {
    	String res = "not run yet";
    	/*  Get parameters from the UI */
	    TextView dataFileNameText = (TextView) findViewById(R.id.DataFileName);
	    TextView treeFileNameText = (TextView) findViewById(R.id.TreeFileName);
	    EditText outFileNameText = (EditText) findViewById(R.id.outFileName);
	    dataFileName = dataFileNameText.getText().toString();
	    treeFileName = treeFileNameText.getText().toString();
	    outFileName = outFileNameText.getText().toString();
	    /*
	    Switch useMedianSwitch = (Switch) findViewById(R.id.useMedian);
	    useMedian = useMedianSwitch.isChecked();
	    */
	    RadioButton catButton = (RadioButton) findViewById(R.id.cat);
	    RadioButton gammaButton = (RadioButton) findViewById(R.id.gamma);
	    RadioButton gamma_iButton = (RadioButton) findViewById(R.id.gamma_i);
	    if(catButton.isChecked()) 
	    	model = 0;
	    if(gammaButton.isChecked()) 
	    	model = 1;
	    if(gamma_iButton.isChecked()) 
	    	model = 2;
        res = nativeLib.raxml_main(dataFileName,treeFileName,outFileName,
        		model,useMedian);
        result.setText(res);
      }
    });
    /*
    Button buttonSelectData = (Button) findViewById(R.id.buttonSelectData);
    buttonSelectData.setOnClickListener(new OnClickListener() {
		public void onClick(View v) {
			Intent myIntent = new Intent(RAxMLnativeActivity.this, FileChooser.class);
			RAxMLnativeActivity.this.startActivityForResult(myIntent,0);
		}
    });
    Button buttonSelectTree = (Button) findViewById(R.id.buttonSelectTree);
    buttonSelectTree.setOnClickListener(new OnClickListener() {
		public void onClick(View v) {
			Intent myIntent = new Intent(RAxMLnativeActivity.this, FileChooser.class);
			RAxMLnativeActivity.this.startActivityForResult(myIntent,1);
		}
    });
    */
    TextView dataFileNameText = (TextView) findViewById(R.id.DataFileName);
    dataFileNameText.setOnClickListener(new OnClickListener() {
		public void onClick(View v) {
			Intent myIntent = new Intent(RAxMLnativeActivity.this, FileChooser.class);
			RAxMLnativeActivity.this.startActivityForResult(myIntent,0);
		}
    });
    TextView treeFileNameText = (TextView) findViewById(R.id.TreeFileName);
    treeFileNameText.setOnClickListener(new OnClickListener() {
		public void onClick(View v) {
			Intent myIntent = new Intent(RAxMLnativeActivity.this, FileChooser.class);
			RAxMLnativeActivity.this.startActivityForResult(myIntent,1);
		}
    });
  }
  @Override
  protected void onActivityResult(int requestCode, int resultCode, Intent intent) {
	super.onActivityResult(requestCode,resultCode,intent);
	Bundle newBundle = intent.getExtras();
    TextView dataFileNameText = (TextView) findViewById(R.id.DataFileName);
    TextView treeFileNameText = (TextView) findViewById(R.id.TreeFileName);
    if(requestCode == 0) {
	    dataFileName = newBundle.getString("fileName");
	    dataFileNameText.setText(dataFileName);
    }
    if(requestCode == 1) {
	    treeFileName = newBundle.getString("fileName");
	    treeFileNameText.setText(treeFileName);
    }
  }
  public static void copyFile(Activity c, String filename) {
      AssetManager assetManager = c.getAssets();
      InputStream in = null;
      OutputStream out = null;
      try 
      {
          in = assetManager.open(filename);
          String newFileName = "/mnt/sdcard/"+filename;
          out = new FileOutputStream(newFileName);

          byte[] buffer = new byte[1024];
          int read;
          while ((read = in.read(buffer)) != -1) {
              out.write(buffer, 0, read);
          }
          in.close();
          in = null;
          out.flush();
          out.close();
          out = null;
      } catch (Exception e) {
    	  Log.e("fileCopy Exception",e.getMessage());
      } finally {
          if(in!=null){
              try {
                  in.close();
              } catch (IOException e) {
                  Log.e("ERROR", "Exception while closing input stream",e);
              }
          }
          if(out!=null){
              try {
                  out.close();
              } catch (IOException e) {
                  Log.e("ERROR", "Exception while closing output stream",e);
      } } } }

}