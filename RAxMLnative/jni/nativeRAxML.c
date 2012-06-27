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

/* global variables*/
char char_buffer[1024] = "";
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
	  __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, ("\n Error: the file %s you want to open for reading does not exist, exiting ...\n\n", path));
	  return (FILE *)NULL;
	}
    }
  else {
      if(fp)
	return fp;
      else {	 
	  __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, ("\n Error: the file %s you want to open for writing or appending can not be opened [mode: %s], exiting ...\n\n", path, mode));
	  return (FILE *)NULL;
	} } }

static int iterated_bitcount(unsigned int n) {
    int count=0;    
    while(n) {
        count += n & 0x1u ;    
        n >>= 1 ;
	}
    return count;
}

static void compute_bits_in_16bits(char *bits_in_16bits) {
    unsigned int i;    
    
    /* size is 65536 */

    for (i = 0; i < (0x1u<<16); i++)
        bits_in_16bits[i] = iterated_bitcount(i);       
    return ;
}

const partitionLengths *getPartitionLengths(pInfo *p) {
  int dataType  = p->dataType, states = p->states, tipLength = p->maxTipStates;
  assert(states != -1 && tipLength != -1);
  assert(MIN_MODEL < dataType && dataType < MAX_MODEL);
  return (&pLengths[dataType]); 
}
size_t discreteRateCategories(int rateHetModel) {
  size_t result;
  switch(rateHetModel) {
    case CAT:
      result = 1;
      break;
    case GAMMA:
      result = 4;
      break;
    default:
      assert(0);
    }
  return result;
}
int getUndetermined(int dataType) {
  assert(MIN_MODEL < dataType && dataType < MAX_MODEL);
  return pLengths[dataType].undetermined;
}

