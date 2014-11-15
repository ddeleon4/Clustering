//*****************************************************************************
//Writen by: Daniel De Leon
//	implements clustering using loyds method through the kmeans fucntion
//	and average-linkage clustering thtough the average function
//
//	need to pass in the data file's full name, e.g. data1.txt
//	need to pass in the number of clusters desiered
//	need to pass in which fucntion to run [kmeans|average]
//
//	*NOTE: the printout for the time output has been commented out
//		to re-enable remove the '//' from lines 23, 311, 312, 336,
//		337, and 339
//*****************************************************************************

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <ctime>
//#include <sys/time.h>
using namespace std;

static int dimension = 0;
static int numPoints = 0;
vector<double> data;//all the coordinates in sequential order
vector<int> finalkCluster;//each position shows in which cluster the point in
vector<int> finalaCluster;//that position of the data is

//*****************************************************************************
//printClustermap
//	a vector is passed in containing point cluster locations
//	prints a '[' then the cluster the points are in separated by ','
//		and ending with a ']'
//*****************************************************************************
void printClustermap(vector<int> Clusters){
	cout << "[" << Clusters[0];
        for (int i = 1; i < numPoints; i++){
                cout << " " << Clusters[i];
        }
        cout << "]" << endl;
}


//*****************************************************************************
//readData
//	string containing the name of the data file is passed in
//
//	calculates the number of points and stores that in 'numPoints'
//	calculates the number of dimensions and stores that in 'dimension'
//	also pushes all the coordinates onto the 'data' vector in squential
//		 order.
//*****************************************************************************
void readData(string argv){
	ifstream file;
	file.open(argv.c_str());
	string temp = "";
	int tempNumPoint = 0;
	double test = 0;

	//just getting the total number of lines
	getline(file, temp);
	while(!file.eof() && (isdigit(temp[0]))){
		numPoints++;
		getline(file, temp);
	}
	
	//need to close and re-open to refill the buffer	
	file.close();
	file.open(argv.c_str());

	//pushes all the coordinates on the data vector inorder. Then,
	//calculates the total number of points
	file >> test;
	while(!file.eof()){
		data.push_back(test);
		dimension++;
		file >> test;
	}
	//using the total number of points stored in 'dimension', the actual
	//dimensions can be calculeted by dividing by the number of line in
	//the file a.k.a. the total number of points
	dimension = dimension / numPoints;
}

//*****************************************************************************
//kmeans
//      int containing the total number of clusters(k) there will be is passed
//
//      gets k random points form the data set and clusters all the data around
//      	them.
//      recalculates the centers and recalculates which points whould go where
//*****************************************************************************
void kmeans(int k){
	double kmeansValue = 1000000;
//the loop that dictates how many times it will run while searching for the 
//	lowest kmeans value
//in this case it runs 100 times
for(int progCounter = 0; progCounter < 100; progCounter++){
	//initializations
	vector< vector<double> > centers;
	vector<int> random;
	vector<int> kClusters;
	bool centerChange = false;

	//cluster size initialization
	for (int i = 0; i < numPoints; i++){
		kClusters.push_back(-1);
	}

	//get k random numbers
	for (int i = 0; i < k; i++){
		bool check = true;
		int temprand = 0;
		
		//this is done to make sure that numbers, which will later
		//	relate to points, don't repeat (its bad if multiple 
		//	clusters have the same center)
		do{
			temprand = rand() % numPoints;
			check = true;
			for (int j = 0; j < i; j++){
				if (temprand == random[j])
					check = false;
			}
		}while(!check);
		random.push_back(temprand);
	}

	//pick k random points from data using (random % numpoints) * dimension
	for (int i = 0; i < k; i++){
		vector<double> temp;
		// find the random'th element in the data set
		for (int j = 0; j < dimension; j++){
			temp.push_back(data[(random[i] * dimension) + j]);
		}
		//push onto centers vector
		centers.push_back(temp);
	}
	
	//bool to later check if the center has change during this run through
	centerChange = false;
	do{
		//get points, calculate distance, keep track of shortest
			//use kcluster to mark if a point is in the 
		//goes through every point(i) and cluster(j)
		for (int i = 0; i < numPoints; i++){
			int shortCluster = -1;
			double shortDistance = 1000;
			for (int j = 0; j < k; j++){
				double curDis = 0;
				//goes through every dimension
				for (int d = 0; d < dimension; d++){
					curDis += pow((data[(i*dimension)+d] -
						centers[j][d]),2);
				}
	
				curDis = sqrt(curDis);
				//checks if the current distance is the shortest of
				//	the clusters
				if (curDis < shortDistance){
					shortDistance = curDis;
					shortCluster = j;
				}
			}
			kClusters[i] = shortCluster;
		}

		//get averages for each cluster and update accordingly
		centerChange = false;
		for (int i = 0; i < k; i++){
			//initialze the total array
			double total[dimension];
			for (int j = 0; j < dimension; j++){
				total[j] = 0;
			}

			//goes through every cluster's items adding up the repective
			//	total and keeping count of how many items are in the
			//	cluster
			int items = 0;
			for (int j = 0; j < numPoints; j++){
				if (i == kClusters[j]){
					for (int d = 0; d < dimension; d++){
						total[d] += data[(j*dimension) + d];
						items++;
					}
				}
			}
			//adjusts items for number of dimensions
			items = items/dimension;

			//determines the average and changes it if it is different
			//	from original. if it changes then updates the
			//	centercheck boolean accordingly
			double finalcoord[dimension];
			for (int j = 0; j < dimension; j++){
				finalcoord[j] = total[j] / items;
				if (finalcoord[j] != centers[i][j]){
					centers[i][j] = finalcoord[j];
					centerChange = true;
				}
			}
		}
	//repeat untill no change in any cluster center
	}while(centerChange);
	//calculate k-means, keep track of lowest kmeans and associated cluster
	double curDis = 0;
	for (int i = 0; i < numPoints; i++){
		for (int j = 0; j < k; j++){
			//goes through every point(i) and cluster(j)
			curDis = 0;
                        for (int d = 0; d < dimension; d++){
	                        curDis += pow((data[(i*dimension)+d] -
					centers[j][d]),2);
                        }

                }
        }
	//used to keep track of lowest kmeans value and associated cluster
	if (curDis < kmeansValue){
        	kmeansValue = curDis;
		finalkCluster = kClusters;
        }
	//repeat 100 times
}
}

