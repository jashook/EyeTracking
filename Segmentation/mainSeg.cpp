#include "ImageObj.h"
#include "MedianFilter.h"
#include "CLAHE.h"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <fstream>
#include <string>
#include <cstring>
#include <math.h>
#include <algorithm>
#include <ctime>

using namespace std;

//hough detection (passed a smoothed image - sets the values of the center points)
void houghTrans(Image* mag,double** phase,int** alterations,int* center,int filterSize){
	int rMin = 25;
	int rMax = 130;
	int cols = mag->getNumColumns();
	int rows = mag->getNumRows();
	int rowMax = rows-filterSize;
	int colMax = cols-filterSize;
	int maxAlterations = 0;
	
	double cosPh,sinPh;
	int ccPlusB,crMinusA;

	//for each point
	for(int cr=filterSize;cr<rowMax;cr++){
		crMinusA = cr;
		for(int cc=filterSize;cc<colMax;cc++){
			if(mag->getPixelElement(cr,cc)<255)
				continue;
			
			ccPlusB = cc;
			cosPh = cos(phase[cr][cc]);
			sinPh = sin(phase[cr][cc]);
	//		e=mag->getPixelElement(cr,cc);			
			//sweep the radius values
			for(int r=rMin;r<rMax;r++){	
	//			double crPlusA = (double)cr + a;
				ccPlusB += r*sinPh;
				crMinusA -= r*cosPh;
	//			double ccMinusB = (double)cc - b;
				
				//for all quadrents if vals exist
				if(crMinusA<filterSize || ccPlusB<filterSize || crMinusA>rowMax || ccPlusB>colMax){
					continue;
				}
				else{
					alterations[crMinusA][ccPlusB]++;
					
					if(alterations[crMinusA][ccPlusB]>maxAlterations){
						maxAlterations = alterations[crMinusA][ccPlusB];
						center[0]=crMinusA;
						center[1]=ccPlusB;
					}	
				}
				if(maxAlterations>200)
					break;
/*				//1st quadrent
				if(currentPhase<0 && currentPhase>-3.14/2){
					//if values exist
					if(crMinusA<0 || ccPlusB<0 || crMinusA>479|| ccPlusB>639)
						continue;
					else{
					//if the phase is pos?
					hough->setPixelElement(crMinusA,ccPlusB,e+hough->getPixelElement(crMinusA,ccPlusB));
					}
				}	
				//2nd quadrent
				else if(currentPhase>3.14/2 && currentPhase<3.14){
					//if values exist
					if(crMinusA<0 || ccPlusB<0 || crMinusA>479|| ccPlusB>639)
						continue;
					else{
					//if the phase is pos?
					hough->setPixelElement(crMinusA,ccPlusB,e+hough->getPixelElement(crMinusA,ccPlusB));
					}
				}
				//3rd qudrent
				else if(currentPhase>0 && currentPhase<3.14/2){
					//if values exist
					if(crPlusA>479 || ccMinusB<0 || crPlusA<0|| ccMinusB>639)
						continue;
					else{
					//if the phase is pos?
					hough->setPixelElement(crPlusA,ccMinusB,e+hough->getPixelElement(crPlusA,ccMinusB));
					}
				}
				else{
					if(crPlusA>479 || ccMinusB<0 || crPlusA<0|| ccMinusB>639)
						continue;
					else{
						//if the phase is neg?
						hough->setPixelElement(crPlusA,ccMinusB,e+hough->getPixelElement(crPlusA,ccMinusB));
					}
				}*/
				ccPlusB = cc;
				crMinusA = cr;
			}
		}
	}
}

//preforms a threshold on the magnitude values
int magThresh(double x){
	int thresh = 80;
	if(x>thresh)
		return 255;
	else
		return 0;
}

//preforms a threshold which scales outward from the centerpoint
int magVarThresh(double x,double dist,int pupilSize){
	double thresh;
	int startThresh = 15;
	int maxThresh = 45;
	
	if(dist<pupilSize+20)
		thresh = 256;
	else if(dist<140+pupilSize)
		thresh = exp(dist-(125+pupilSize))+startThresh;
		//thresh = exp(dist-140) + 20;
		//thresh = exp(dist-150)+startThresh+(threshAt150-startThresh)*dist/150;//18 at 0 dist, 20 at 150
		//thresh = exp(.1*dist - 11.3) + 18;//threshold is 18 at 0 dist, 22 at 130 (graph it in wolframAlpha)
	else
		thresh = 256;

	//check value against calculated thresh
	if(x>thresh)
		return 255;
	else
		return 0;
}