static void initializePartitions(tree *tr, tree *localTree, int tid, int n) { 
  size_t model, maxCategories;
  localTree->threadID = tid; 
  compute_bits_in_16bits(localTree->bits_in_16bits);
  assert(tr == localTree);
  for(model = 0; model < (size_t)tr->NumberOfModels; model++)
    assert(tr->partitionData[model].width == tr->partitionData[model].upper - tr->partitionData[model].lower);
  maxCategories = (size_t)localTree->maxCategories;
  for(model = 0; model < (size_t)localTree->NumberOfModels; model++) {
      size_t j, width = localTree->partitionData[model].width;
      const partitionLengths 
	*pl = getPartitionLengths(&(localTree->partitionData[model]));
      localTree->partitionData[model].wr = (double *)malloc(sizeof(double) * width);
      localTree->partitionData[model].wr2 = (double *)malloc(sizeof(double) * width);     
      /* 
	 globalScaler needs to be 2 * localTree->mxtips such that scalers of inner AND tip nodes can be added without a case switch
	 to this end, it must also be initialized with zeros -> calloc
       */

      localTree->partitionData[model].globalScaler    = (unsigned int *)calloc(2 *(size_t)localTree->mxtips, sizeof(unsigned int));  	         
      localTree->partitionData[model].left              = (double *)malloc_aligned((size_t)pl->leftLength * (maxCategories + 1) * sizeof(double));
      localTree->partitionData[model].right             = (double *)malloc_aligned((size_t)pl->rightLength * (maxCategories + 1) * sizeof(double));
      localTree->partitionData[model].EIGN              = (double*)malloc((size_t)pl->eignLength * sizeof(double));
      localTree->partitionData[model].EV                = (double*)malloc_aligned((size_t)pl->evLength * sizeof(double));
      localTree->partitionData[model].EI                = (double*)malloc((size_t)pl->eiLength * sizeof(double));
      localTree->partitionData[model].substRates        = (double *)malloc((size_t)pl->substRatesLength * sizeof(double));
      localTree->partitionData[model].frequencies       = (double*)malloc((size_t)pl->frequenciesLength * sizeof(double));
      localTree->partitionData[model].empiricalFrequencies       = (double*)malloc((size_t)pl->frequenciesLength * sizeof(double));
      localTree->partitionData[model].tipVector         = (double *)malloc_aligned((size_t)pl->tipVectorLength * sizeof(double));
      localTree->partitionData[model].symmetryVector    = (int *)malloc((size_t)pl->symmetryVectorLength  * sizeof(int));
      localTree->partitionData[model].frequencyGrouping = (int *)malloc((size_t)pl->frequencyGroupingLength  * sizeof(int));
      localTree->partitionData[model].perSiteRates      = (double *)malloc(sizeof(double) * maxCategories);
      localTree->partitionData[model].nonGTR = FALSE;            
      localTree->partitionData[model].gammaRates = (double*)malloc(sizeof(double) * 4);
      localTree->partitionData[model].yVector = (unsigned char **)malloc(sizeof(unsigned char*) * ((size_t)localTree->mxtips + 1));
      localTree->partitionData[model].xVector = (double **)malloc(sizeof(double*) * (size_t)localTree->mxtips);   
      for(j = 0; j < (size_t)localTree->mxtips; j++)	        	  	  	  	 
		  localTree->partitionData[model].xVector[j]   = (double*)NULL;   
      localTree->partitionData[model].xSpaceVector = (size_t *)calloc((size_t)localTree->mxtips, sizeof(size_t));  
      localTree->partitionData[model].sumBuffer = (double *)malloc_aligned(width *
									   (size_t)(localTree->partitionData[model].states) *
									   discreteRateCategories(localTree->rateHetModel) *
									   sizeof(double));
      localTree->partitionData[model].wgt = (int *)malloc_aligned(width * sizeof(int));	  
      /* rateCategory must be assigned using calloc() at start up there is only one rate category 0 for all sites */
      localTree->partitionData[model].rateCategory = (int *)calloc(width, sizeof(int));
      if(width > 0 && localTree->saveMemory) {
	  localTree->partitionData[model].gapVectorLength = ((int)width / 32) + 1;
	  localTree->partitionData[model].gapVector = (unsigned int*)calloc((size_t)localTree->partitionData[model].gapVectorLength * 2 * (size_t)localTree->mxtips, sizeof(unsigned int));	  	    	  	  
	  localTree->partitionData[model].gapColumn = (double *)malloc_aligned(((size_t)localTree->mxtips) *								      
									       ((size_t)(localTree->partitionData[model].states)) *
									       discreteRateCategories(localTree->rateHetModel) * sizeof(double));
	} else {
	   localTree->partitionData[model].gapVectorLength = 0;
	   localTree->partitionData[model].gapVector = (unsigned int*)NULL; 	  	    	   
	   localTree->partitionData[model].gapColumn = (double*)NULL;	    	    	   
	} }
#if ! (defined(_USE_PTHREADS) || defined(_FINE_GRAIN_MPI))
  /* figure in tip sequence data per-site pattern weights */ 
  for(model = 0; model < (size_t)tr->NumberOfModels; model++) {
      size_t j, lower = tr->partitionData[model].lower, width = tr->partitionData[model].upper - lower;  
      for(j = 1; j <= (size_t)tr->mxtips; j++)
		tr->partitionData[model].yVector[j] = &(tr->yVector[j][tr->partitionData[model].lower]);
      memcpy((void*)(&(tr->partitionData[model].wgt[0])), (void*)(&(tr->aliaswgt[lower])), sizeof(int) * width);
    }  
#else
  {
    size_t model,  j, i, globalCounter = 0, localCounter  = 0, offset, countOffset, myLength = 0;
    for(model = 0; model < (size_t)localTree->NumberOfModels; model++)
      myLength += localTree->partitionData[model].width;         
    /* assign local memory for storing sequence data */
    localTree->y_ptr = (unsigned char *)malloc(myLength * (size_t)(localTree->mxtips) * sizeof(unsigned char));
    assert(localTree->y_ptr != NULL);
    for(i = 0; i < (size_t)localTree->mxtips; i++) {
	for(model = 0, offset = 0, countOffset = 0; model < (size_t)localTree->NumberOfModels; model++) {
	    localTree->partitionData[model].yVector[i+1]   = &localTree->y_ptr[i * myLength + countOffset];
	    countOffset +=  localTree->partitionData[model].width;
	  }
	assert(countOffset == myLength);
      }

    /* figure in data */
    if(tr->manyPartitions)
      for(model = 0, globalCounter = 0; model < (size_t)localTree->NumberOfModels; model++) {
	  if(isThisMyPartition(localTree, tid, model)) {
	      assert(localTree->partitionData[model].upper - localTree->partitionData[model].lower == localTree->partitionData[model].width);
	      for(localCounter = 0, i = (size_t)localTree->partitionData[model].lower;  i < (size_t)localTree->partitionData[model].upper; i++, localCounter++) {	    
		  localTree->partitionData[model].wgt[localCounter] = tr->aliaswgt[globalCounter];
		  for(j = 1; j <= (size_t)localTree->mxtips; j++)
		    localTree->partitionData[model].yVector[j][localCounter] = tr->yVector[j][globalCounter]; 	     
		  globalCounter++;
		} }
	  else
	    globalCounter += (localTree->partitionData[model].upper - localTree->partitionData[model].lower);
	}
    else
      for(model = 0, globalCounter = 0; model < (size_t)localTree->NumberOfModels; model++) {
	  for(localCounter = 0, i = (size_t)localTree->partitionData[model].lower;  i < (size_t)localTree->partitionData[model].upper; i++) {
	      if(i % (size_t)n == (size_t)tid) {
		  localTree->partitionData[model].wgt[localCounter]          = tr->aliaswgt[globalCounter];	      	     		 
		  for(j = 1; j <= (size_t)localTree->mxtips; j++)
		    localTree->partitionData[model].yVector[j][localCounter] = tr->yVector[j][globalCounter]; 	     
		  localCounter++;
		}
	      globalCounter++;
	    } } }
#endif
  /* initialize gap bit vectors at tips when memory saving option is enabled */
  if(localTree->saveMemory) {
      for(model = 0; model < (size_t)localTree->NumberOfModels; model++) {
	  int undetermined = getUndetermined(localTree->partitionData[model].dataType);
	  size_t i, j, width =  localTree->partitionData[model].width;
	  if(width > 0) {	   	    	      	    	     
	      for(j = 1; j <= (size_t)(localTree->mxtips); j++)
		for(i = 0; i < width; i++)
		  if(localTree->partitionData[model].yVector[j][i] == undetermined)
		    localTree->partitionData[model].gapVector[localTree->partitionData[model].gapVectorLength * j + i / 32] |= mask32[i % 32];	    
	    }     
} } }

