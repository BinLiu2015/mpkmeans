#include "stdafx.h"

/**
 * chunk information to be passed to clustering threads
 */
struct chunkInfo
{
	vector<std::string>* lines;
	int numClustersForChunks, chunkNo, attempts;
	bool jobs_finished = false;
};

vector<pthread_t*> threadsQueue;
vector<Mat*> centersCollection;
vector<chunkInfo*> chunksQueue;

sem_t empty,full;
int f,e;
pthread_mutex_t mutex;

void loadParameters(int argc, char** argv, int &num_of_threads, int &numClusters, int &numClustersForChunks, int& chunk_size, char* &fileName, int &sseEnabled, char* &outputFileName);


/**
 * The clustering function: Classifies the data and returns the centers
 * Besides, it deletes the training data.
 */
void cluster(chunkInfo *chunk){
	Mat data = vec2Mat( getChunk(*(chunk->lines)) );
	cout << "\x1b[1;31m++Clustering chunk " << chunk->chunkNo << " with " << data.rows << " rows\x1b[0m\n"; cout.flush();

	Mat labels,*centers = new Mat();
	if(data.rows <= chunk->numClustersForChunks){
		cout << "\x1b[36;1m-- chunk " <<  chunk->chunkNo <<" skipped because K>N ==> skipping KMeans\x1b[0m\n" << endl;
	}else{
		kmeans(data, chunk->numClustersForChunks, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 0.0001, 10), chunk->attempts, KMEANS_PP_CENTERS, *centers);
		//kmeans(data, chunk->numClustersForChunks, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 0.0001, 10000), chunk->attempts, KMEANS_RANDOM_CENTERS, *centers);
		centersCollection.push_back(centers);
		cout << "\x1b[36;1m--Clustered chunk " << chunk->chunkNo << " with " << data.rows << " rows\x1b[0m\n"; cout.flush();
	}
	delete chunk->lines;
	delete chunk;
}

/**
 * The clustering thread which runs cluster(args)
 */
void *clusterThread(void *param){
	pthread_t *tid = (pthread_t*) param;

	while(1)
	{
		sem_wait(&full);
		pthread_mutex_lock(&mutex);
		chunkInfo *chunk = chunksQueue.back();	//TODO: it actually seems to be a stack!
		//TODO it may cause race condition if the loader thread write another chunk at the end of stack
		// just between these two lines.
		chunksQueue.pop_back();
		pthread_mutex_unlock(&mutex);

		if(chunk->jobs_finished)
			break;
		else
			cluster(chunk);

		sem_post(&empty);
	}
}

/**
 * Aggregates outputs of all chunks into a singe matrix
 */
Mat* aggregateCenters(vector< Mat* > centers){
	int numColumns = centers[0]->cols;
	int numPoints = 0;
	for(unsigned int i=0; i< centers.size();i++)
		numPoints += centers[i]->rows;

	//coping centers of each chunk to a single matrix
	Mat *mat = new Mat(numPoints,numColumns,CV_32F);
	int pointsAdded = 0;
	for(unsigned int chunkNo=0; chunkNo< centers.size();chunkNo++){	//for each chunk
		for(int i=0; i< centers[chunkNo]->rows;	i++, pointsAdded++){	//for each center in each chunk
			for(int j=0; j<numColumns; j++){
				mat->at<float>(pointsAdded,j) = centers[chunkNo]->at<float>(i,j);
			}
		}

	}
	cout << "Total centers: " << mat->rows << endl; cout.flush();
	return mat;
}

void init_threads(int num_of_threads){
	pthread_mutex_init(&mutex, NULL);
	sem_init(&full, 0, 0);
	f=0;
	sem_init(&empty, 0, num_of_threads);
	e=num_of_threads;
	for(int i=0; i<num_of_threads; i++){
		pthread_t *thread = new pthread_t;
		threadsQueue.push_back(thread);
		pthread_create(thread,NULL,clusterThread,(void*) thread);
	}

}

void push_chunk(chunkInfo *currentChunkInfo){
	sem_wait(&empty);
	pthread_mutex_lock(&mutex);
	chunksQueue.push_back(currentChunkInfo);
	pthread_mutex_unlock(&mutex);
	sem_post(&full);
}

void wait_for_threads(int num_of_threads){

	cout << "sending job_finished signal\n";cout.flush();
	for(unsigned int i=0; i<num_of_threads; i++)	//sending job_finished signal
	{
		chunkInfo *currentChunkInfo = new chunkInfo();
		currentChunkInfo->jobs_finished = true;
		push_chunk(currentChunkInfo);
	}

	cout << "waiting for threads..." << endl; cout.flush();
	for(unsigned int i=0; i<num_of_threads; i++)	//waiting for termination
		pthread_join(*(threadsQueue[i]),NULL);

}



