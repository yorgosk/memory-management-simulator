/*
Author: George Kamaras
Date: 24/1/2017
*/
#include "header.h"

using namespace std;

void entry::initializeEntry()
{
  pid = -1;   // it's enough to determine if an entry position is occupied
  char initializer[6] = "00000";
  strcpy(reference, initializer); // we initialize reference to prevent memory leaks because of a conditional jump or move which depends on uninitialised value(s)
  state = 'E';  // empty state
}

void entry::printEntry()
{
  cout << "\t" << pid << "\t\t" << reference << "\t\t" << state << endl;
}

void initializePageTable(entry* ipt)
{
  for (int i = 0; i < FRAMES; i++) ipt[i].initializeEntry(); // initialize each entry
}

void printPageTable(entry* ipt)
{
  cout << "\tPID\t\tReference\t\tState" << endl;
  cout << "\t___\t\t_________\t\t_____" << endl;
  for (int i = 0; i < FRAMES; i++) ipt[i].printEntry();  // print each entry
  cout << "\t___\t\t_________\t\t_____" << endl << endl;
}

void insertToPageTable(entry* ipt, int n, char* ref, char s)
{
  /* is there a trace with same pid and memory reference already in the page table? */
  for (int i = 0; i < 2*K; i++) // there is no point going further than 2*K
  {
    if (ipt[i].pid != -1) // "ipt[i].pid != -1" to prevent memory management errors when checking with an unitialized ipt[i].reference
    {
      if (ipt[i].pid == n && !strcmp(ipt[i].reference, ref))
      {
        if(ipt[i].state == 'R') ipt[i].state = s;
        return;
      }
    }
  }
  for (int i = 0; i < 2*K; i++)
  {
    if(ipt[i].pid == -1)  // if there is empty space for the entry
    {                     // insert the entry
      ipt[i].pid = n;
      strcpy(ipt[i].reference, ref);
      assert(ipt[i].reference[5] == '\0');
      ipt[i].state = s;
      break;
    }
  }
}

int checkForPageFault(entry* ipt, int n, char* ref, char s)
{
  for (int i = 0; i < 2*K; i++) // there is no point going further than 2*K
    if (ipt[i].pid != -1) // "ipt[i].pid != -1" to prevent memory management errors when checking with an unitialized ipt[i].reference
      if (ipt[i].pid == n && !strcmp(ipt[i].reference, ref))
        return 0;   // no page fault
  return 1;
}

int flushPageTable(entry* ipt, int n)
{
  int counter = 0;
  for (int i = 0; i < 2*K; i++) // there is no point going further than 2*K
    if(ipt[i].pid == n)
    {
      if (ipt[i].state == 'W') counter++; // we need to count how many times we write back to disc
      ipt[i].initializeEntry();   // when we flush an entry, we basically re-initialize it
    }
  return counter;   // return how many times we write back to disc
}
