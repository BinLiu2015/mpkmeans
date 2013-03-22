/*
 * FileOperations.cpp
 *
 *  Created on: Mar 14, 2013
 *      Author: ali
 */
#include "stdafx.h"
using namespace std;
using namespace cv;

vector<string>* getChunkLines(std::istream& str, int chunkSize){
	vector<string>*   lines = new vector<string>;
	lines->reserve(chunkSize);
	for(int i =0; i<chunkSize; i++ ){
		std::string line;
		getline(str,line);

		lines->push_back(line);
		if(line.size() <1)
			break;
	}
	return lines;
}

std::vector<float> splitIntoTokens(std::string line){

	std::vector<float>   result;

	if(numFeatures > 0) result.reserve(numFeatures);	//performance tuning

	std::istringstream          lineStream(line);
	std::string                cell;
	while(std::getline(lineStream,cell,','))
	{
		result.push_back(atof(cell.c_str()));
	}

	if(numFeatures <= 0) numFeatures = result.size();

	return result;
}

vector<vector<float> > getChunk(vector<string> lines){
	int currentChunkSize = lines.size();
	std::vector< vector<float> > chunkInstances;
	chunkInstances.reserve(currentChunkSize);
	std::vector<float>   instance;

	for(int i =0; i<currentChunkSize; i++ ){
		instance = splitIntoTokens(lines[i]);
		if(instance.size() > 0)
			chunkInstances.push_back(instance);
		else
			break;
	}

	return chunkInstances;
}

Mat vec2Mat(const std::vector< vector<float> > chunkInstances){

	if(chunkInstances.size() == 0)
		return Mat(0,0,CV_32F);
	else if (chunkInstances[0].size() == 0)
		return Mat(0,0,CV_32F);

	int num_instances = chunkInstances.size();
	int num_attributes = chunkInstances[0].size();

	Mat mat(num_instances,num_attributes,CV_32F);
	for(int i=0; i<num_instances; i++){
		for(int j=0; j<num_attributes; j++){
			mat.at<float>(i,j) = chunkInstances[i][j];
		}
	}
	return mat;
}


void printUsage(){
	cout << "Usage: KMeans -f fileName [-k num_of_clusters] [-c chunk_size] [-chk num_of_cluster_centers_for_each_chunk] [-t max_threads]" << endl;
	exit(0);
}

void loadParameters(int argc, char** argv, int &num_of_threads, int &numClusters,
		int &numClustersForChunks, int& chunk_size, char* &fileName){
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
		} else if (! strcmp(argv[i] , "-chk")) {
			numClustersForChunks = atoi(argv[i+1]);
		}else if (! strcmp(argv[i] , "-t")) {
			num_of_threads = atoi(argv[i+1]);
		}
		i++;
	}
	cout << "num_of_threads: " << num_of_threads << "\tnumClusters: " << numClusters <<
			"\nnumClustersForChunks: " << numClustersForChunks << "chunk_size: " << chunk_size << endl;
}

template< typename TYPE >
void print(TYPE val)
{
	std::cout << val << " ";
}