int main( int argc, char** argv )
{

	//Loading parameters
	int numClustersForChunks, numClusters,num_of_threads,chunk_size;
	int attempts = 1;
	int sseEnabled = 0;
	char* fileName;
	char* outputFileName;
	loadParameters(argc,argv, num_of_threads, numClusters, numClustersForChunks, chunk_size, fileName, sseEnabled, outputFileName);

	//running threads and semaphores
	init_threads(num_of_threads);

	struct timeval startPhase1;
	gettimeofday(&startPhase1, NULL);

	cout << "Loading dataset from " << fileName << endl;
	ifstream file(fileName);
	int chunkNo = 0;

	//running chunks
	while(1){
		//cout << "Loading Chunk " << chunkNo << "...\n"; cout.flush();
		vector<string>* lines;
		lines = getChunkLines(file,chunk_size);
		int lastChunkSize = lines->size();

		chunkInfo *currentChunkInfo = new chunkInfo();
		currentChunkInfo->lines = lines;
		currentChunkInfo->numClustersForChunks = numClustersForChunks;
		currentChunkInfo->chunkNo = chunkNo;
		currentChunkInfo->attempts = attempts;

		push_chunk(currentChunkInfo);
		chunkNo++;

		if(lastChunkSize < chunk_size){  //In this case, EOF has been reached, so we should wait for all threads
			wait_for_threads(num_of_threads);
			break;
		}
	}
	file.close();

	struct timeval startPhase2;
	gettimeofday(&startPhase2, NULL);

	cout<<"Pass 2..." << endl; cout.flush();
	Mat centers,labels;
	Mat *points = aggregateCenters(centersCollection);
	cout << "points: " << points->rows << endl; cout.flush();
	kmeans(*points, numClusters, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 0.0001, 10000), attempts, KMEANS_PP_CENTERS, centers );

	//save timing info...
	struct timeval finishPhase2;
	gettimeofday(&finishPhase2, NULL);
	double phase1TimeMicrosec = (startPhase2.tv_sec-startPhase1.tv_sec)*1000000+(startPhase2.tv_usec-startPhase1.tv_usec);
	double phase2TimeMicrosec = (finishPhase2.tv_sec-startPhase2.tv_sec)*1000000+(finishPhase2.tv_usec-startPhase2.tv_usec);

	cout << "Saving centers..." << endl; cout.flush();
	saveCenters(centers, outputFileName);

	double sse = 0;
	if(sseEnabled == 1){
		cout << "Computing SSE... " << endl; cout.flush();
		sse = evaluateSSEFromFile(centers, fileName);
	}
	// PRINTING RESULTS
	cout 	<< 	"Phase1, Phase2, totalRunTime, SSE, num_of_threads, numClusters, numClustersForChunks, chunk_size, fileName" << endl;
	cout 	<< 	fixed 			<< long(phase1TimeMicrosec) 	<< ", " << long(phase2TimeMicrosec)	<< ", " << long(phase1TimeMicrosec + phase2TimeMicrosec) 	<< ", " <<	scientific <<  sse	<< ", "
			<<	num_of_threads 	<< ", " << numClusters 				<< ", " << numClustersForChunks << ", "
			<< 	chunk_size 		<< ", " << fileName << endl;
	delete points;
}

void printUsage(){
	cout << "Usage: KMeans -f fileName [-o outputfileName] [-k num_of_clusters] [-c chunk_size] [-chk num_of_cluster_centers_for_each_chunk] [-t max_threads] [-sse 1 (compute SSE)]" << endl;
	exit(0);
}

void loadParameters(int argc, char** argv, int &num_of_threads, int &numClusters,
		int &numClustersForChunks, int& chunk_size, char* &fileName, int &sseEnabled, char* &outputFileName){
	//DEFAULT VALUES
	numClustersForChunks =5;
	numClusters = 5;
	num_of_threads = 2;
	chunk_size = 10000;
	outputFileName = "output.txt";
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
		}else if (! strcmp(argv[i] , "-o")) {
			outputFileName = argv[i+1];
		}else if (! strcmp(argv[i] , "-sse")) {
			sseEnabled = atoi(argv[i+1]);
		}
		i++;
	}
	cout << "num_of_threads: " << num_of_threads << "\tnumClusters: " << numClusters <<
			"\nnumClustersForChunks: " << numClustersForChunks << "chunk_size: " << chunk_size << endl;
}
