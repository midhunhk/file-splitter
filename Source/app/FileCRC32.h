/* FileCRC32.h
 *      [8:46 PM 6/19/2007]
 * file that contains the maiin class file for CRC32 class
 */

#if !defined(__STDIO_H) // avoid multiple declarations
# include <stdio>
#endif
//////////////////////////////////////////////////////////////
class FileCRC32
{

private:
	FILE *file;

public:
	FileCRC32()
	   {   }
   FileCRC32(char *filePath)
   {
		BuildCRCTable(); // initialize the CRC table
   	file = fopen(filePath,"rb");
   }

	// Member Functions
	void BuildCRCTable();
   unsigned long CalculateFileCRC();
	unsigned long CalculateBufferCRC(unsigned int count,unsigned long crc,void* buffer );
};
//////////////////////////////////////////////////////////////