//*****************************************************************************
//average
//	get the number of cluster k that are desired at the end
//
//	in a while loop that runs untill there are only k clusters, it goes
//	through all possible cluster combinations and finds the shortest
//	distance pair. it then combines the two cluster, eliminating the old
//	one.
//	once k clusters are reached, it reformats the vector used in the
//	clustering into a 1d vector for the print function to be able to use
//*****************************************************************************
void average(int k){
	//initialze vector of clusters, positions represent points and data is
	//	what cluster it is in
	vector< vector<int> > aCluster;
	for (int i = 0; i < numPoints; i++){
		vector<int> temp;
		temp.push_back(i);
		aCluster.push_back(temp);
	}
	// continue to do this untill only k clusters remian
	while(k < aCluster.size()){
		double minDistance = 1000000;
		int a = -1;
		int b = -1;
		
		//used to find distances of shortest pair
		//in order to go through all possible pair combinations we
		//	goes through all clusters (i)
		// 	then all cluster headed ahead of the i'th position (j)
		//	then go through every element in cluster i (l)
		//	then go through every element in cluster j (m)
		//	then through each dimension and preform the calculations
		//then keep track of the shorted distance and the cluster pairs
		//	associated with it
		for (int i = 0; i < aCluster.size(); i++){
			for (int j = i + 1; j < aCluster.size(); j++){
				double total = 0;
				for (int l = 0; l < aCluster[i].size(); l++){
					for (int m = 0; m < aCluster[j].size(); m++){
						double total2 = 0;
						for (int d = 0; d < dimension; d++){
							total2 += pow(data[(aCluster[i][l]*dimension)+d] -
								 data[(aCluster[j][m]*dimension)+d],2);
						}
					total += sqrt(total2);
					}
				}
				if (total < minDistance){
					minDistance = total;
					a = i;
					b = j;
				}
			}
		}
		//combine clusters
		//copies items from one cluster to another while poping each item after
		//	it is read into it's destination cluster
		while(aCluster[b].size() > 0){
			aCluster[a].push_back(aCluster[b].at(aCluster[b].size() -1));
			aCluster[b].pop_back();
		}
		//remove empty cluster
		aCluster.erase(aCluster.begin() + b);
	}
	//make into cluster map
	//first need to initialize final vector
	for (int i = 0; i < numPoints; i++){
		finalaCluster.push_back(0);
	}
	//then we need to change the values to match the correct cluster location
	for (int j = 0; j < aCluster.size(); j++){
		for (int l = 0; l < aCluster[j].size(); l++){
			finalaCluster[aCluster[j][l]] = j;
		}
	}
}

int main (int argc, const char* argv[]){
	//take time
	//struct timeval t1, t2;
	//gettimeofday(&t1, NULL);

	readData(argv[1]);
	string kstr = "kmeans";
	string astr = "average";
	string prog = argv[3];

	srand(time(NULL));

	stringstream strValue;
	strValue << argv[2];
	int k = 0;
	strValue >> k;
	
	if (prog == kstr){
		kmeans(k);
		printClustermap(finalkCluster);
	}
	else if (prog == astr){
		average(k);
		printClustermap(finalaCluster);
	}
	else cout << "incorrect usage" << endl;

	//gettimeofday(&t2, NULL);
	//int milliseconds = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec)/1000;

	//cout << "Run time is " << milliseconds << " milliseconds" << endl;	

	return 0;
}
