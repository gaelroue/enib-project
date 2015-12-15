/* File: fb.c */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>             /* getopt_long() */
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

typedef void (*ProcessFunc)(uint8_t *, int, int);

/* Device Name like /dev/fb */
#define FBNAME	"/dev/fb0"

/* fixed screen information */
struct fb_fix_screeninfo fix_info;

/* configurable screen info */
struct fb_var_screeninfo var_info;

/* The frame buffer memory pointer */
uint8_t *framebuffer;

uint8_t otsu[640*480];
uint8_t img[640][480];	
int img_labelled[640*480];



#define CALL_LabelComponent(x,y,returnLabel) { STACK[SP] = x; STACK[SP+1] = y; STACK[SP+2] = returnLabel; SP += 3; goto START; }
#define RETURN { SP -= 3;                \
                 switch (STACK[SP+2])    \
                 {                       \
                 case 1 : goto RETURN1;  \
                 case 2 : goto RETURN2;  \
                 case 3 : goto RETURN3;  \
                 case 4 : goto RETURN4;  \
                 case 5 : goto RETURN5;  \
                 case 6 : goto RETURN6;  \
                 case 7 : goto RETURN7;  \
                 case 8 : goto RETURN8;  \
                 default: return;        \
                 }                       \
               }
#define X (STACK[SP-3])
#define Y (STACK[SP-2])


uint8_t image_test[640*480];

/*
 * Macro to pack the pixel based on the rgb bit offset.
 * We compute each color value based on bit length
 * and shift it to its corresponding offset in the pixel.
 * each color component is 8 bits long
 *
 * For example: Considering a RGB565, the formula will
 * expand as:-
 * Red len=5, off=11 : Green len=6, off=6 : Blue len=5, off=0
 * pixel_value = ((red >> (8 - 5) << 11)|
 *       ((green >> (8 - 6) << 6) |
 *      ((blue >> (8 - 5) << 0)
 */
#define RGB(r,g,b) ( \
        (((r) >> (8-var_info.red.length)) << var_info.red.offset) | \
        (((g) >> (8-var_info.green.length)) << var_info.green.offset) | \
        (((b) >> (8-var_info.blue.length)) << var_info.blue.offset) \
)

#define SET_PIXEL(x,y,color) (((short*)framebuffer)[(x)+(y)*fix_info.line_length/2]=(color))

/* function to a filled rectangle at position (x,y), width w and height h */
void rect_fill(int x,int y, int w, int h, unsigned short color)
{
	int i, j;
	for (i=0;i<w;i++) {
		for (j=0;j<h;j++) {
			SET_PIXEL(x+i,y+j,color);
		}
	}
}

/***************************************************************************
 * decoding stuff
 ***************************************************************************/
#define bounds(m,M,x) ((x)>M ? M : (x)<(m) ? m : (x))
static void process_image_yuv422 (uint8_t * videoFrame, int width, int height)
{
	/* A COMPLETER */
	int k = 1;
	int i, j;
	for(i=0; i<width; i++){
		for(j=0; j<height; j=j+1){
				k = (j*width + i)*2+1;
				SET_PIXEL(i,j,RGB(videoFrame[k],videoFrame[k],videoFrame[k]));
		}
	}
	
}


