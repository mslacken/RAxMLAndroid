package raxml.edu;

public class NativeRAxML {
	static {
	    System.loadLibrary("nativeRAxML");
	  }
	  
	  /** 
	   * Adds two integers, returning their sum
	   */
	  public native int add( int v1, int v2 );
	  
	  /**
	   * Returns Hello World string
	   */
	  public native String hello();
}
