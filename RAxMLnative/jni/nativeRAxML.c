#include "raxml_edu_NativeRAxML.h"
JNIEXPORT jint JNICALL Java_raxml_edu_NativeRAxML_add
	(JNIEnv * env, jobject obj, jint value1, jint value2) {
		return (value1 + value2);
	}

JNIEXPORT jstring JNICALL Java_raxml_edu_NativeRAxML_hello
	(JNIEnv * env, jobject obj) {
		return (*env)->NewStringUTF(env, "Hello World!");
	}