//scaling function
double scale(double x){
	double sum = 0;
	double prod = 1;
	
	return (x+sum)*prod;
}

//scaling function for mag
void scaleMag(Image* mag, double mm,bool constThresh,int centerX,int centerY,int pupilSize){
	int rows = mag->getNumRows();
	int cols = mag->getNumColumns();
	int dY,dX;
	
	double prod = 255/mm;
	
	for(int cr=0;cr<rows;cr++){
		if(!constThresh){
			dY = cr-centerY;
			dY = dY*dY;
		}
		for(int cc=0;cc<cols;cc++){
			if(constThresh)
				mag->setPixelElement(cr,cc,magThresh(prod*mag->getPixelElement(cr,cc)));
			else{
				dX = cc-centerX;
				dX = dX*dX;
				double dist = sqrt(dX+dY);
				mag->setPixelElement(cr,cc,magVarThresh(prod*mag->getPixelElement(cr,cc),dist,pupilSize));
			}
		}
	}
}

//sobel filter proccess
double sobelFilter(Image* smoothed, Image* mag,Image* mag2, double** phase,int* center,int pupilSize){
	//for each position in the smoothed image
	int rows = smoothed->getNumRows();
	int cols = smoothed->getNumColumns();
	int halfSize = 1;
	int colLim = cols-halfSize;
	int rowLim = rows-halfSize;
	double maxMag = 0;
	double verticalFilter;
	double horizontalFilter;
	int crM1,crP1,ccM1,ccP1;
	bool constThresh = true;
	
	//uses a constant threshold before the ceneter is determined
	if(center[0]!=0 && center[1]!=0)
		constThresh=false;
	
	//for each col
	for(int cc=halfSize;cc<colLim;cc++){
		ccM1 = cc-1;
		ccP1 = cc+1;
		//for each row
		for(int cr=halfSize;cr<rowLim;cr++){
			verticalFilter =0;
			horizontalFilter = 0;
			crM1 = cr-1;
			crP1 = cr+1;
			
			//at each position apply the filters
			//vertical filter (filling the array manually - start at the top left, work right, then return to the next row
			verticalFilter += smoothed->getPixelElement(crM1,ccM1)*-1;
			verticalFilter += smoothed->getPixelElement(crM1,ccP1);
			verticalFilter += smoothed->getPixelElement(cr,ccM1)*-2;
			verticalFilter += smoothed->getPixelElement(cr,ccP1)*2;
			verticalFilter += smoothed->getPixelElement(crP1,ccM1)*-1;
			verticalFilter += smoothed->getPixelElement(crP1,ccP1);
			
			//horizontal filter
			horizontalFilter += smoothed->getPixelElement(crM1,ccM1)*-1;
			horizontalFilter += smoothed->getPixelElement(crM1,cc)*-2;
			horizontalFilter += smoothed->getPixelElement(crM1,ccP1)*-1;
			horizontalFilter += smoothed->getPixelElement(crP1,ccM1);
			horizontalFilter += smoothed->getPixelElement(crP1,cc)*2;
			horizontalFilter += smoothed->getPixelElement(crP1,ccP1);

			//determine the magnitude at the position and add it to the mag image
			double magnitude = sqrt(verticalFilter*verticalFilter+horizontalFilter*horizontalFilter);
			if(magnitude>maxMag)
				maxMag = magnitude;
			mag->setPixelElement(cr,cc,magnitude);
			mag2->setPixelElement(cr,cc,magnitude);
			double ph=0;
			//determine the phase
			if(horizontalFilter==0 && verticalFilter>0){
				ph = scale(-3.14159);
			}
			else if(horizontalFilter==0){
				ph = scale(3.14159); 
			}
			else{
				ph = scale(-1*atan2(verticalFilter,horizontalFilter));
			}
/*			if(cc==332 && cr==210)
				cout<<"ph: "<<ph<<endl;
			if(cc==332 && cr==265)
				cout<<"ph: "<<ph<<endl;
			if(cc==380 && cr==262)
				cout<<"ph: "<<ph<<endl;
			if(cc==375 && cr==205)
				cout<<"ph: "<<ph<<endl;*/
			//set the value
			phase[cr][cc]=ph;
		}
	}
	scaleMag(mag,maxMag,constThresh,center[1],center[0],pupilSize);
	return maxMag;
}

