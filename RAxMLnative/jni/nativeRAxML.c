/* include NDK binding */
#include "raxml_edu_NativeRAxML.h"
/* include binding to raxml */
#include "raxml_main.h"
/* small helper functions */
double gettime() {
	struct timeval ttime;
	gettimeofday(&ttime , NULL);
	return ttime.tv_sec + ttime.tv_usec * 0.000001;
}

static void initAdef(analdef *adef) {   
  adef->max_rearrange          = 21;
  adef->stepwidth              = 5;
  adef->initial                = 10;
  adef->bestTrav               = 10;
  adef->initialSet             = FALSE; 
  adef->mode                   = BIG_RAPID_MODE; 
  adef->likelihoodEpsilon      = 0.1;
  adef->permuteTreeoptimize    = FALSE; 
  adef->perGeneBranchLengths   = FALSE;  
  adef->useCheckpoint          = FALSE;
#ifdef _BAYESIAN 
  adef->bayesian               = FALSE;
  adef->num_generations        = 10000;
#endif
}

JNIEXPORT jint JNICALL Java_raxml_edu_NativeRAxML_add
	(JNIEnv * env, jobject obj, jint value1, jint value2) {
		return (value1 + value2);
	}

JNIEXPORT jstring JNICALL Java_raxml_edu_NativeRAxML_hello
	(JNIEnv * env, jobject obj) {
		return (*env)->NewStringUTF(env, "Hello World!");
	}
JNIEXPORT jint JNICALL Java_raxml_edu_NativeRAxML_raxml_1main
  (JNIEnv * env, jobject obj, jstring dataFileName, jstring treeFileName, jint parameter1) {
  tree  *tr = (tree*)malloc(sizeof(tree));
  analdef *adef = (analdef*)malloc(sizeof(analdef));
  double **empiricalFrequencies;
  /*
   * TODO: implement denormalized floating point on arm
#if ! (defined(__ppc) || defined(__powerpc__) || defined(PPC))
   _mm_setcsr( _mm_getcsr() | _MM_FLUSH_ZERO_ON);
#endif 
   */
  masterTime = gettime();         
  /* initialize the analysis parameters in struct adef to default values */
  initAdef(adef);
  return 0;
}