/*              x
		0 1 2 3 4 5 6 7
		
	0	G R G R G R G R
	1	B G B G B G B G
y	2	G R G R G R G R
	3	B G B G B G B G
	4	G R G R G R G R
	5	B G B G B G B G

 */
 
 
 static void detection_contours(uint8_t * videoFrame, int width, int height){

	int k = 0;
	/*for(int i=0; i<width; i++){
		for(int j=0; j<height; j=j+1){
				k = (j*width + i)*2+1;
				SET_PIXEL(i,j,RGB(videoFrame[k],videoFrame[k],videoFrame[k]));
				//SET_PIXEL(i+640,j,RGB(videoFrame[k],videoFrame[k],videoFrame[k]));
		}
	}*/

	uint8_t vert[width][height];
	uint8_t V[width][height];
	
	uint8_t hori[width][height];
	uint8_t H[width][height];
	
	uint8_t contours[width*height];

	int somme=0;
	int i,j;
	for(i=0; i<width; i++){
		for(j=0; j<height; j++){
			//k = (j*width + i)*2+1;
			k = i + width*j;
			vert[i][j] = videoFrame[k];
			hori[i][j] = videoFrame[k];
		}
	}
	
	int a=0;
	for(i=1; i<width-1; i++){
		for(j=1; j<height-1; j++){
	
			// Masques :
			V[i][j] = -1*vert[i-1][j-1] - vert[i-1][j] - vert[i-1][j+1] + vert[i+1][j-1] + vert[i+1][j] + vert[i+1][j+1];
			H[i][j] = -1*hori[i-1][j-1] - hori[i][j-1] - hori[i+1][j-1] + hori[i-1][j+1] + hori[i][j+1] + hori[i+1][j+1];
			
			contours[a] = V[i][j]*V[i][j] + H[i][j]*H[i][j]; 
			somme = somme + contours[a];
			
			a++;
		}
	}
	
	int moyenne = (somme/(width*height))*5;
	//printf("somme : %d\n",somme);
	
	int b=0;
	for(i=1; i<width-1; i++){
		for(j=1; j<height-1; j++){
			
			if(contours[b] < moyenne){
				contours[b]=0;
			}
			else {
				contours[b]=255;
			}
			
			SET_PIXEL(i+640,j,RGB(contours[b],contours[b],contours[b]));
			b++;
		}
	}
 }

float sig(int k, int mu, int taille_x, int taille_y, int * histo){
  double tmp, w, tmp2;
  double N = (double)taille_x*taille_y;
  int i;
  
  w = 0;
  tmp = 0;
  for (i=0 ; i<=k; i++)
    {
      w += histo[i];
      tmp += (i+1)*histo[i];   /* on calcul mu(k)*/
    }
  w /= N;
  tmp /= N;
  tmp2 = (mu*w-tmp)*(mu*w-tmp)*(w*(1-w));

  return (float)tmp2;
}




// FONCTION DE BINARISATION
static void binarisation(uint8_t * videoFrame, int width, int height, uint8_t * otsu){
	int seuil=0;
	int k = 1;

	uint8_t valeur;
	
	// Tableau histo qui est composé des 255 valeurs de niveaux de gris
	// Initialisation des valeurs à 0 :
	int histo[256];
	float sigma[256];
	int i,j;
	for(i=0; i<=255; i++){
		histo[i] = 0;
	}
	
	// Parcours de l'image et comptage des niveaux de gris
	for(i=0; i<width; i++){
		for(j=0; j<height; j=j+1){
			k = (j*width + i)*2+1;
			img[i][j] = videoFrame[k];
			
			valeur = img[i][j];
			histo[valeur]++;

			//SET_PIXEL(i,j,RGB(videoFrame[k],videoFrame[k],videoFrame[k]));
			k++;
		}
	}
	
	int nb_pixels = width*height;
	

	// Calcul de la moyenne des classes :
	float moyenne = 0;
	for (i=0;i<256;i++){
		moyenne = moyenne + (i+1)*histo[i];
	}
				moyenne = moyenne/nb_pixels;
	// calcul des sigma  
	for (i=1; i<255; i++){
		sigma[i] = sig(i,moyenne,width,height,histo);
	}
	
	// recherche du max 
	i = 0;
	for (j=1; j<255;j++){
	  if (i+1 < sigma[j])   // on a un nouveau maximum 
	  { 
		seuil=j; 
		i=sigma[j];  //on met le seuil dans k 
	  }
	}
	//printf("seuil = %d\n", seuil);

	// Binarisation :
	
	int b=0;
	for (i=0; i<width; i++){
		for (j=0; j<height; j++){
			if (img[i][j] >= seuil){
				img[i][j] = 0;
			}
			else{
				img[i][j] = 255;
			}
			
			otsu[b] = img[i][j];
			
			//SET_PIXEL(i,j+480,RGB(otsu[b],otsu[b],otsu[b]));
			b++;
			
		}
	}
}