//returns the position in a 1d arrary for the 2d value
int findPos(int row, int col, int width){
	return (width*row)+col;
}

////smoothing function
//void smooth(Image* img, Image* medIMG, int filterSize){
//	int maxVals = filterSize*filterSize;
//	int vals1[filterSize][filterSize];
//	int vals2[maxVals];
//	int rows = img->getNumRows();
//	int cols = img->getNumColumns();
//	int halfSize = (filterSize/2);
//	
//	int colLim = cols-halfSize;
//	int rowLim = rows-halfSize;
//	
//	int ccMinusHalfSize;
//	int crMinusHalfSize;
//	int count = 0;
//	int processed = 0;
//	bool right = true;
//	int cfc;
//	int currentMed;
//	int colManip;
//	
//	//read initial filter content
//	for(int cr=halfSize;cr<filterSize+halfSize;cr++)
//		crMinusHalfSize = cr-halfSize;
//		for(int cc=halfSize;cc<filterSize+halfSize;cc++){
//			ccMinusHalfSize = cc-halfSize;
//			int val = img->getPixelElement(crMinusHalfSize,ccMinusHalfSize);
//			vals1[crMinusHalfSize][ccMinusHalfSize] = val;
//			vals2[count] = val;
//			count++;
//		}
//	//add median value of first pixel
//	currentMed = quick_select(vals2,maxVals);
//	medIMG->setPixelElement(halfSize,halfSize,currentMed);
//	
//	//iterate through the rest of the image
//	//for number of rows
//	for(int cr=halfSize;cr<rowLim;cr++){
//		crMinusHalfSize = cr-halfSize;
//		right = !right;
//		//for number of columns
//		for(int cc=halfSize;cc<colLim;cc++){
//			if(processed==1)
//				cc+=1;
//			
//			if(right)
//				colManip = cc;
//			else
//				colManip = colLim-cc-1;
//			ccMinusHalfSize = cc-halfSize;
//			count = 0;
//			processed++;
//
//				//shift filter right
//				if(right){
//					//move the data left
//					for(int y=0;y<filterSize;y++)
//						for(int x=0;x<filterSize-1;x++)
//							vals1[y][x] = vals1[y][x+1];
//					//read and replace the rightmost column of the filter data
//					cfc = 20;
//					for(int cfr=0;cfr<filterSize;cfr++)
//						vals1[cfr][cfc] = img->getPixelElement(crMinusHalfSize+cfr,colManip+halfSize);
//				}
//				//shift filter left
//				else{
//					//move the data right
//					for(int y=0;y<filterSize;y++)
//						for(int x=filterSize-1;x>0;x--)
//							vals1[y][x] = vals1[y][x-1];
//						
//					//read and replace the leftmost column of the filter data
//					cfc = 0;
//					for(int cfr=0;cfr<filterSize;cfr++)
//						vals1[cfr][cfc] = img->getPixelElement(crMinusHalfSize+cfr,colManip-halfSize);
//				}
//				//fill vals2 with the values from vals1
//				for(int y=0;y<filterSize;y++)
//					for(int x=0;x<filterSize;x++){
//						vals2[count] = vals1[y][x];
//						count++;
//					}
//				//calc and add the median value at each pixel
//				currentMed = quick_select(vals2,maxVals);
//				//add value
//				medIMG->setPixelElement(cr,colManip,currentMed);
//		}
//		if(cr!=rowLim-1){
//		count = 0;
//			//shift filter down
//			for(int y=0;y<filterSize-1;y++)
//				for(int x=0;x<filterSize;x++)
//					vals1[y][x] = vals1[y+1][x];
//			//read and replace the bottommost row of the filter data
//			int cfr = 20;
//			for(int fc=0;fc<filterSize;fc++)
//				vals1[cfr][fc] 	cout<<"centerx: "<<centerX<<"\ncenterY: "<<centerY<<endl;= img->getPixelElement(crMinusHalfSize+cfr,colManip-halfSize+fc);
//
//			
//			//fill vals2 with the values from vals1
//			for(int y=0;y<filterSize;y++)
//				for(int x=0;x<filterSize;x++){
//					vals2[count] = vals1[y][x];
//					count++;
//				}
//			//calc and add the median value at each pixel
//			currentMed = quick_select(vals2,maxVals);
//			//add value
//			medIMG->setPixelElement(cr+1,colManip,currentMed);
//		}
//	}
//	
//	
///*	
//	//for each column
//	for(int cc=halfSize;cc<colLim;cc++){
//		filterColUpperLim = cc+halfSize+1;
//		ccMinusHalfSize = cc-halfSize;
//		//for each row
//		for(int cr=halfSize;cr<(rowLim);cr++){
//			count = 0;
//			filterRowUpperLim = cr+halfSize+1;
//			crMinusHalfSize = cr-halfSize;
//			//calc the median around each position
//			for(int x=ccMinusHalfSize;x<filterColUpperLim;x++){
//				for(int y=crMinusHalfSize;y<filterRowUpperLim;y++){
//					//add the vals to a vector
//					vals[count] = img->getPixelElement(y,x);
//					count++;
//				}
//			}
//			//sort the vector
//			std::sort(vals,vals+maxVals);
//			//set the current target to the median value
//			medIMG->setPixelElement(cr,cc,vals[medianInd]);
//		}
//	}*/
//}

