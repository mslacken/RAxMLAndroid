package raxml.edu;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class RAxMLnativeActivity<CurrentActivity> extends Activity {
  NativeRAxML nativeLib;
  FileChooser chooser;
  public String dataFileName;
  public String dataFilePath;
  public String treeFileName;
  public String treeFilePath;
  public int parameter1;

  /** Called when the activity is first created. */
  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.main);
    nativeLib = new NativeRAxML();
    String helloText = nativeLib.hello();
    // Update the UI
    TextView outText = (TextView) findViewById(R.id.textOut);
    outText.setText(helloText);
    // Setup the UI
    Button buttonCalc = (Button) findViewById(R.id.buttonCalc);
    buttonCalc.setOnClickListener(new OnClickListener() {
      TextView result = (TextView) findViewById(R.id.result);

      public void onClick(View v) {
    	int res = -1;
	    EditText outParameter1 = (EditText) findViewById(R.id.parameter1);
    	parameter1 = Integer.parseInt(outParameter1.getText().toString());
        res = nativeLib.raxml_main(dataFileName,treeFileName,parameter1);
        result.setText(new Integer(res).toString());
      }

    });
    
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
  }
  @Override
  protected void onActivityResult(int requestCode, int resultCode, Intent intent) {
	super.onActivityResult(requestCode,resultCode,intent);
	Bundle newBundle = intent.getExtras();
    TextView dataFileNameText = (TextView) findViewById(R.id.DataFileName);
    TextView treeFileNameText = (TextView) findViewById(R.id.TreeFileName);
    if(requestCode == 0) {
	    dataFileNameText.setText(newBundle.getString("fileName"));
	    dataFileName = newBundle.getString("fileName");
	    dataFilePath = newBundle.getString("pathName");
    }
    if(requestCode == 1) {
	    treeFileNameText.setText(newBundle.getString("fileName"));
	    treeFileName = newBundle.getString("fileName");
	    treeFilePath = newBundle.getString("pathName");
    }
  }
}