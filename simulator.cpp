/*
Author: George Kamaras
Date: 24/1/2017
*/
#include "header.h"

using namespace std;

/* K, Q, MAXREF and trace-files names can be global variables */
int K, Q, FRAMES, MAXREF;
string input1 = "bzip.trace", input2 = "gcc.trace";

int main(int argc, char const *argv[]) {
  /* checking parameters */
  if (argc < 4)
  {
    cerr << "You must give at least 3 parameters. One integer for the size K, one integer for the number of memory frames and another integer for the size of the reference-groups (Q). You can optionally give another integer for the maximum number of references to be examined. All in that order." << endl;
    exit(-1);
  }

  /* assign parameters' values */
  K = atoi(argv[1]), FRAMES = atoi(argv[2]), Q = atoi(argv[3]);
  if (argc == 4) MAXREF = 150000;
  else MAXREF = atoi(argv[4]);
  if (K < 0 || FRAMES < 0 || Q < 0 || MAXREF < 0)   // check if all parameters' values are greater than or equal to 0
  {
    cerr << "All parameters' numbers must be greater than or equal to zero." << endl;
    exit(-1);
  }
  if (2*K > FRAMES) // we can't have less than 2*K memory frames
  {
    cerr << "We must have at least 2*K memory frames." << endl;
    exit(-1);
  }

  /* open files for input */
  ifstream f1(input1.c_str()), f2(input2.c_str());
  if(!f1.is_open() || !f2.is_open()) // check if f1 and f2 are open
  {
    cerr << "Error at opening an input file" << endl;
    exit(-1);
  }

  /* our Inverted Page Table */
  entry IPT[FRAMES];

  /* how I work:
      I have an outer loop until MAXREF
      and an inner loop 1 -> Q with a step 1 where all the work happens
      statistics:
      disc read / write: for write when I flush how many 'W's I flush and
                          for read basically how many page faults (because if you think of it, you read something from disc to move forward)
      page faults: how many page faults happened
      references examined from reference files: I count both how many references I read from each trace-file and how many references I read totally
      number of memory frames: FRAMES */

  /* initialize Inverted Page Table */
  initializePageTable(IPT);

  bool first = true;
  int discwrites = 0, discreads = 0, input1pagefaults = 0, input2pagefaults = 0, totalpagefaults = 0, i1contpf = 0, i2contpf = 0,
      input1entries = 0, input2entries = 0, totalentries = 0;

  while(totalentries < MAXREF)  // until we have read MAXREF references
  {
    for (int i = 1; i <= Q && totalentries < MAXREF; i++) // if we reach MAXREF we need to stop anyway
    {
      int temppid;
      char a[10], tempref[6], b, initializer[6] = "00000";
      strcpy(tempref, initializer); // we initialize tempref to prevent memory leaks because of a conditional jump or move which depends on uninitialised value(s)

      if (first)  // if we read from the first input file
      {
        if (f1 >> a >> b)
        {
          temppid = 1;
          strncpy(tempref, a, 5*sizeof(char));
          assert(tempref[5] == '\0');
        }
        else
        {
          cerr << "Error at reading from " << input1 << endl;
          exit(-2);
        }
        if (checkForPageFault(IPT, temppid, tempref, b))  // if a page fault occured
        {
          i1contpf++;           // how many continuous page faults from input1
          input1pagefaults++;
          discreads++;
          totalpagefaults++;
          if (i1contpf == K+1)
          {
            discwrites += flushPageTable(IPT, 1);
            i1contpf = 1;           // we are dealing with a page fault right now
          }
        }
        input1entries++;
      }
      else   // if we read from the second input file
      {
        if (f2 >> a >> b)
        {
          temppid = 2;
          strncpy(tempref, a, 5*sizeof(char));
        }
        else
        {
          cerr << "Error at reading from " << input2 << endl;
          exit(-2);
        }
        if (checkForPageFault(IPT, temppid, tempref, b))  // if a page fault occured
        {
          i2contpf++;             // how many continuous page faults from input2
          input2pagefaults++;
          discreads++;
          totalpagefaults++;
          if (i2contpf == K+1)
          {
            discwrites += flushPageTable(IPT, 2);
            i2contpf = 1;         // we are dealing with a page fault right now
          }
        }
        input2entries++;
      }
      insertToPageTable(IPT, temppid, tempref, b);
      totalentries++;
    }
    first = !first;   // change input file
  }

  /* print statistics */
  cout << discwrites << " disc writes and " << discreads << " disc reads occured" << endl;
  cout << "Total Page Faults: " << totalpagefaults << ". Specifically:" << endl;
  cout << input1pagefaults << " Page Faults from " << input1 << " and " << input2pagefaults << " Page Faults from " << input2 << endl;
  cout << "Total entries examined: " << totalentries << ". Specifically:" << endl;
  cout << input1entries << " entries were examined from " << input1 << " and " << input2entries << " entries from " << input2 << endl;
  cout << "We used " << FRAMES << " memory frames" << endl;

  /* close input files */
  f1.close();
  f2.close();

  return 0;
}