//returns the limbic radius
int findLimbRad(Image* mag,int centerX,int centerY,int pupilSize){
	int minRad = pupilSize+20;
	int maxRad = pupilSize+150;
	
	int q1x=0;
	int q1y=0;
	int maxSum=0;
	int maxSumRad=0;
	
	bool startCount = false;
	int count = 0;//keeps track of the number of iterations since the last good fit
	
/*	//for each radius
	for(int r=minRad;r<maxRad;r++)
	{
		int sum = 0;
		//for each theta value
		for(double theta=-.8;theta<.8;theta+=.05)
		{
			q1x = r*cos(theta);
			q1y = r*sin(theta);
			if(q1y+centerY>470 || q1y+centerY<10 || centerY-q1y<10 || centerY-q1y>470 ||
					q1x+centerX>470 || q1x+centerX<10 || centerX-q1x>470 || centerX-q1x<10){
				continue;
			}
			
			
			//sum the points
			//q1
			sum+=mag->getPixelElement(q1y+centerY,q1x+centerX);
			//sum+=mag->getPixelElement(q1x+centerY,q1y+centerX);
			//q2
			sum+=mag->getPixelElement(q1y+centerY,centerX-q1x);
			//sum+=mag->getPixelElement(q1x+centerY,centerX-q1y);
		}
		if(sum>maxSum){
			maxSum=sum;
			maxSumRad=r;
		}
	}*/
	
	//polar coords
	for(int r=minRad;r<maxRad;r++){
		int sum=0;
		for(double theta=0;theta<0.78;theta+=.01){
			q1x = r*cos(theta);
			q1y = r*sin(theta);
	
			//sum the points
			//q1
			sum+=mag->getPixelElement(q1y+centerY,q1x+centerX);
			//q2
			sum+=mag->getPixelElement(q1y+centerY,centerX-q1x);
			//q3
			sum+=mag->getPixelElement(centerY-q1y,centerX-q1x);
			//q4
			sum+=mag->getPixelElement(centerY-q1y,q1x+centerX);
			}
		//compare the results
		if(sum>maxSum){
			startCount = true;
			count = 0;
			maxSum=sum;
			maxSumRad=r;
		}
		if(startCount && maxSum>40000)
			count++;
		
		//if the number of iterations since the last good fit is large enough, the function returns
		if(count>50)
			return maxSumRad;
	}
	
	//cartisian coords, mostly works
/*	double cosTheta = cos(.72);
	for(int r=minRad;r<maxRad;r++){
		int sum=0;
		int r2 = r*r;
		for(int i=r*cosTheta;i<r;i++){
			q1x = i;
			q1y = sqrt(r2-i*i);
	
			//sum the points
			//q1
			sum+=mag->getPixelElement(q1y+centerY,q1x+centerX);
			sum+=mag->getPixelElement(q1x+centerY,q1y+centerX);
			//q2
			sum+=mag->getPixelElement(q1y+centerY,centerX-q1x);
			sum+=mag->getPixelElement(q1x+centerY,centerX-q1y);
			//q3
			sum+=mag->getPixelElement(centerY-q1y,centerX-q1x);
			sum+=mag->getPixelElement(centerY-q1x,centerX-q1y);
			//q4
			sum+=mag->getPixelElement(centerY-q1y,q1x+centerX);
			sum+=mag->getPixelElement(centerY-q1x,q1y+centerX);
			
			}
		//compare the results
		if(sum>maxSum){
			maxSum=sum;
			maxSumRad=r;
		}
	}*/
	return maxSumRad;
	
}

