/* FileSplitter_1.cpp
 * FileSplitter Utility | Implementation in C++ [OOP] | OpenSource
 * June 2007 Centrum inc Software Solutions
 *
 * $ Version 1.9
 */

 // CommandLine : fileSplitter <-S> "<indexFilePath>" "<mainFilePath>" "<subFileTemplate>" <mainFileType> <numSubFile>
 //             : fileSplitter <-J> "<indexFilePath>" "<outptFilPath>" "<subFileTemplate>" {SPLIT} {1}

# include <dos>
# include <stdio>
# include <conio>
# include <string>
# include <fstream>
# include <iostream>
# include "SplitterDefns.h"
# include "FileCRC32.cpp"


/*******************************/
/* Class For The Split Objects */
/*******************************/

class splitter
{
protected:
	char process[10];
	char subFileIndex[2];
	char mainFileName[256],outputFileBackup[256];
   char nextTargetFile[256];
   char subFileNameTemplate[256];
   char formattedTimeString[32];
   unsigned long fileCRC;
   unsigned long int mainFileSize,subFileSize,lastSubFileSize,numSubFiles;
   int errorCode,numFilesProc;
   struct time initTime,finishTime;

// File Handlers
   FILE *ft,*fs;
   ofstream logger;

public:
	splitter() // no-arg constructor
   {
   	mainFileSize = subFileSize = lastSubFileSize = numFilesProc = 0;
      numSubFiles  = 1; // so that no divide by 0
      errorCode = 0;		// no errors
      strcpy(mainFileName,"");
      strcpy(outputFileBackup,"");
      strcpy(subFileIndex,"00");
      strcpy(subFileNameTemplate,"");
   }

   splitter(char* sOrig,char* sSub, unsigned int iCount)
   {  // 3 arg std constructor
		gettime(&initTime);
      formatTimeString(initTime);
   	mainFileSize = subFileSize = lastSubFileSize = numFilesProc = 0;
      numSubFiles  = iCount;
      errorCode    = 0;
      strcpy(mainFileName,sOrig);
      strcpy(subFileNameTemplate,sSub);
      strcpy(outputFileBackup,"");

      logger.open(logFile,ios::app);		// Open The Log File in append mode
      logger<<"=====Starting New Session=====";
      logger<<"\n\nsplitter::Constructor()";
      logger<<"\nInit Time       :"<<formattedTimeString;
      logger<<"\nNo of SubFiles  : "<<numSubFiles;
      logger<<"\nSubFileTemplate : "<<subFileNameTemplate;

   }
   ~splitter()
   	{ // std destructor
      	gettime(&finishTime);
         formatTimeString(finishTime);
         struct time tDiff = getProcessDuration();
      	logger<<"\n\n~splitter()::Process Completed.";
         logger<<"\nCompletion Time  :"<<formattedTimeString;
         formatTimeString(tDiff);
         logger<<"\nProcess Duration :"<<formattedTimeString;
         logger<<"\n"<<process<<numFilesProc<<" out of "<<numSubFiles<<"\n----------------------\n";
      }
   /**********Common*Functions*Used*************/
   // getErrorCode()
   // getMainFileSize()
   // getProcessDuration()
   // getIntermediateTime()
   // getFileCRC()
   // calcCRC32()
   // initSubFileIndexCounter()
   // getNextSubFile()
   // openNextSubFile()
   // formatTimeString()

   int getErrorCode() 				 // returns the error code
	   { return errorCode; }

   unsigned int getMainFileSize()
   	{ return mainFileSize;}

   inline void initSubFileIndexCounter()// initiates the indexCOunter, a major bugfix
   	{ strcpy(subFileIndex,"00");}

	void calcCRC32()              // calculates the CRC32 of the main file
   {
      FileCRC32 obj(mainFileName);
      fileCRC = obj.CalculateFileCRC();
      logger<<"\nFileCRC32::calcCRC32() ";
   }

   void formatTimeString(struct time t)
   {
    	strcpy(formattedTimeString,"\0");
      sprintf(formattedTimeString," [ %2d : %02d : %02d.%02d ] ",t.ti_hour, t.ti_min, t.ti_sec, t.ti_hund);
   }
   inline struct time getProcessDuration()
   {
   	struct time td;
      unsigned int init,final,durtn;
      init = getIntermediateTime(initTime);	// get integral values to aid subtraction, bugfix 1928
      final= getIntermediateTime(finishTime);
      durtn = final - init;

      td.ti_hund = durtn%100; durtn /= 100; // now set td with duration and send it to caller
      td.ti_sec  = durtn%100; durtn /= 100;
      td.ti_min  = durtn%100; durtn /= 100;
      td.ti_hour = durtn%100; durtn /= 100;
      return td;
   }

   inline unsigned int getIntermediateTime(struct time what)
	   { return (what.ti_hund + (what.ti_sec*100) + (what.ti_min*10000) + (what.ti_hour*1000000)) ;  }

