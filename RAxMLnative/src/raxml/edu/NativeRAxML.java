package raxml.edu;

public class NativeRAxML {
	static {
	    System.loadLibrary("nativeRAxML");
	  }
	  public native int raxml_main(String dataFileName, String treeFileName,
			  String outFileName,int model, boolean useMedian);
}
	