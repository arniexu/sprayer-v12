static unsigned char buffer [256][16] = {};
volatile unsigned char size = 0;
	
	
void push(unsigned char b[], unsigned char size) // input 
{
	// abandon  if not 7 bytes
	// copy the receiced bytes into bbuffer
	if (size = 256)
	{
		printf("buffer is full.");
		return;
	}
	else
	{
		memcpy(b, buffer[size + 1], 7);			
		size += 1;
	}
	return;
}

void pop(unsigned char *b, unsigned char size) // output
{
	if (size == 0)
		return;
	else
	{
		// copy 
		size -= 1;
	}
	return;
}
unsigned char size ()
{
	return size;
}