//returns the calculated radius for the pupil
int findRadius(Image* mag,int centerX,int centerY){
	int minRad = 25;
	int maxRad = 130;
	
	int q1x=0;
	int q1y=0;
	int maxSum=0;
	int maxSumRad=0;
	bool startCount = false;
	int count = 0;//keeps track of the number of iterations since the last good fit
	
	for(int r=minRad;r<maxRad;r++){
		int sum=0;
		int r2 = r*r;
		for(int i=0;i<r;i++){
			q1x = i;
			q1y = sqrt(r2-i*i);
				
			//sum the points
			//q1
			sum+=mag->getPixelElement(q1y+centerY,q1x+centerX);
			sum+=mag->getPixelElement(q1x+centerY,q1y+centerX);
			//q2
			sum+=mag->getPixelElement(q1y+centerY,centerX-q1x);
			sum+=mag->getPixelElement(q1x+centerY,centerX-q1y);
			//q3
			sum+=mag->getPixelElement(centerY-q1y,centerX-q1x);
			sum+=mag->getPixelElement(centerY-q1x,centerX-q1y);
			//q4
			sum+=mag->getPixelElement(centerY-q1y,q1x+centerX);
			sum+=mag->getPixelElement(centerY-q1x,q1y+centerX);
			
			}
		//compare the results
		if(sum>maxSum){
			startCount = true;
			count = 0;
			maxSum=sum;
			maxSumRad=r;
		}
		if(startCount && maxSum>40000)
			count++;
		
		//if the number of iterations since the last good fit is large enough, the function returns
		if(count>30)
			return maxSumRad;
	}
	return maxSumRad;
}

//unwraps the iris according to the width and height of the declared unwrap image
void unwrapIris(Image* img,Image* unwrap,int prad,int lrad,int* center){
	double deltaTheta,theta,deltaR,r;
	deltaTheta = 6.28318/unwrap->getNumColumns();
	deltaR = (lrad-prad)/(double)unwrap->getNumRows();
	int centerX = center[1];
	int centerY = center[0];
	int xCoord,yCoord;
	
	//for each bar around the iris
	for(int bar=0;bar<unwrap->getNumColumns();bar++){
		theta = deltaTheta*bar;
		//for each radius value
		for(int i=0;i<unwrap->getNumRows();i++){
			r = prad + i*deltaR;
			xCoord = centerX+r*cos(theta);
			yCoord = centerY+r*sin(theta);
			if(xCoord<10 || xCoord>(img->getNumColumns()-10) || yCoord<10 || yCoord>(img->getNumRows()-10)){
				continue;
			}
			else
				unwrap->setPixelElement(i,bar,img->getPixelElement(yCoord,xCoord));
		}	
	}
}

