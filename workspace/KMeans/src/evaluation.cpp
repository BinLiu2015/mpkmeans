/*
 *  evaluation.cpp
 *
 *  Created on: Mar 22, 2013
 *      Author: ali
 */

#include "stdafx.h"
using namespace std;
using namespace cv;


double leastDistance(Mat point, Mat centers){
	double leastDistance = DBL_MAX;
	for(int i=0; i<centers.rows; i++){
		double dist = 0;
		for(int j=0; j<point.cols; j++){
			double dimdist =  point.at<float>(0,j) - centers.at<float>(i,j);
			dist += dimdist * dimdist; 	//^2
		}
		dist = sqrt(dist);
		if(leastDistance > dist)
			leastDistance = dist;
	}
	return leastDistance;
}
//void computerSSE(Mat centers, char* fileName){
//	double sse = 0;
//	while(1){
//		vector<string>* lines;
//		int datasize = countLines(fileName);
//
//		ifstream file(fileName);
//		lines = getChunkLines(file,datasize);
//		Mat data = vec2Mat( getChunk(file, lines) );
//		int currentChunkSize = data->rows;
//		for(int i=0; i < currentChunkSize; i++)
//			sse += leastDistance(data->row(i), centers);
//
//		delete data;
//		if(currentChunkSize < chunkSize) break;	//reached EOF
//	}
//	cout << "SSE = " << sse << endl;
//}

double computerSSE(Mat centers, Mat data){
	double sse = 0;
	int rows = data.rows;
	for(int i=0; i < rows; i++)
		sse += leastDistance(data.row(i), centers);
	return sse;
}

void printUsage(){
	cout << "Usage: KMeansEval -df dataFileName -cf centersfileName" << endl;
	exit(0);
}

void loadParameters(int argc, char** argv, char* &dataFileName, char* &centersFileName){
	//DEFAULT VALUES
	centersFileName = "output.txt";

	int i = 1;
	if(argc < 2)
		printUsage();
	while (i + 1 < argc){ // Check that we haven't finished parsing already
		if (strcmp(argv[i],"-df") == 0) {
			dataFileName = argv[i+1];
		} else if (! strcmp(argv[i] , "-cf")) {
			centersFileName = argv[i+1];
		}
		i++;
	}
}

int main( int argc, char** argv )
{
	char* dataFileName;
	char* centersFileName;
	loadParameters(argc, argv, dataFileName, centersFileName);
	Mat data = loadDatasetAtOnce(dataFileName);
	Mat centers = loadDatasetAtOnce(centersFileName);
	cout << "SSE:\t" << computerSSE(centers,data) << endl;

}
