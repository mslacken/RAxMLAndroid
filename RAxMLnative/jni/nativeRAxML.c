/* include NDK binding */
#ifndef RAXML_MAIN_H
#define RAXML_MAIN_H
/* system includes, shoudl be compatible to stdclib */
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
/* raxml includes */
#include "phylogenetic-likelihood-library/axml.h"
#include "phylogenetic-likelihood-library/globalVariables.h"
#endif
/* include access to logcat */
#include <android/log.h>
#define APPNAME "RAxMLnative"

/* glue for jni */
#include <jni.h>
#define JNI_FALSE  0
#define JNI_TRUE   1
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

static void myBinFread(void *ptr, size_t size, size_t nmemb, FILE *byteFile) {  
	size_t bytes_read;
	bytes_read = fread(ptr, size, nmemb, byteFile);
	assert(bytes_read == nmemb);
}

FILE *myfopen(const char *path, const char *mode) {
  FILE *fp = fopen(path, mode);

  if(strcmp(mode,"r") == 0 || strcmp(mode,"rb") == 0) {
      if(fp)
	return fp;
      else {	  
	  printf("\n Error: the file %s you want to open for reading does not exist, exiting ...\n\n", path);
	  exit(-1);
	  return (FILE *)NULL;
	}
    }
  else {
      if(fp)
	return fp;
      else {	 
	  printf("\n Error: the file %s you want to open for writing or appending can not be opened [mode: %s], exiting ...\n\n",
		 path, mode);
	  exit(-1);
	  return (FILE *)NULL;
	}
}



}

