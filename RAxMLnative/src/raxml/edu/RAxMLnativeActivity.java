package raxml.edu;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
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
  public String dataFilePath;
  public String treeFileName;
  public String treeFilePath;
  public String outFileName;
  public int model;
  public boolean useMedian;

  /** Called when the activity is first created. */
  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.main);
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
    	useMedian = Integer.parseInt(outParameter1.getText().toString());
	    EditText outParameter1 = (EditText) findViewById(R.id.parameter1);
    	*/
	    Switch useMedianSwitch = (Switch) findViewById(R.id.useMedian);
	    useMedian = useMedianSwitch.isChecked();
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