/*
static void display(int * img, int width, int height){
	int b=0;
	for (int i=0; i<width; i++){
		for (int j=0; j<height; j++){
			SET_PIXEL(i+640,j+480,RGB(img[b],img[b],img[b]));
			b++;			
		}
	}
}*/


// FONCTION DE LABELISATION DE COMPOSANT
void LabelComponent(unsigned short* STACK, unsigned short width, unsigned short height, uint8_t* input, int* output, int labelNo, unsigned short x, unsigned short y)
{
  STACK[0] = x;
  STACK[1] = y;
  STACK[2] = 0;  /* return - component is labelled */
  int SP   = 3;
  int index;

START: /* Recursive routine starts here */

  index = Y + height*X;

  if (input[index] == 0) RETURN;   /* This pixel is not part of a component */
  if (output[index] != 0) RETURN;   /* This pixel has already been labelled  */
  output[index] = labelNo;

    
  if (X > 0) CALL_LabelComponent(X-1, Y, 1);   /* left  pixel */
RETURN1:

  if (X < width-1) CALL_LabelComponent(X+1, Y, 2);   /* rigth pixel */
RETURN2:

  if (Y > 0) CALL_LabelComponent(X, Y-1, 3);   /* upper pixel */
RETURN3:

  if (Y < height) CALL_LabelComponent(X, Y+1, 4);   /* lower pixel */
RETURN4:

  if (X > 0 && Y > 0) CALL_LabelComponent(X-1, Y-1, 5);  
RETURN5:

  if (X > 0 && Y < height) CALL_LabelComponent(X-1, Y+1, 6);   
RETURN6:

  if (X < width && Y > 0) CALL_LabelComponent(X+1, Y-1, 7);   
RETURN7:

  if (X < width && Y < height) CALL_LabelComponent(X+1, Y+1, 8);   
RETURN8:

  RETURN;
}

// FONCTION DETECTION DE CERCLE
void cercle(uint8_t *  videoFrame, uint8_t * img_otsu, int width, int height){
			
	double compteurA = 0;
	double compteurP = 0;
	//int size = width*height;
	int i,j;
	for(i = 0; i < width*height; i++){
		image_test[i] = 0;		
	}	


	for(i = 0; i < width; i++){
		for( j = 0; j < height; j++){
			int index = i + width*j;
			// Calcul de l'aire
			// Comptage du nombre de pixel dans l'image :
			if(img_otsu[index] == 255){compteurA++;}
			// Calcul du périmètre
			// Comptage du nb de pixels ayant des voisins noirs (connexite 4) :
			if(img_otsu[index] == 255 
			   &&
			   (img_otsu[index - 1] == 0 ||
			   img_otsu[index + 1] == 0 ||
			   img_otsu[index - height] == 0 ||
			   img_otsu[index + height] == 0)){
			   
			   image_test[index] = img_otsu[index];
								
				compteurP++;
			}
		}
	}	
	
	
	// Calcul du rapport :	
	double cercle = 0;
	cercle = compteurA/(compteurP*compteurP)*4*3.14;
	
	//printf("aire = %f\n",compteurA);
	//printf("perimetre = %f\n",compteurP);
	//printf("rapport : %f\n", cercle);
	
	// Affichage du contour des cercles en rouge :
	//uint8_t image_result[width*height];
	int k =0;
	int b=0;
	if(cercle < 1.3 && cercle > 1.1){
		for(i = 0; i < width; i++){
			for(j = 0; j < height; j++){
				k = (j*width + i)*2+1;
				//image_result[b] = videoFrame[k];
				
				if(image_test[b] == 255){
					SET_PIXEL(i,j,RGB(0, 0, 255));
					if(i>0 && j>0 && i<width && j<height){
						SET_PIXEL(i+1,j,RGB(0, 0, 255));
						SET_PIXEL(i,j+1,RGB(0, 0, 255));
						SET_PIXEL(i-1,j,RGB(0, 0, 255));
						SET_PIXEL(i,j-1,RGB(0, 0, 255));
					}
				}
				k++;
				b++;
			}
		}
	}
}