const unsigned int *getBitVector(int dataType) {
	assert(MIN_MODEL < dataType && dataType < MAX_MODEL);
	return pLengths[dataType].bitVector;
}

void storeExecuteMaskInTraversalDescriptor(tree *tr) {
   int model;
   for(model = 0; model < tr->NumberOfModels; model++)
     tr->td[0].executeModel[model] = tr->executeModel[model];
}

void getxnode (nodeptr p) {
  nodeptr  s;
  if ((s = p->next)->x || (s = s->next)->x) {
      p->x = s->x;
      s->x = 0;
    }
  assert(p->x);
}

unsigned int precomputed16_bitcount (unsigned int n, char *bits_in_16bits) {
	/* works only for 32-bit int*/
    return bits_in_16bits [n         & 0xffffu]
        +  bits_in_16bits [(n >> 16) & 0xffffu] ;
}

void storeValuesInTraversalDescriptor(tree *tr, double *value) {
   int model;
   for(model = 0; model < tr->NumberOfModels; model++)
     tr->td[0].parameterValues[model] = value[model];
}

void printLog(tree *tr) {
  FILE *logFile;
  double t;
  t = gettime() - masterTime;
  logFile = myfopen(logFileName, "ab");
  fprintf(logFile, "%f %f\n", t, tr->likelihood);
  fclose(logFile);
}

void printBothOpen(const char* format, ... ) {
	/*
  FILE *f = myfopen(infoFileName, "ab");

  va_list args;
  va_start(args, format);
  vfprintf(f, format, args );
  va_end(args);

  va_start(args, format);
  vprintf(format, args );
  va_end(args);

  fclose(f);
  */
}