JNIEXPORT jint JNICALL Java_raxml_edu_NativeRAxML_raxml_1main
  (JNIEnv * env, jobject obj, jstring dataFileName, jstring treeFileName, jstring outFileName,
   jint model, jboolean useMedian) {
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Started native RAxML");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Started native RAxML");
	return 0;
	tree *tr = (tree*)malloc(sizeof(tree));
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
	/* no getargs as we get all parameters with pseudo main call */

	/*********** tr inits **************/
	tr->numberOfThreads = 1; 
	tr->doCutoff = TRUE;
	tr->secondaryStructureModel = SEC_16; /* default setting */
	tr->searchConvergenceCriterion = FALSE;
	tr->rateHetModel = GAMMA;
	tr->multiStateModel  = GTR_MULTI_STATE;
	tr->useGappedImplementation = FALSE;
	tr->saveMemory = FALSE;
	tr->manyPartitions = FALSE;
	tr->startingTree = randomTree;
	tr->randomNumberSeed = 12345;
	tr->categories             = 25;
	tr->grouped = FALSE;
	tr->constrained = FALSE;
	tr->gapyness               = 0.0; 
	tr->useMedian = FALSE;
	/********* tr inits end*************/
    /* now setup the options we get from the android interface */
	if(useMedian == JNI_TRUE)
		tr->useMedian = TRUE;
	/* get the data file */
	const char *nativeStringData = (*env)->GetStringUTFChars(env,dataFileName,0);
	strcpy(byteFileName,nativeStringData);	 	
	const char *nativeStringTree = (*env)->GetStringUTFChars(env,treeFileName,0);
	strcpy(tree_file,nativeStringTree);	 	
	tr->startingTree = givenTree;
	/* get the rate model */
	tr->rateHetModel = model;
	/* generate the ouput file names */
	strcpy(resultFileName,(*env)->GetStringUTFChars(env,outFileName,0));
	strcpy(logFileName,(*env)->GetStringUTFChars(env,outFileName,0));  
	strcpy(infoFileName,(*env)->GetStringUTFChars(env,outFileName,0));
	strcat(resultFileName,"RAxML_result.");
	strcat(logFileName,"RAxML_log.");  
	strcat(infoFileName,"RAxML_info.");
	/* end of file name generation */
	{
		size_t i, model;
		unsigned char *y;
		FILE *byteFile = myfopen(byteFileName, "rb");	 
		myBinFread(&(tr->mxtips),                 sizeof(int), 1, byteFile);
		myBinFread(&(tr->originalCrunchedLength), sizeof(int), 1, byteFile);
		myBinFread(&(tr->NumberOfModels),         sizeof(int), 1, byteFile);
		myBinFread(&(tr->gapyness),            sizeof(double), 1, byteFile);
		if(adef->perGeneBranchLengths)
			tr->numBranches = tr->NumberOfModels;
		else
			tr->numBranches = 1;

		/* If we use the RF-based convergence criterion we will need to allocate some hash tables.
		 let's not worry about this right now, because it is indeed RAxML-specific */
		tr->aliaswgt = (int *)malloc((size_t)tr->originalCrunchedLength * sizeof(int));
		myBinFread(tr->aliaswgt, sizeof(int), tr->originalCrunchedLength, byteFile);	       
		tr->rateCategory    = (int *)    malloc((size_t)tr->originalCrunchedLength * sizeof(int));	  
		tr->wr              = (double *) malloc((size_t)tr->originalCrunchedLength * sizeof(double)); 
		tr->wr2             = (double *) malloc((size_t)tr->originalCrunchedLength * sizeof(double)); 
		tr->patrat          = (double*)  malloc((size_t)tr->originalCrunchedLength * sizeof(double));
		tr->patratStored    = (double*)  malloc((size_t)tr->originalCrunchedLength * sizeof(double)); 
		tr->lhs             = (double*)  malloc((size_t)tr->originalCrunchedLength * sizeof(double)); 
		tr->executeModel    = (boolean *)malloc(sizeof(boolean) * (size_t)tr->NumberOfModels);
		for(i = 0; i < (size_t)tr->NumberOfModels; i++)
			tr->executeModel[i] = TRUE;

		empiricalFrequencies = (double **)malloc(sizeof(double *) * (size_t)tr->NumberOfModels);
		y = (unsigned char *)malloc(sizeof(unsigned char) * ((size_t)tr->originalCrunchedLength) * ((size_t)tr->mxtips));
		tr->yVector = (unsigned char **)malloc(sizeof(unsigned char *) * ((size_t)(tr->mxtips + 1)));

		for(i = 1; i <= (size_t)tr->mxtips; i++)
			tr->yVector[i] = &y[(i - 1) *  (size_t)tr->originalCrunchedLength];	

		setupTree(tr);

		/* data structures for convergence criterion need to be initialized after! setupTree */
		if(tr->searchConvergenceCriterion) {                     
			tr->bitVectors = initBitVector(tr->mxtips, &(tr->vLength));
			tr->h = initHashTable(tr->mxtips * 4);        
		}
		for(i = 1; i <= (size_t)tr->mxtips; i++) {
			int len;
			myBinFread(&len, sizeof(int), 1, byteFile);
			tr->nameList[i] = (char*)malloc(sizeof(char) * (size_t)len);
			myBinFread(tr->nameList[i], sizeof(char), len, byteFile);
			/*printf("%s \n", tr->nameList[i]);*/
		}  
		for(i = 1; i <= (size_t)tr->mxtips; i++)
			addword(tr->nameList[i], tr->nameHash, i);
    
		for(model = 0; model < (size_t)tr->NumberOfModels; model++) {
			int len;

			pInfo *p = &(tr->partitionData[model]);	   

			myBinFread(&(p->states),             sizeof(int), 1, byteFile);
			myBinFread(&(p->maxTipStates),       sizeof(int), 1, byteFile);
			myBinFread(&(p->lower),              sizeof(int), 1, byteFile);
			myBinFread(&(p->upper),              sizeof(int), 1, byteFile);
			myBinFread(&(p->width),              sizeof(int), 1, byteFile);
			myBinFread(&(p->dataType),           sizeof(int), 1, byteFile);
			myBinFread(&(p->protModels),         sizeof(int), 1, byteFile);
			myBinFread(&(p->autoProtModels),     sizeof(int), 1, byteFile);
			myBinFread(&(p->protFreqs),          sizeof(int), 1, byteFile);
			myBinFread(&(p->nonGTR),             sizeof(boolean), 1, byteFile);
			myBinFread(&(p->numberOfCategories), sizeof(int), 1, byteFile);	 
			/* later on if adding secondary structure data
			   int    *symmetryVector;
			   int    *frequencyGrouping;
			*/
			myBinFread(&len, sizeof(int), 1, byteFile);
			p->partitionName = (char*)malloc(sizeof(char) * (size_t)len);
			myBinFread(p->partitionName, sizeof(char), len, byteFile);

			empiricalFrequencies[model] = (double *)malloc(sizeof(double) * (size_t)tr->partitionData[model].states);
			myBinFread(empiricalFrequencies[model], sizeof(double), tr->partitionData[model].states, byteFile);	   
		  }
		myBinFread(y, sizeof(unsigned char), ((size_t)tr->originalCrunchedLength) * ((size_t)tr->mxtips), byteFile);
		fclose(byteFile);
	}

	return 7+model;
}
