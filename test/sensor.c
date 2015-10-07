#include "base.h"
#include "sensor/sensor_client.h"
#include "sensor/sensor_struct.h"


int main(int argc, char ** argv)
{

	sensor_init_client();
	//uint8_t * data =  "\x00\x03\xFC\x01\x01\x01\x02\x02\x01\x02\x03\x02\x01\x12\xFF"; // essaie inform
	//uint8_t * data =  "\x00\x00\xFE";
	uint8_t * data =  "\x00\x03\x01\x05\x41\x42\x14\x15\x16";
	uint8_t len = 14;
	rawdata_sensor(data,len);


	return 0;
}