void printResult(tree *tr, analdef *adef, boolean finalPrint) {
  FILE *logFile;
  char temporaryFileName[1024] = "";
  strcpy(temporaryFileName, resultFileName);
  switch(adef->mode) {    
    case TREE_EVALUATION:
      Tree2String(tr->tree_string, tr, tr->start->back, TRUE, TRUE, FALSE, FALSE, finalPrint, SUMMARIZE_LH, FALSE, FALSE);
      logFile = myfopen(temporaryFileName, "wb");
      fprintf(logFile, "%s", tr->tree_string);
      fclose(logFile);

      if(adef->perGeneBranchLengths)
	printTreePerGene(tr, adef, temporaryFileName, "wb");
      break;
    case BIG_RAPID_MODE:     
      if(finalPrint) {
	  switch(tr->rateHetModel) {
	    case GAMMA:
	    case GAMMA_I:
	      Tree2String(tr->tree_string, tr, tr->start->back, TRUE, TRUE, FALSE, FALSE, finalPrint,
			  SUMMARIZE_LH, FALSE, FALSE);
	      logFile = myfopen(temporaryFileName, "wb");
	      fprintf(logFile, "%s", tr->tree_string);
	      fclose(logFile);
	      if(adef->perGeneBranchLengths)
		printTreePerGene(tr, adef, temporaryFileName, "wb");
	      break;
	    case CAT:
	      /*Tree2String(tr->tree_string, tr, tr->start->back, FALSE, TRUE, FALSE, FALSE, finalPrint, adef,
		NO_BRANCHES, FALSE, FALSE);*/
	      Tree2String(tr->tree_string, tr, tr->start->back, TRUE, TRUE, FALSE, FALSE,
			  TRUE, SUMMARIZE_LH, FALSE, FALSE);
	      logFile = myfopen(temporaryFileName, "wb");
	      fprintf(logFile, "%s", tr->tree_string);
	      fclose(logFile);
	      break;
	    default:
	      assert(0);
	    }
	} else {
	  Tree2String(tr->tree_string, tr, tr->start->back, FALSE, TRUE, FALSE, FALSE, finalPrint,
		      NO_BRANCHES, FALSE, FALSE);
	  logFile = myfopen(temporaryFileName, "wb");
	  fprintf(logFile, "%s", tr->tree_string);
	  fclose(logFile);
	}    
      break;
    default:
      __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"FATAL ERROR call to printResult from undefined STATE %d\n", adef->mode);
      exit(-1);
      break;
    }
}
static void finalizeInfoFile(tree *tr, analdef *adef) {
  double t;
  t = gettime() - masterTime;
  accumulatedTime = accumulatedTime + t;
  switch(adef->mode)
    {	
    case  BIG_RAPID_MODE:	 
      printBothOpen("\n\nOverall Time for 1 Inference %f\n", t);
      printBothOpen("\nOverall accumulated Time (in case of restarts): %f\n\n", accumulatedTime);
      printBothOpen("Likelihood   : %f\n", tr->likelihood);
      printBothOpen("\n\n");	  	  
      printBothOpen("Final tree written to:                 %s\n", resultFileName);
      printBothOpen("Execution Log File written to:         %s\n", logFileName);
      printBothOpen("Execution information file written to: %s\n",infoFileName);	
      break;
    default:
      assert(0);
    }
}
double randum (long  *seed) {
  long  sum, mult0, mult1, seed0, seed1, seed2, newseed0, newseed1, newseed2;
  double res;

  mult0 = 1549;
  seed0 = *seed & 4095;
  sum  = mult0 * seed0;
  newseed0 = sum & 4095;
  sum >>= 12;
  seed1 = (*seed >> 12) & 4095;
  mult1 =  406;
  sum += mult0 * seed1 + mult1 * seed0;
  newseed1 = sum & 4095;
  sum >>= 12;
  seed2 = (*seed >> 24) & 255;
  sum += mult0 * seed2 + mult1 * seed1;
  newseed2 = sum & 255;

  *seed = newseed2 << 24 | newseed1 << 12 | newseed0;
  res = 0.00390625 * (newseed2 + 0.000244140625 * (newseed1 + 0.000244140625 * newseed0));

  return res;
}

JNIEXPORT jstring JNICALL Java_raxml_edu_NativeRAxML_raxml_1main
  (JNIEnv * env, jobject obj, jstring dataFileName, jstring treeFileName, jstring outFileName,
   jint model, jboolean useMedian) {
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Started native RAxML");
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
	/********* tr inits end*************/
    /* now setup the options we get from the android interface */
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "initialized standard values");
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
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "generated filenames");
	{
		size_t i, model;
		unsigned char *y;
		FILE *byteFile = myfopen(byteFileName, "rb");	 
		if(byteFile == NULL) {
			sprintf(char_buffer,"could not open data file: %s",byteFileName);
			return (*env)->NewStringUTF(env,char_buffer);
		}
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "opened file: %s",byteFileName);
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
			/*__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, ("%s \n", tr->nameList[i]));*/
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
	/* 
	 allocate the required data structures for storing likelihood vectors etc 
	*/
	initializePartitions(tr, tr, 0, 0);
	initModel(tr, empiricalFrequencies);                      
	/* 
	 not important, only used to keep track of total accumulated exec time 
	 when checkpointing and restarts were used 
	 */
	accumulatedTime = 0.0;
      /* get the starting tree: here we just parse the tree passed via the command line 
	 and do an initial likelihood computation traversal 
	 which we maybe should skeip, TODO */
	switch(tr->startingTree) {
	case randomTree:
	  makeRandomTree(tr);
	  break;
	case givenTree:
	  getStartingTree(tr);     
	  break;
	case parsimonyTree:	     
	  /* runs only on process/thread 0 ! */
	  allocateParsimonyDataStructures(tr);
	  makeParsimonyTreeFast(tr);
	  freeParsimonyDataStructures(tr);
	  break;
	default:
	  assert(0);
	}
	/* 
	here we do an initial full tree traversal on the starting tree using the Felsenstein pruning algorithm 
	This should basically be the first call to the library that actually computes something :-)
	*/
	evaluateGeneric(tr, tr->start, TRUE);	 
	/* the treeEvaluate() function repeatedly iterates over the entire tree to optimize branch lengths until convergence */
	treeEvaluate(tr, 1); 	 	 	 	 	 
	/* now start the ML search algorithm */
	computeBIGRAPID(tr, adef, TRUE); 	     
	finalizeInfoFile(tr, adef);

	return (*env)->NewStringUTF(env, "finished RAxML");
}
