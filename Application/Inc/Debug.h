#pragma once

#define ITM_Port32(n)	(*((volatile unsigned long *)(0xE0000000+4*n)))

extern "C"
{
	int _write(int file, char*ptr, int len);
}
