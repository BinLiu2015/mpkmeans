/*
 * FileOperations.cpp
 *
 *  Created on: Mar 14, 2013
 *      Author: ali
 */
#include "stdafx.h"
using namespace std;
using namespace cv;

std::vector<float> getNextLineAndSplitIntoTokens(std::istream& str){
	std::vector<float>   result;
	std::string                line;
	std::getline(str,line);

	std::stringstream          lineStream(line);
	std::string                cell;

	while(std::getline(lineStream,cell,','))
	{
		result.push_back(atof(cell.c_str()));
	}
	return result;
}

std::vector<vector<float> > getChunk(std::istream& str, int chunkSize){
	std::vector< vector<float> > chunkInstances;
	std::vector<float>   instance;

	for(int i =0; i<chunkSize; i++ ){
		instance = getNextLineAndSplitIntoTokens(str);
		if(instance.size() > 0)
			chunkInstances.push_back(instance);
		else
			break;
	}

	return chunkInstances;
}

Mat* vec2Mat(const std::vector< vector<float> > chunkInstances){

	if(chunkInstances.size() == 0)
		return new Mat(0,0,CV_32F);
	else if (chunkInstances[0].size() == 0)
		return new Mat(0,0,CV_32F);

	int num_instances = chunkInstances.size();
	int num_attributes = chunkInstances[0].size();

	Mat* mat = new Mat(num_instances,num_attributes,CV_32F);
	for(int i=0; i<num_instances; i++){
		for(int j=0; j<num_attributes; j++){
			mat->at<float>(i,j) = chunkInstances[i][j];
		}
	}
	return mat;
}

void evaluate(char* fileName,int chunk_size){

}





void printUsage(){
	cout << "Usage: KMeans -f fileName [-k num_of_clusters] [-c chunk_size] [-chk num_of_cluster_centers_for_each_chunk] [-t max_threads]" << endl;
	exit(0);
}

void loadParameters(int argc, char** argv, int &num_of_threads, int &numClusters,
		int &numClustersForChunks, int &chunk_size, char* &fileName){
	int i = 1;
	if(argc < 2)
		printUsage();
	while (i + 1 < argc){ // Check that we haven't finished parsing already
		if (strcmp(argv[i],"-f") == 0) {
			fileName = argv[i+1];
		} else if (! strcmp(argv[i] , "-k")) {
			numClusters = atoi(argv[i+1]);
		} else if (! strcmp(argv[i] , "-c")) {
			chunk_size = atoi(argv[i+1]);
		} else if (! strcmp(argv[i+1] , "-chk")) {
			numClustersForChunks = atoi(argv[i+1]);
		}else if (! strcmp(argv[i+1] , "-t")) {
			num_of_threads = atoi(argv[i+1]);
		}
		i++;
	}
}

template< typename TYPE >
void print(TYPE val)
{
	std::cout << val << " ";
}
