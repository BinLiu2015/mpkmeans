#include <limits.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <math.h>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/ml/ml.hpp"
#include <pthread.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <time.h>
#include <vector>
using namespace cv;
using namespace std;


//ioOperations
vector<string>* getChunkLines(std::istream& str, int chunkSize);
void loadParameters(int argc, char** argv, int &num_of_threads, int &numClusters,int &numClustersForChunks, int &chunk_size, char* &fileName);
Mat vec2Mat(std::vector< vector<float> > chunkInstances);
vector<vector<float> > getChunk(vector<string> lines);
void evaluate(char* fileName,int chunk_size);

static double numFeatures = 0;
