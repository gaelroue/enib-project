//----------------------------------------------------------------------------

#include "base.h"
#include "zigbee/xbee_client.h"
#include "websocket/serveur_socket.h"
#include <math.h>



int main(int argc, char **argv)
{
	int	data= 61198;
	printf("\ndata lumi = %x \n", data);
	int  exposant  = (data&0xF000) >> 12;
	printf("exposant = %x\n", exposant);
	  // printf("exposant= %x \n", exposant);
	int mantisse  = data&0x0F00>>4 | data&0x000F;
		// mantisse |= 
	printf("mantisse = %x\n", mantisse);
	  // printf("mantisse = %x \n", mantisse);
	float  value = pow(2,exposant)* mantisse * 0.045;
	printf("%f\n", value);
	
}

//----------------------------------------------------------------------------