   unsigned long getFileCRC()		// returns the CRC of the main file
   	{ return fileCRC; }

   void getNextSubFile() 			// Increments the file index
   {
      strcpy(nextTargetFile,"");
		strcat(nextTargetFile,subFileNameTemplate);
		strcat(nextTargetFile,subFileIndex);
      strcat(nextTargetFile,".split");
      logger<<"\n[#]subFileIndex : "<<subFileIndex;

  		if(subFileIndex[1] == '9')	// Updating the Count
      {
       	subFileIndex[0]++;
         subFileIndex[1] = '0';
      }
      else subFileIndex[1]++;
   }

   void openNextSubFile(int mode)
   {
     	getNextSubFile();
      if(mode==SPLIT) ft = fopen(nextTargetFile,"wb");
      if(mode==JOIN)  ft = fopen(nextTargetFile,"rb");
      if(ft)         numFilesProc++;
      else
      {
      	logger<<"\n[!] Error Opening subfile : "<<nextTargetFile;
         errorCode = NO_SUB_FILE;
      }
   }

   /**********Functions*for*Splitting*the*source*file*******/
   // write the index file with data
   // open the source file
   // write n-1 subfiles with subFileSize of data from the main file
   // the last file may have different amount of data, so write that separately

   void openSourceFile() // Opens The Source File and gets the file size
   {
		fs = fopen(mainFileName,"rb");
      logger<<"\n\nopenSourceFile()";
      if(fs)
      {
         logger<<" Opened Source File : "<<mainFileName;
         fseek(fs, 0, SEEK_END);			// goto eof
	      mainFileSize = ftell(fs);		// get offset
   	   fseek(fs,0,SEEK_SET);			// rewind

   	   subFileSize = mainFileSize / numSubFiles;
	      lastSubFileSize = mainFileSize - ((numSubFiles-1)*subFileSize);
         logger<<" ("<<mainFileSize<<" bytes)\n";
         calcCRC32();
         logger<<"Source file CRC : ";
         logger.setf(ios::uppercase);
         logger<<hex<<getFileCRC();
         logger.unsetf(ios::uppercase);
      }
      else
      {
         errorCode = NO_SOURCE_FILE;
       	logger<<" Error Opening File : "<<mainFileName;
      }
   }

   void createSubFiles() // main function in SPLIT - Creates the subfiles
   {
      char buffer[512]; int j,i;
      unsigned long int toRead,totalBytes;
      struct time ts;
      strcpy(process,"Splitted ");
      initSubFileIndexCounter(); // initiate the subfileindex counter. set it to 00
   	for(j=0; j < (numSubFiles-1); j++)
      {
	      openNextSubFile(SPLIT);
         if(errorCode) break;
         toRead = sizeof(buffer);
         totalBytes = 0;
        	while(totalBytes<subFileSize)
	      {
         	if((subFileSize - totalBytes) < toRead)
            	toRead = ((subFileSize - totalBytes));

   	      fread(buffer, toRead, 1, fs);
				fwrite(buffer,toRead, 1, ft);
            totalBytes += toRead;
        	}
        	fclose(ft);
      	logger<<"\n[v]createSubFiles() : Created "<<nextTargetFile;
         logger<<" ("<<dec<<subFileSize<<" bytes)\n";
      }
      // process the last subfile
      openNextSubFile(SPLIT);
      if(!errorCode)
      {
			int numFullBlocks = (lastSubFileSize/512);
         int lastFewBytes  =  lastSubFileSize - (numFullBlocks*512);
			for(i = 0; i <numFullBlocks; i++)
         {
        	     fread(buffer, 512, 1, fs);
      	 	  fwrite(buffer,512, 1, ft);
         }
	      fread(buffer, lastFewBytes, 1, fs);
	      fwrite(buffer,lastFewBytes, 1, ft);
         fclose(ft);
      	logger<<"\n[v]createSubFiles() : Created "<<nextTargetFile;
         logger<<" ("<<lastSubFileSize<<" bytes)\n";
       	fclose(fs);
	   }
      // end processing the last file
   }

   /***********Join*The*SubFiles*To*Form*The*Original*File************/
   // Read the details from the index file
   // Set values to variables
   // if output file doesnt exist, open each subfile and append them to form the real file

   void joinLogDetails(int fileVersion,char *origFileType,int finalSize,unsigned long expFileCRC)
   { // just logs additional information about the join from data from the indx file
   	char tmp[256];
      strcpy(tmp,mainFileName);
      strcat(tmp,".");
      strcat(tmp,origFileType);
      strcpy(mainFileName,tmp);
      mainFileSize = finalSize;

    	logger<<"\nreadIndexHeader() Index File Version : "<<fileVersion<<endl;
      logger<<"                  Original File Extn : "<<origFileType<<endl;
      logger<<"                  Output File name   : "<<mainFileName<<endl;
      logger<<"                  Final File Size    : "<<finalSize<<endl;
      logger.setf(ios::uppercase);
      logger<<"                  Final File CRC-32  : "<<hex<<expFileCRC<<endl;
      logger.unsetf(ios::uppercase);
   }

