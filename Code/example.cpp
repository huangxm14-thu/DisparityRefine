#include "qx_basic.h"
#include "qx_ppm.h"
#include "qx_nonlocal_cost_aggregation.h"

#include "windows.h"

double g_nSigma = 0.04;
double g_nSigmaHybrid = 0.0016;
double g_colorWeight = 0.05;





void refine(char*filename_disparity_map,char*filename_left_image,char*filename_disparity_map_L, char*filename_disparity_map_R, int nDisparityScale)
{
	int max_disparity = 1;	//not used here
	double sigma=g_nSigma;//QX_DEF_SIGMA;
	unsigned char***left,***right,**disparity, **disparityL, **disparityR; 
	int h,w;

	qx_image_size(filename_left_image,h,w);//obtain image size

	left=qx_allocu_3(h,w,3);//allocate memory
	right=qx_allocu_3(h,w,3);
	disparity=qx_allocu(h,w);
	disparityL=qx_allocu(h,w);
	disparityR=qx_allocu(h,w);

	qx_nonlocal_cost_aggregation m_nlca;//non-local cost aggregation class
	m_nlca.init(h,w,max_disparity,sigma);//initialization

	qx_loadimage(filename_left_image,left[0][0],h,w);//load left image

	qx_loadimage(filename_disparity_map_L,disparityL[0],h,w);//load left image
	qx_loadimage(filename_disparity_map_R,disparityR[0],h,w);//load right image

	for(int y = 0; y < h; y ++)
		for(int x = 0; x < w; x ++)
		{
			disparityL[y][x] /= nDisparityScale;
			disparityR[y][x] /= nDisparityScale;
		}

	qx_timer timer;//
	timer.start();

	int t1 = GetTickCount();
	m_nlca.matching_cost(left,right);//compute matching cost
	int t2 = GetTickCount();
	printf("matching_cost time: %d ms\n", (t2 - t1));
	
	timer.start();
	m_nlca.refineFast(disparity,disparityL,disparityR);//refine disparity

	//http://vision.middlebury.edu/stereo/data/scenes2003/
	//Disparities are encoded using a scale factor 4 for gray levels 1 .. 255, while gray level 0 means "unknown disparity". Therefore, the encoded disparity range is 0.25 .. 63.75 pixels.
	for(int y=0;y<h;y++) for(int x=0;x<w;x++) disparity[y][x]*=nDisparityScale;//rescale the disparity map for visualization
	qx_saveimage(filename_disparity_map,disparity[0],h,w,1);//write the obtained disparity map to the harddrive


	qx_freeu_3(left); left=NULL;//free memory
	qx_freeu_3(right); right=NULL;
	qx_freeu(disparity); disparity=NULL;
	qx_freeu(disparityL); disparityL=NULL;
	qx_freeu(disparityR); disparityR=NULL;
}

int main(int argc,char*argv[])
{
	if(argc != 6)
	{
		printf("Usage: demo.exe disparity_output image_left disparity_left disparity_right disparity_scale\n");
		getchar();
		return -1;
	}

	printf("disparity_output: %s\n", argv[1]);
	printf("image_left: %s\n", argv[2]);
	printf("disparity_left: %s\n",argv[3]);
	printf("disparity_right: %s\n",argv[4]);
	printf("disparity_scale: %s\n", argv[5]);

	int t1 = GetTickCount();
	refine(argv[1], argv[2],argv[3],argv[4], atoi(argv[5]));
	int t2 = GetTickCount();
	printf("sal time: %d ms\n", (t2 - t1));

	printf("refine finished\n\n");



	return 0;
}

