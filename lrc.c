#include "lrc.h"

static unsigned char
LRC(unsigned char *auchMsg, unsigned short usDataLen)
{
	unsigned char uchLRC;
	uchLRC = 0;
	while (usDataLen--)
		uchLRC += *auchMsg++;
	return ((unsigned char) (-((char) uchLRC)));
}
