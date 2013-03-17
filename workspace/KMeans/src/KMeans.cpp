#include "stdafx.h"

/**
 * chunk information to be passed to clustering threads
 */
struct chunkInfo
{
	pthread_t tid;
	int numClustersForChunks, chunkNo, attempts;
	Mat *data,centers;
};

/**
 * The clustering thread: Classifies the data and returns the centers
 * Besides, it deletes the training data.
 */
void *cluster(void *param){
	chunkInfo *args;
	args = (chunkInfo *) param;
	cout << "\x1b[1;31m++Clustering chunk " << args->chunkNo << " with " << args->data->rows << " rows\x1b[0m\n"; cout.flush();

	Mat labels;
	kmeans(*(args->data), args->numClustersForChunks, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 0.0001, 10000), args->attempts, KMEANS_PP_CENTERS, args->centers);

	cout << "\x1b[36;1m--Clustered chunk " << args->chunkNo << " with " << args->data->rows << " rows\x1b[0m\n"; cout.flush();
	delete args->data;
}

/**
 * Aggregates outputs of all chunks into a singe matrix
 */
Mat aggregateCenters(vector< chunkInfo* > chunksInfoList){
	int numColumns = chunksInfoList[0]->centers.cols;
	int numPoints = 0;
	for(unsigned int i=0; i< chunksInfoList.size();i++)
		numPoints += chunksInfoList[i]->centers.rows;

	//coping centers of each chunk to a single matrix
	Mat mat(numPoints,numColumns,CV_32F);
	int pointsAdded = 0;
	for(unsigned int threadNo=0; threadNo< chunksInfoList.size();threadNo++){	//for each chunk
		for(int i=0; i<chunksInfoList[threadNo]->centers.rows;	i++, pointsAdded++){	//for each center in each chunk
			for(int j=0; j<numColumns; j++){
				mat.at<float>(pointsAdded,j) = chunksInfoList[threadNo]->centers.at<float>(i,j);
			}
		}
	}
	return mat;
}

int main( int argc, char** argv )
{
	clock_t start = clock();

	//Loading parameters
	int numClustersForChunks =50, numClusters = 5;
	int num_of_threads = 30;
	int attempts = 3;
	int chunk_size = 100000;
	char* fileName;
	loadParameters(argc,argv, num_of_threads, numClusters, numClustersForChunks, chunk_size, fileName);
	vector < chunkInfo* > chunksInfoList;

	cout << "Loading dataset from " << fileName << endl;
	ifstream file(fileName);
	int chunkNo = 0;

	//running chunks
	while(1){
		//cout << "Loading Chunk " << chunkNo << "...\n"; cout.flush();
		Mat* data;
		data = vec2Mat( getChunk(file,chunk_size) );
		int lastChunkSize = data->rows;

		chunkInfo *currentChunkInfo = new chunkInfo();
		currentChunkInfo->data = data;
		currentChunkInfo->numClustersForChunks = numClustersForChunks;
		currentChunkInfo->chunkNo = chunkNo;
		currentChunkInfo->attempts = attempts;

		pthread_create(&(currentChunkInfo->tid),NULL,cluster,(void*)currentChunkInfo);
		chunksInfoList.push_back(currentChunkInfo);
		chunkNo++;
		if(lastChunkSize < chunk_size){
			//In this case, EOF has been reached, so we should wait for all threads
			cout << "waiting for threads..." << endl;
			for(unsigned int i=0; i<chunksInfoList.size(); i++)
				pthread_join(chunksInfoList[i]->tid,NULL);
			break;
		}
	}

	cout<<"Pass 2..." << endl; cout.flush();
	Mat centers,labels;
	Mat points = aggregateCenters(chunksInfoList);
	cout << "points: " << points.rows << endl; cout.flush();
	kmeans(points, numClusters, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 0.0001, 10000), attempts, KMEANS_PP_CENTERS, centers );
	cout << centers;

	clock_t end = clock();
	double time_elapsed_in_seconds = (end - start)/((double)CLOCKS_PER_SEC);
	cout << endl << "Clustering time in seconds: " <<time_elapsed_in_seconds << endl;
	computerSSE()
}
