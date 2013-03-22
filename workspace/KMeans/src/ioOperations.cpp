/*
 * FileOperations.cpp
 *
 *  Created on: Mar 14, 2013
 *      Author: ali
 */
#include "stdafx.h"
using namespace std;
using namespace cv;

int countLines(char* filename){
	ifstream file(filename);
	int number_of_lines = count(istreambuf_iterator<char>(file), istreambuf_iterator<char>(), '\n');
	file.close();
	cout << "there are " << number_of_lines << " lines in " << filename << endl;
	return number_of_lines;
}

Mat loadDatasetAtOnce(char* fileName){
	cout << "loading " << fileName << "...\n";
	vector<string>* lines;
	int datasize = countLines(fileName);

	ifstream file(fileName);
	lines = getChunkLines(file,datasize);
	return vec2Mat( getChunk(*lines) );
}

vector<string>* getChunkLines(istream& str, int chunkSize){
	vector<string>*   lines = new vector<string>;
	lines->reserve(chunkSize);
	for(int i =0; i<chunkSize; i++ ){
		string line;
		getline(str,line);

		lines->push_back(line);
		if(line.size() <1)
			break;
	}
	return lines;
}

vector<float> splitIntoTokens(string line){

	vector<float>   result;

	if(numFeatures > 0) result.reserve(numFeatures);	//performance tuning

	istringstream          lineStream(line);
	string                cell;
	while(getline(lineStream,cell,','))
	{
		result.push_back(atof(cell.c_str()));
	}

	if(numFeatures <= 0) numFeatures = result.size();

	return result;
}

vector<vector<float> > getChunk(vector<string> lines){
	int currentChunkSize = lines.size();
	vector< vector<float> > chunkInstances;
	chunkInstances.reserve(currentChunkSize);
	vector<float>   instance;

	for(int i =0; i<currentChunkSize; i++ ){
		instance = splitIntoTokens(lines[i]);
		if(instance.size() > 0)
			chunkInstances.push_back(instance);
		else
			break;
	}

	return chunkInstances;
}

Mat vec2Mat(const vector< vector<float> > chunkInstances){

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


void saveCenters(Mat centers, char* outputFileName){
	ofstream outfile;
	outfile.open(outputFileName);
	int rows = centers.rows;
	int cols = centers.row(1).cols;
	for(int i=0; i<rows; i++){
		for (int j=0; j<cols; j++){
			outfile << centers.at<float>(i,j);
			if(j != (cols-1))
				outfile << ",";
		}
		outfile << endl;
	}
	outfile.close();
}

template< typename TYPE >
void print(TYPE val)
{
	cout << val << " ";
}
