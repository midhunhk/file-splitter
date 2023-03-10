/*	FileSplitter_in_c.c - splits a given source file into subparts
 *	Author:Midhun Harikumar
 *      (c)2007 Centrum inc Software Solutions
 *
 *	$ Version 1.0 | 3 - June - 2007
 */

# include <stdio>
# include <conio>
# include <string>

void main(int argc,char *argv[])
{
   FILE *fs,*ft,*fl;
   char src[512],*targetFileName = "__conv_file_a.split",ch;
   unsigned int mainFileSize,subFileSize,i,fileSplitCount = 2;
   unsigned int j,numFiles=0; // counter to keep track of the no of subfiles generated

   if(argc!=2)
   {
	printf("File Splitter - The File Splitting Utility \n$_Version 0.90\n\n");
	printf("SourceFileName...");
   	gets(src);
	printf("FileSplitCount...");
   	scanf("%d",&fileSplitCount);
   }
   else
   {
    	strcpy(src,argv[0]);
      fileSplitCount = (int)argv[1];
   }

	fl = fopen("FileSplitter.log","a");
   fs = fopen(src,"rb");

   fprintf(fl,"\nStarting New Instance\n\n");

   if(fs)
   {
      printf("\nProcessing...\n");
      fseek(fs, 0, SEEK_END);			// goto eof
      mainFileSize = ftell(fs);		// get offset
      fseek(fs,0,SEEK_SET);			// rewind

      subFileSize = mainFileSize / fileSplitCount;

      fprintf(fl,"Opened the Source file '%s'...\nFile Size : %d",src,mainFileSize);
      fprintf(fl,"\nSplitting into %d parts.",fileSplitCount);

      for(j = 0; j < fileSplitCount-1; j++)
      {
	      i = 0;

	      ft = fopen(targetFileName,"wb");
        if(ft)
      	{
         	numFiles++;
      		while(i++<subFileSize)
	        {
   	             fread(&ch,sizeof(ch),1,fs);
      		     fwrite(&ch,sizeof(ch),1,ft);
         	}
         	fclose(ft);
         	fprintf(fl,"\n [v] Created subfile \"%s\" (%d bytes)",targetFileName,subFileSize);            
         }
         else
         { fprintf(fl,"\n [!] Error creating subfile '%s'",targetFileName);}
         
	 targetFileName[12]++; // incrementing the subfile name
      }

      // To prevent loss of infn, the last file will have all the remaining data
      ft = fopen(targetFileName,"wb");
      if(ft)
      {
         numFiles++;
         unsigned int lastFileSize = mainFileSize - ((fileSplitCount-1)*subFileSize);
         fprintf(fl,"\n [v] Created subfile \"%s\" (%d bytes)",targetFileName,lastFileSize);
         i = 1;
         while(i)
         {
   	     i = fread(&ch,sizeof(ch),1,fs);
      	     fwrite(&ch,sizeof(ch),1,ft);
         }
         fclose(ft);
      }
      // End of creating the last file
      fprintf(fl,"\n\nCreated %d subfiles.(%d needed).\n",numFiles,fileSplitCount);
   }else{ fprintf(fl,"Error Opening the Source file '%s'...",src);}

   fclose(fs);
   fclose(fl);
}
