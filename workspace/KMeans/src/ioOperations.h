/*
 * ioOperations.h
 *
 *  Created on: Mar 22, 2013
 *      Author: ali
 */

using namespace cv;
using namespace std;

#ifndef IOOPERATIONS_H_
#define IOOPERATIONS_H_

vector<string>* getChunkLines(std::istream& str, int chunkSize);
void loadParameters(int argc, char** argv, int &num_of_threads, int &numClusters, int &numClustersForChunks, int& chunk_size, char* &fileName, char* &outputFileName);
Mat vec2Mat(std::vector< vector<float> > chunkInstances);
vector<vector<float> > getChunk(vector<string> lines);
void saveCenters(Mat centers, char* outputFileName);
void evaluate(char* fileName,int chunk_size);
int countLines(char* filename);
Mat loadDatasetAtOnce(char* fileName);



#endif /* IOOPERATIONS_H_ */
