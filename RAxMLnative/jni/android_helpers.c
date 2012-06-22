#include "android_helpers.h"
/* some functions for the likelihood computation */
boolean isTip(int number, int maxTips) {
  assert(number > 0);
  if(number <= maxTips)
    return TRUE;
  else
    return FALSE;
}

unsigned int KISS32() {
  static unsigned int 
    x = 123456789, 
    y = 362436069,
    z = 21288629,
    w = 14921776,
    c = 0;
  unsigned int t;
  x += 545925293;
  y ^= (y<<13); 
  y ^= (y>>17); 
  y ^= (y<<5);
  t = z + w + c; 
  z = w; 
  c = (t>>31); 
  w = t & 2147483647;
  return (x+y+w);
}

boolean whitechar (int ch) {
  return (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r');
}

void hookup (nodeptr p, nodeptr q, double *z, int numBranches) {
  int i;
  p->back = q;
  q->back = p;
  for(i = 0; i < numBranches; i++)
    p->z[i] = q->z[i] = z[i];
}

void hookupDefault (nodeptr p, nodeptr q, int numBranches) {
  int i;
  p->back = q;
  q->back = p;

  for(i = 0; i < numBranches; i++)
    p->z[i] = q->z[i] = defaultz;
}

void *malloc_aligned(size_t size) {
  void *ptr = (void *)NULL;
  int res;
#if defined (__APPLE__) || defined (ANDROID)
  /* 
     presumably malloc on MACs always returns 
     a 16-byte aligned pointer
  */
  ptr = malloc(size);
  
  if(ptr == (void*)NULL) 
   assert(0);
#ifdef __AVX
  assert(0);
#endif
#else
  res = posix_memalign( &ptr, BYTE_ALIGNMENT, size );
  if(res != 0) 
    assert(0);
#endif 
  return ptr;
}

boolean setupTree(tree *tr) {
  nodeptr  p0, p, q;
  int i, j;
  size_t tips, inter; 
  
  tr->bigCutoff = FALSE;
  tr->maxCategories = MAX(4, tr->categories);
  tr->partitionContributions = (double *)malloc(sizeof(double) * (size_t)tr->NumberOfModels);
  
  for(i = 0; i < tr->NumberOfModels; i++)
    tr->partitionContributions[i] = -1.0;
  
  tr->perPartitionLH = (double *)malloc(sizeof(double) * (size_t)tr->NumberOfModels);
  
  for(i = 0; i < tr->NumberOfModels; i++)    
    tr->perPartitionLH[i] = 0.0;	    
  
  tips  = (size_t)tr->mxtips;
  inter = (size_t)(tr->mxtips - 1);
  
  tr->fracchanges  = (double *)malloc((size_t)tr->NumberOfModels * sizeof(double));
  tr->treeStringLength = tr->mxtips * (nmlngth+128) + 256 + tr->mxtips * 2;
  tr->tree_string  = (char*)calloc((size_t)tr->treeStringLength, sizeof(char)); 
  tr->tree0 = (char*)calloc((size_t)tr->treeStringLength, sizeof(char));
  tr->tree1 = (char*)calloc((size_t)tr->treeStringLength, sizeof(char));
  /*TODO, must that be so long ?*/
  tr->td[0].count = 0;
  tr->td[0].ti    = (traversalInfo *)malloc(sizeof(traversalInfo) * (size_t)tr->mxtips);
  tr->td[0].executeModel = (boolean *)malloc(sizeof(boolean) * (size_t)tr->NumberOfModels);
  tr->td[0].parameterValues = (double *)malloc(sizeof(double) * (size_t)tr->NumberOfModels);
  
  for(i = 0; i < tr->NumberOfModels; i++)
    tr->fracchanges[i] = -1.0;

  tr->fracchange = -1.0;
  tr->constraintVector = (int *)malloc((2 * (size_t)tr->mxtips) * sizeof(int));
  tr->nameList = (char **)malloc(sizeof(char *) * (tips + 1));

  p0 = (nodeptr)malloc((tips + 3 * inter) * sizeof(node));
  assert(p0);
 
  tr->nodeBaseAddress = p0;
  tr->nodep = (nodeptr *) malloc((2* (size_t)tr->mxtips) * sizeof(nodeptr));
  assert(tr->nodep);    
  tr->nodep[0] = (node *) NULL;    /* Use as 1-based array */

  for (i = 1; i <= tips; i++) {
      p = p0++;
      p->hash   =  KISS32(); /* hast table stuff */
      p->x      =  0;
      p->xBips  = 0;
      p->number =  i;
      p->next   =  p;
      p->back   = (node *)NULL;
      p->bInf   = (branchInfo *)NULL;            
      tr->nodep[i] = p;
    }

  for (i = tips + 1; i <= tips + inter; i++) {
      q = (node *) NULL;
      for (j = 1; j <= 3; j++) {	 
	  p = p0++;
	  if(j == 1) {
	      p->xBips = 1;
	      p->x = 1;
	    }
	  else {
	      p->xBips = 0;
	      p->x =  0;
	    }
	  p->number = i;
	  p->next   = q;
	  p->bInf   = (branchInfo *)NULL;
	  p->back   = (node *) NULL;
	  p->hash   = 0;       
	  q = p;
	}
      p->next->next->next = p;
      tr->nodep[i] = p;
    }

  tr->likelihood  = unlikely;
  tr->start       = (node *) NULL;  
  tr->ntips       = 0;
  tr->nextnode    = 0;
 
  for(i = 0; i < tr->numBranches; i++)
    tr->partitionSmoothed[i] = FALSE;

  tr->bitVectors = (unsigned int **)NULL;
  tr->vLength = 0;
  tr->h = (hashtable*)NULL;
  tr->nameHash = initStringHashTable(10 * tr->mxtips);
  tr->partitionData = (pInfo*)malloc(sizeof(pInfo) * (size_t)tr->NumberOfModels);
  return TRUE;
}