// FONCTION DE LABELISATION DE L'IMAGE
void LabelImage(unsigned short width, unsigned short height, uint8_t* input, int* img_labelled, uint8_t* videoFrame){
	int i;
	for (int i=0; i<width*height; i++){
		img_labelled[i] = 0;
	}
	
	unsigned short* STACK = (unsigned short*) malloc(3*sizeof(unsigned short)*(width*height + 1));

	int c =0;
	int labelNo = 0;
	int index   = -1;
	int b = 0;
	unsigned short x,y;
	for (x = 0; x < width; x++){
		for (y = 0; y < height; y++){
			index++;
			if (input[index] == 0) continue;   /* This pixel is not part of a component */
			if (img_labelled[index] != 0) continue;   /* This pixel has already been labelled  */
			
			// New component found :
			labelNo = labelNo + 1;
			LabelComponent(STACK, width, height, input, img_labelled, labelNo, x, y);
		}
	}
	
	printf("nombre d'objets : %d\n", labelNo);
	
	/*
		for (int i=0; i<width; i++){
		for (int j=0; j<height; j++){		
			//Image Labellisé
			SET_PIXEL(i+640,j+480,RGB(img_labelled[b],img_labelled[b],img_labelled[b]));
			
			b++;
		}
	}*/
	
	
	uint8_t img_tmp[width*height];
	int k,j;
	if(labelNo < 7){
		for(k = 1; k < labelNo + 1; k++){
			for(i = 0; i < width; i++){
				for(j = 0; j < height; j++){
					if(img_labelled[i+width*j] == k){
						img_tmp[i+width*j] = 255;
					}
					else{
						img_tmp[i+width*j] = 0;
					}			
				}
			}
			cercle(videoFrame, img_tmp, width, height);
		}
	}
	free(STACK);
}




static void process_image_raw12(uint8_t * videoFrame, int width, int height){
	int    x,y;

	
	for (y = 1; y < height-1; y++) {
		for (x = 1; x < width-1; x++) {

			/* A COMPLETER */

			//SET_PIXEL(x,y,RGB(R,G,B));
		}
	}
}


/***************************************************************************
 * main
 ***************************************************************************/
typedef enum 
{      
	PIX_FMT_YUV420P,
	PIX_FMT_RGB565,
	PIX_FMT_RGB32,
	PIX_FMT_YUV422,
	PIX_FMT_RAW12
} pix_fmt;

//needed to parse command line arguments with getopt_long
static const char short_options [] = "p:w:h";

//also needed to parse command line arguments with getopt_long
static const struct option
long_options [] = 
{
	{ "pixel-format",required_argument,      NULL,           'p' },
	{ "window-size", required_argument,      NULL,           'w' },
	{ "help",        no_argument,            NULL,           'h' },
	{ 0, 0, 0, 0 }
};

static void usage (FILE *fp, int argc, char **argv)
{
	fprintf (fp,
				"Usage: %s [options]\n\n"
				"Options:\n"
				"-p | --pixel-format   number          Pixel Format \n"
				"                                      (0 = YUV420)\n"
				"                                      (1 = RGB565 )\n"                 
				"                                      (2 = RGB32  )\n"  
				"-w | --window-size    <640*480|       Video size\n"
				"                       320*240>\n"
				"-h | --help                           Print this message\n"
				"\n",
				argv[0]);
}


int size;
int					fbd;			/* Frame buffer descriptor */
int                 width                = 640;
int                 height               = 480;
int                 index_t;
int                 c;
pix_fmt             pixel_format = PIX_FMT_YUV422;
ProcessFunc			proc_func=process_image_yuv422;
int					quit=0;

