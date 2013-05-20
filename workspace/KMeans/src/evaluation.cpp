/*
 *  evaluation.cpp
 *
 *  Created on: Mar 22, 2013
 *      Author: ali
 */

#include "stdafx.h"
using namespace std;
using namespace cv;


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
	cout << "SSE:\t" << computeSSE(centers,data) << endl;

}
