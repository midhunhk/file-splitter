/* FileCRC32.cpp
 *
 * Implementation of CRC-32 on Files in C++, the memberfns
 *
 * CalculateFileCRC()
 * BuildCRCTable()
 * CalculateBufferCRC()
 */

# include "FileCRC32.h"

const unsigned long CRC32_POLYNOMIAL = 0xEDB88320L;
unsigned long CRCTable[ 256 ];

void FileCRC32 :: BuildCRCTable()
{
  	 int i;
    int j;
  	 unsigned long crc;

    for ( i = 0; i <= 255 ; i++ ) {
  	     crc = i;
     	  for ( j = 8 ; j > 0; j-- ) {
        	   if ( crc & 1 )
          	    crc = ( crc >> 1 ) ^ CRC32_POLYNOMIAL;
            else
  	             crc >>= 1;
     	  }
        CRCTable[ i ] = crc;
  	 }
}

unsigned long FileCRC32 :: CalculateFileCRC()
{
  	 unsigned long crc;
    int count;
  	 unsigned char buffer[ 512 ];
    int i;
  	 crc = 0xFFFFFFFFL;
    i = 0;
  	 for ( ; ; )
    {
        count = fread( buffer, 1, 512, file );
        if ( ( i++ % 32 ) == 0 )
					;
     	  if ( count == 0 )
        	   break;
        crc = CalculateBufferCRC( count, crc, buffer );
  	 }
    return( crc ^= 0xFFFFFFFFL );
}
unsigned long FileCRC32 :: CalculateBufferCRC(unsigned int count,unsigned long crc,void* buffer )
{
  	 unsigned char *p;
    unsigned long temp1;
  	 unsigned long temp2;
    p = (unsigned char*) buffer;
  	 while ( count-- != 0 ) {
     	  temp1 = ( crc >> 8 ) & 0x00FFFFFFL;
        temp2 = CRCTable[ ( (int) crc ^ *p++ ) & 0xff ];
  	     crc = temp1 ^ temp2;
    }
  	 return( crc );
}