//preforms a haar filter at the location and sets the value in template
int preformHaarAt(Image* unwrap,Image* templ,int cr,int cc,int kernalSize){
	int maxCol = unwrap->getNumColumns();
	int maxRow = unwrap->getNumRows();
	int halfSize = kernalSize/2;
	int rowStart = cr-halfSize;
	int rowFinish = cr+halfSize;
	int colStart = cc-halfSize;
	int colFinish=cc+halfSize;
	int colCount;
	double currentVal = 0;
	unsigned char black = 0;
	
	//for each pixel within the kernal size
	for(int row=rowStart;row<rowFinish;row++){
		colCount=0;
		for(int col=colStart;col<colFinish;col++){
			//left side of the kernal
			if(colCount<halfSize){
				if(row<0)
					currentVal+=0.0;
				else if(row>maxRow-1)
					currentVal+=0.0;
				else if(col<0)
					currentVal+=-1.0*unwrap->getPixelElement(row,maxCol+col);
				else if(col>maxCol-1)
					currentVal+=-1.0*unwrap->getPixelElement(row,col-maxCol);
				else
					currentVal+=-1.0*unwrap->getPixelElement(row,col);		
			}
			//right side of the kernal
			else{
				if(row<0)
					currentVal+=0.0;
				else if(row>maxRow-1)
					currentVal+=0.0;
				else if(col<0)
					currentVal+=unwrap->getPixelElement(row,maxCol+col);
				else if(col>maxCol-1)
					currentVal+=unwrap->getPixelElement(row,col-maxCol);
				else
					currentVal+=unwrap->getPixelElement(row,col);
			}
			colCount++;
		}
	}
	//scale currentVal
	currentVal = currentVal/(double)(kernalSize*kernalSize*1.414);
	
	//sets the template value if negative (it is already set to positive)
	if(currentVal<=0)
		templ->setPixelElement(cr,cc,black);
	//returns 1 if the value is positive
	if(currentVal>=1)
		return 1;
	//returns -1 if the val is negative
	else if(currentVal<-1)
		return -1;
	//returns 0 if the value is in between
	else
		return 0;	
}

//creates a template for storage and comparison - the template identifies the user/iris
void makeTemplate(Image* unwrap, Image* templ,Image* mask){
	int unwrapHeight = unwrap->getNumRows();
	int unwrapWidth = unwrap->getNumColumns();
	int haarSize = 8;
//	int result;//1 if haar is pos, -1 if negative, 0 if in between
	unsigned char black = 0;
	
	//for each pixel in the unwrap
	for(int cr=0;cr<unwrapHeight;cr++){
		for(int cc=0;cc<unwrapWidth;cc++){
			if(preformHaarAt(unwrap,templ,cr,cc,haarSize)==0)
				mask->setPixelElement(cr,cc,black);
		}
	}
}