   void joinSubFiles()
   { // Joins the SubFiles
      int i,j; unsigned char buffer[512];
      unsigned int outputFileSize,bytesToRead,numBlocksToRead,inFileSize;
      bool fileDoesNotExist = true;

      strcpy(process,"Joined ");
  	   subFileSize = mainFileSize / numSubFiles;

   	fs = fopen(mainFileName,"rb"); // if given file exists, it will be corrupted. so make sure that this file is not present
      if(fs)								 // - file exists
      {
      	fileDoesNotExist = false;
         fclose(fs);
         logger<<"The outfile already exists";
      }
      strcpy(outputFileBackup,mainFileName);
    	fs = fopen(mainFileName,"ab"); // open output file in append mode
      if(fs && fileDoesNotExist)
      {
            initSubFileIndexCounter();
            for(j=0;j<numSubFiles;j++)
            {
	            openNextSubFile(JOIN);
			      bytesToRead = sizeof(buffer);
               if(j==numSubFiles-1) inFileSize = mainFileSize - ((numSubFiles-1)*subFileSize); // compute size for last subfile
               else						inFileSize = subFileSize;
			      numBlocksToRead = (inFileSize/512)+1;

               while(numBlocksToRead>0)
		         {
                  if(numBlocksToRead==1) bytesToRead = inFileSize % 512;
                  fread(buffer ,bytesToRead ,1 ,ft);
      			 	fwrite(buffer,bytesToRead ,1 ,fs);
                  numBlocksToRead--;
               }
	            fclose(ft);
               logger<<"\n[v] Read from the subfile : "<<nextTargetFile<<endl;
		       }
	       	fclose(fs);
            strcpy(mainFileName,outputFileBackup);
      }
      else{cout<<"Error Creating The output File, a file by that name may already exist..."; getch();}
   }
   void verifyCRC(unsigned long origCRC)
   {
		calcCRC32();
      logger<<"\nmainClass :: verfifyCRC() ";
      logger.setf(ios::uppercase);
      if(origCRC == getFileCRC())
  			logger<<" FileCRC's match ("<<hex<<origCRC<<")";
      else
      {
      	logger<<" FileCRCFail! The file may be corrupt. Current CRC : "<<hex<<getFileCRC();
         logger<<" ,Expecting : "<<hex<<origCRC;
      }
      logger.unsetf(ios::uppercase);
   }
};

/*******************************/
/* Main Function :: EntryPoint */
/*******************************/

void main(int argc,char *argv[])
{
	FILE *idx;
   theIndex data;						// data of index file
	char *mode;
   int subFileCount=1;				// no divide by 0
   char indexFile[256],mainFile[256],subFile[256],mainType[8];

	if(argc==7)
   {
      mode = argv[1];				//	S or J
      strcpy(indexFile,argv[2]); // the path to the index file
      strcpy(mainFile,argv[3]);	// if S source file; if J output file
      strcpy(subFile,argv[4]);	// subFileNameTemplate
      strcpy(mainType,argv[5]);  // file extension of the original file

		// split the main file
      if((strcmp(mode,"-S")==0))
      {
      	subFileCount = parseInt(argv[6]); 				// get int value from the string

         splitter s(mainFile,subFile,subFileCount);   // create an object
        	s.openSourceFile();
			s.createSubFiles();

         // now write index file
         idx = fopen(indexFile,"wt");
         if(idx)
         {
         	strcpy(data.identifier,"IDX");
            strcpy(data.mainFileExt,mainType);
            data.version      = PROG_VERSION;
           	data.mainFileSize = s.getMainFileSize();
				data.numSubFiles  = subFileCount;
            data.fileCRC		= s.getFileCRC();
          	fwrite(&data,sizeof(data),1,idx);	// write data
            fclose(idx);                        // it is fair to close the file!
         }else{cout<<"Couldn't create the index file.."; getch();}
      }

		// Join up the subfiles
      if(strcmp(mode,"-J")==0)
      {
      	idx = fopen(indexFile,"rt"); // read data from the index file
         if(idx!=NULL)
         { // block alias - readIndexHeader()
		    int res;
         	res = fread(&data,sizeof(data),1,idx);
            fclose(idx);
			if(res == 0) break;
            // process
	         splitter j(mainFile,subFile,data.numSubFiles);   // create an object
            j.joinLogDetails(data.version,data.mainFileExt,data.mainFileSize,data.fileCRC);
            j.joinSubFiles();
            j.verifyCRC(data.fileCRC);
         }else{cout<<"Error Opening the index file..."; getch();}
      }
   }
}
