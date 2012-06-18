#include "raxml_edu_NativeRAxML.h"
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
	  return parameter1*23;
  }
