//----------------------------------------------------------------------------

#include "base.h"
#include "drivers/xbee_serial.h"
#include "zigbee/xbee_struct.h"
#include "zigbee/xbee_client.h"
#include "sensor/sensor_client.h"
#include "sensor/sensor_struct.h"



void *
routeur(void)
{
struct xbee_rawframe * frame  ;

   xbee_open(PORT1);

   sleep(1);
        uint8_t data_rt[3];
      uint8_t i;
   
      printf("%x\n", i);
      data_rt[0] = 0;
      data_rt[1] = 0;
      data_rt[2] = 0xFE;
      xbee_send_data(data_rt, 3, 0, 0);
    

  while(1){
    #ifdef __DEBUG__
    //printf("Lecture de %s ... (Poll)... \n", (char *)PORT0);
    #endif


    frame = xbee_read();

    int i = 0;

   

   //if(frame->header.api == 0x90){


       #ifdef __DEBUG__
        xbee_print_frame((uint8_t *) frame);
       
        #endif
      
      
        rawdata_sensor(&(frame->rawdata[11]),1,(uint16_t)frame->rawdata[9]<<8&0xFF00 | (uint16_t)frame->rawdata[10]&0xFF);

          
     //}
    
    }


    free(frame);
    
    
         /* Closing file descriptor */
    xbee_close();
 
  
}


void *
coordinateur(void )
 {
  
  struct xbee_rawframe * frame  ;

   xbee_open(PORT0);

  while(1){
    #ifdef __DEBUG__
    //printf("Lecture de %s ... (Poll)... \n", (char *)PORT0);
    #endif


    frame = xbee_read();

    int i = 0;

  

    if(frame->header.api == 0x90){


       #ifdef __DEBUG__
        xbee_print_frame((uint8_t *) frame);
       
        #endif
      
      
        rawdata_sensor(&(frame->rawdata[11]),1,(uint16_t)frame->rawdata[9]<<8&0xFF00 | (uint16_t)frame->rawdata[10]&0xFF);

          
     }
    
    }


    free(frame);
    
    
         /* Closing file descriptor */
    xbee_close();
 
}


int
main(int argc,
     char **argv)
{
  /*if(argc != 2){
    return -1;
  }*/
  char * port;
// Init : 
    sensor_init_client();

  //if(!strcmp(argv[1], "coordinateur")){
    
     #ifdef __DEBUG__
    printf(" Ouverture de Xbee PORT0...\n");
    #endif
    coordinateur();



/*
  }else if(!strcmp(argv[1], "routeur")){
      port = (char *)PORT1;

    
    
     #ifdef __DEBUG__
    printf(" Ouverture de Xbee PORT1...\n");
    #endif
    //xbee_open(port);


  routeur();


  }else{
       #ifdef __DEBUG__
    printf(" Erreur d'arguments...\n");
    #endif
    
    return -1;
  }
*/
    return 0;
}

//----------------------------------------------------------------------------
