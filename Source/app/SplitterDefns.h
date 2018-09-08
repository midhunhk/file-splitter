/* SplitterDefns.h
 * Defines the constants and index file header used by FileSplitter
 *
 * June 2007 | Centrum inc Software Solutions
 */

 # include <ctype>
 
///////////////////////////////////////////
typedef struct
{
   char identifier[4];
   int version;
   int numSubFiles;
   unsigned long int mainFileSize;
   char mainFileExt[8];
   unsigned long fileCRC;
}theIndex;
///////////////////////////////////////////

const char logFile[28] = "FileSplitter_Process.log";

const int SPLIT = 12;
const int JOIN  = 24;
const int PROG_VERSION   = 1937;
const int NO_SOURCE_FILE = 1; 	// Could not open the source file
const int NO_SUB_FILE    = 2; 	// Could not open a sub file


//////////////////////////////////////
// Fn to parse an int from a string //
//////////////////////////////////////
int parseInt(const char *str)
{
 	int val = 0,i = 0;
   while(str[i]!='\0')
   {
   	if(isdigit(str[i]))
	    	val = val*10 + (str[i]-'0'); // computed using ASCII values
      i++;
   }
	return val;
}