int main(int argc, char** argv){
	int maxImages = 27324;
	int filterSize = 21;//median filter size
	int unwrapWidth = 180;
	int unwrapHeight = 96;
	
	//print toggles
	bool printStatus = false;
	bool printTimes = false;
	bool printLoading = true;
	bool printUnwrap = false;
	bool printMask = true;
	bool printTemplate = true;
	bool printMag2 = false;
	bool drawCircles = true;
	bool printMag = false;
	bool printMedian = false;
	
	int prad,lrad,centerX,centerY;
	
	//generate the list of files
	system("ls Images/*.bmp > imageNames.txt");
	
	//open the imageName file
	ifstream fileNames;
	fileNames.open("imageNames.txt", ios::in);
	
	string image;
	string path = "";
	
	//timer variables
	clock_t start;
	clock_t finish;
	clock_t start2;
	clock_t finish2;
	double medTime,sobelTime,houghTime,averageLapTime;
	if(printTimes){
		medTime=0;
		sobelTime=0;
		houghTime=0;
	}
	
	//counter
	int count=0;
	averageLapTime=0;
	
	Image* img = new Image();
	Image* cleanImg = new Image();
	Image* medianIMG = new Image(480,640);
    Image* mag = new Image(480,640);
    Image* mag2 = new Image(480,640);
    Image* unwrap = new Image(unwrapHeight,unwrapWidth);
    Image* templ = new Image(unwrapHeight,unwrapWidth);
    Image* mask = new Image(unwrapHeight,unwrapWidth);

    //declare the phase array
    double** phase;
    phase = new double* [480];
    for(int i=0;i<480;i++)
    	phase[i] = new double[640];
    
    //declare the alteration count(used to find the center)
    int** alterations;
    alterations = new int*[480];
    for(int i=0;i<480;i++)
    	alterations[i] = new int[640];
    
    //declare the centerpoint array (two values - row,column)
    int center[2];
    double maxMag;
    	                            
	if(fileNames.good()){
		getline(fileNames,image);
		do{
			if(printLoading)
				start2 = clock();
			//set the images to zero
			medianIMG->setAllData(0);
			mag->setAllData(0);
			templ->setAllData(255);
			mask->setAllData(255);
			mag2->setAllData(0);
			
			//set the center values to zero
			center[0] = 0;
			center[1] = 0;
			
			//set the radius values to zero
			prad = 0;
			lrad = 0;
			
		    //set the alteration values to zero
		    for(int i=0;i<480;i++)
		    	for(int y=0;y<640;y++)
		    		alterations[i][y]=0;
		    
			//load the image
			img->readBMP(image,path);
			cleanImg->readBMP(image,path);
			
		    if(printStatus)
		    	cout<<"begin smooth"<<endl;
		    if(printTimes){
		    	start = clock();
		    }
			//smooth the image
			//smooth(img,medianIMG,filterSize);
		    MedianFilter(img,medianIMG,filterSize);
		    if(printTimes){
		    	finish = clock();
		    	medTime+=(finish-start)/(double)CLOCKS_PER_SEC;
		    }
		    if(printStatus)
		    	cout<<"smooth complete"<<endl;
			medianIMG->writeBMP("medianIMGTest.bmp","");
			
			if(printMedian){
				//write the image to a new file
				string s2 = "median/";
				s2.append(image.substr(7,14));
				s2.append("_median.bmp");
				medianIMG->writeBMP(s2,path);
			}
			
		    if(printTimes){
		    	start = clock();
		    }
			//apply sobel filter
			maxMag = sobelFilter(medianIMG,mag,mag2,phase,center,0);
		    if(printTimes){
		    	finish = clock();
		    	sobelTime+=(finish-start)/(double)CLOCKS_PER_SEC;
		    }
			
			mag->writeBMP("magTEST","");
	//		phase->writeBMP("phaseTEST","");
			
			if(printMag){
				//write the image to a new file
				string s2 = "mag/";
				s2.append(image.substr(7,14));
				s2.append("_mag.bmp");
				mag->writeBMP(s2,path);
			}
			
		    if(printTimes){
		    	start = clock();
		    }
		    if(printStatus)
		    	cout<<"begin hough"<<endl;
			//preform hough transform
			houghTrans(mag,phase,alterations,center,filterSize);
		    if(printTimes){
		    	finish = clock();
		    	houghTime+=(finish-start)/(double)CLOCKS_PER_SEC;
		    }
		    if(printStatus)
		    	cout<<"hough complete"<<endl;
		   
			//define center
			centerX = center[1];
			centerY = center[0];
		    
		    //find the pupil radius value
		    prad = findRadius(mag,centerX,centerY);
		    
		    //redraw the magnitude image scaling the threshold value as you move out from the center
			scaleMag(mag2,maxMag,false,centerX,centerY,prad);
			
		    mag2->writeBMP("mag2TEST","");
		    
		    //find the limbic radius value
		    lrad = findLimbRad(mag2,centerX,centerY,prad);
		    
			if(printMag2){
				//write the image to a new file
				string s2 = "mag2/";
				s2.append(image.substr(7,14));
				s2.append("_mag2.bmp");
				mag2->writeBMP(s2,path);
			}
		    
		    if(lrad==0){
		    	cout<<"\n0Radius@: "<<image.substr(7,14)<<"\nTrying again."<<endl;
		    }
		    
		    
		    if(drawCircles){
				//proccess the image - draw a circle//
				unsigned char white = 255;
					
					//define radius squared
					int r = prad;
					int r2 = r*r;
					
					//define the circle in the first quadrent and draw in all
					int q1x = 0;
					int q1y = 0;
					
					//polar coords
/*					for(double theta=0;theta<(3.14159/2);theta+=.01){
						q1x = r*cos(theta);
						q1y = r*sin(theta);
						
						//draw the circle
						//q1
						img->setPixelElement(q1y+centerY,q1x+centerX,white);
						//q2
						img->setPixelElement(q1y+centerY,centerX-q1x,white);
						//q3
						img->setPixelElement(centerY-q1y,centerX-q1x,white);
						//q4
						img->setPixelElement(centerY-q1y,q1x+centerX,white);
					}*/
					
					//cartisian coords
					for(int i=0;i<r;i++){
						q1x = i;
						q1y = sqrt(r2-i*i);
		
						//draw the circle
						//q1
						img->setPixelElement(q1y+centerY,q1x+centerX,white);
						img->setPixelElement(q1x+centerY,q1y+centerX,white);
						//q2
						img->setPixelElement(q1y+centerY,centerX-q1x,white);
						img->setPixelElement(q1x+centerY,centerX-q1y,white);
						//q3
						img->setPixelElement(centerY-q1y,centerX-q1x,white);
						img->setPixelElement(centerY-q1x,centerX-q1y,white);
						//q4
						img->setPixelElement(centerY-q1y,q1x+centerX,white);
						img->setPixelElement(centerY-q1x,q1y+centerX,white);
					}
		    	}
				
				//draw the limbic boundry as well
				if(drawCircles){
					unsigned char white = 255;
					
					int r = lrad;
					int r2 = r*r;
					
					//define the circle in the first quadrent and draw in all
					int q1x = 0;
					int q1y = 0;
					
					for(int i=0;i<r;i++){
						q1x = i;
						q1y = sqrt(r2-i*i);
		

						//draw the circle
						//q1
						img->setPixelElement(q1y+centerY,q1x+centerX,white);
						img->setPixelElement(q1x+centerY,q1y+centerX,white);
						//q2
						img->setPixelElement(q1y+centerY,centerX-q1x,white);
						img->setPixelElement(q1x+centerY,centerX-q1y,white);
						//q3
						img->setPixelElement(centerY-q1y,centerX-q1x,white);
						img->setPixelElement(centerY-q1x,centerX-q1y,white);
						//q4
						img->setPixelElement(centerY-q1y,q1x+centerX,white);
						img->setPixelElement(centerY-q1x,q1y+centerX,white);
					}
				}

				if(drawCircles){
					//write the image to a new file
					string s1 = "alteredImages/";
					s1.append(image.substr(7,14));
					s1.append("_circleEye.bmp");
					img->writeBMP(s1,path);
				}
				
				//unwrap the iris
				unwrapIris(cleanImg,unwrap,prad,lrad,center);
				//perform clahe (contrast adjustment)
				CLAHE(*unwrap->getData(),unwrapWidth,unwrapHeight,0,255,unwrapWidth/20,unwrapHeight/20,
						256,5.8f);
				
				if(printUnwrap){
					//write the image to a new file
					string s2 = "unwrappedIris/";
					s2.append(image.substr(7,14));
					s2.append("_unwrap.bmp");
					unwrap->writeBMP(s2,path);
				}
				
				//generate a template - two images
				makeTemplate(unwrap,templ,mask);
				
				if(printTemplate){
					//write the image to a new file
					string s2 = "templates/";
					s2.append(image.substr(7,14));
					s2.append("_template.bmp");
					templ->writeBMP(s2,path);
				}
				
				if(printMask){
					//write the image to a new file
					string s2 = "masks/";
					s2.append(image.substr(7,14));
					s2.append("_mask.bmp");
					mask->writeBMP(s2,path);
				}
				
				//inc count
				count+=1;
				
				//get next string
				getline(fileNames,image);
				
				if(printLoading){
					finish2 = clock();
				}
				
				if(printLoading){
					int progress = (20*count)/maxImages;
					cout<<"Progress: {";
					for(int x=0;x<20;x++){
						if(progress>=x)
						cout<<"=";
						else
							cout<<' ';
					}
					averageLapTime += (finish2-start2)/(double)CLOCKS_PER_SEC;
					cout<<"}\tAverage Lap Time: "<<averageLapTime/(count+1)<<"\r"<<flush;
				}
				
		}while(!fileNames.eof() && count<maxImages);
		cout<<"\n\ndone?("<<maxImages<<" images processed)"<<endl;
	}
	
	if(printTimes){
		cout<<"Times:"<<endl;
		cout<<"\tMedian Filter Duration: "<<medTime<<endl;
		cout<<"\tSobel Filter Duration: "<<sobelTime<<endl;
		cout<<"\tHough Transform Duration: "<<houghTime<<endl;
	}
	
	//close the input
	fileNames.close();
	delete img;
	delete medianIMG;
	delete mag;
	delete templ;
	delete mask;
	delete mag2;
	delete cleanImg;
	delete unwrap;
	
		for(int i=0;i<480;i++){
			delete phase[i];
			delete alterations[i];
		}
	
	delete phase;
	delete alterations;
	
	return 0;
}
