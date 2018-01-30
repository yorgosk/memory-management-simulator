/*
Author: George Kamaras
Date: 24/1/2017
*/
#ifndef __HEADER__
#define __HEADER__

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring> // for strncpy(), strcmp()

#include <assert.h>   // for debugging

/* K, Q, MAXREF and trace-files names can be global variables */
extern int K, Q, FRAMES, MAXREF;
extern std::string input1, input2;

typedef struct page_table_entry {
  int pid;            // process number
  char reference[6];  // reference number
  char state;         // 'W' or 'R'
  // member functions
  void initializeEntry();   // initialize a PT entry
  void printEntry();        // print a PT entry (for debugging)
} entry;

void initializePageTable(entry*);                 // initialize a page table entry
void printPageTable(entry*);                      // print page table (created for debugging)
void insertToPageTable(entry*, int, char*, char); // update the page table in a proper way
int checkForPageFault(entry*, int, char*, char);  // check for page fault
int flushPageTable(entry*, int);                  // flush page table from entries of a certain pid

#endif