int init_camera(int argc, char *argv[])
{

	//if(*argv[4]==5)printf("toto\n");
	while(1) {
		c = getopt_long (argc, argv, short_options, long_options, &index_t);

		if (-1 == c)
			break; //no more arguments

		switch (c) {
			case 0: // getopt_long() flag
				break;

			case 'p':
				pixel_format = (pix_fmt) atoi(optarg);
       			switch(pixel_format) {
				case PIX_FMT_YUV422:
					proc_func=process_image_yuv422;
					break;
				case PIX_FMT_RAW12:
					proc_func=process_image_raw12;
					break;
				default:
					fprintf(stderr, "\n\nError: Pixel format NOT supported\n\n");
					exit (EXIT_FAILURE);
				}
				break;

			case 'w':
				sscanf(optarg,"%d*%d",&width,&height);
				fprintf(stderr,"window size %d*%d\n",width,height);
				break;

			case 'h':
				usage (stdout, argc, argv);
				exit (EXIT_SUCCESS);

			default:
				usage (stderr, argc, argv);
				exit (EXIT_FAILURE);
		}
	}
	
	
    /* Open the framebuffer device in read write */
    fbd = open(FBNAME, O_RDWR);
    if (fbd < 0) {
        fprintf(stderr, "Unable to open %s.\n", FBNAME);
        return EXIT_FAILURE;
    }

    /* Do Ioctl. Retrieve fixed screen info. */
    if (ioctl(fbd, FBIOGET_FSCREENINFO, &fix_info) < 0) {
        fprintf(stderr, "get fixed screen info failed: %s\n",
        	strerror(errno));
        close(fbd);
        return EXIT_FAILURE;
    }

    /* Do Ioctl. Get the variable screen info. */
	if (ioctl(fbd, FBIOGET_VSCREENINFO, &var_info) < 0) {
        fprintf(stderr, "Unable to retrieve variable screen info: %s\n",
	    	strerror(errno));
        close(fbd);
        return EXIT_FAILURE;
    }

    /* Print some screen info currently available */
    fprintf(stderr, "Screen resolution: (%dx%d)\n",var_info.xres,var_info.yres);
	fprintf(stderr, "x offset, y offset : %d, %d\n",var_info.xoffset,var_info.yoffset);
    fprintf(stderr, "Line width in bytes %d\n", fix_info.line_length);
    fprintf(stderr, "bits per pixel : %d\n", var_info.bits_per_pixel);
    fprintf(stderr, "Red: length %d bits, offset %d\n",var_info.red.length,var_info.red.offset);
	fprintf(stderr, "Green: length %d bits, offset %d\n",var_info.red.length, var_info.green.offset);
    fprintf(stderr, "Blue: length %d bits, offset %d\n",var_info.red.length,var_info.blue.offset);
	/* Calculate the size to mmap */
	size=fix_info.line_length * var_info.yres;

	/* Now mmap the framebuffer. */
	framebuffer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fbd,0);
    if (framebuffer == NULL) {
        fprintf(stderr, "mmap failed:\n");
        close(fbd);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "framebuffer mmap address=%p\n", framebuffer);
    fprintf(stderr, "framebuffer size=%d bytes\n", size);


}

uint8_t * videoFrame;
int camera(void)
{
    
	
	
	
    //rect_fill(50,20,100,50,RGB(0x7E,0x75,0x76));

    
     videoFrame = (uint8_t*) malloc (width*height*2);
	if (!videoFrame) {
		fprintf(stderr, "could not allocate buffer for video frame (%d bytes required)\n", width*height*2);
		exit (EXIT_FAILURE);
	}
		
	
    while (!quit)
    {
		uint8_t *ptr=videoFrame;
		
		
		int size=width*height*2;
		while(size) {
			int ret = read(STDIN_FILENO, ptr , size);
			ptr+=ret;
			size-=ret;
		}
		// TRAME VIDEO DE DEPART
		proc_func(videoFrame, width, height);
		// BINARISATION
		binarisation(videoFrame, width, height,otsu);
		//DETECTION DE CERCLE
		//cercle(videoFrame, otsu, width, height);
		
		// LABELLISATION DE L'IMAGE ET COMPTAGE
		LabelImage(width, height, otsu, img_labelled, videoFrame);
		

    }

    
    /* Release mmap. */
    munmap(framebuffer,0);
    close(fbd);
    return EXIT_SUCCESS;
}

