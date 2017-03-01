/*
 * stub.cpp
 *
 *  Created on: Feb 21, 2017
 *      Author: red
 */

// this is just stub program for testing autotest

#include<iostream>
#include "goldchase.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Map.h"
#include <cstring>

using namespace std;

vector<vector< char > > readMapFromFile(char * mapFile, int &golds){
  vector<vector< char > > mapVector;
  vector< char > temp;
  string line;
  char c;
  ifstream mapStream(mapFile);
  mapStream >>golds;
  mapStream.get(c);

  while(getline(mapStream,line))
  {
     for(int i=0; i < line.length(); i++){
       temp.push_back(line[i]);
     }
     mapVector.push_back(temp);
     temp.clear();
  }
  //cout<<"ve size "<<mapVector.size()<<" col "<<mapVector[0].size()<<endl;;
  return mapVector;
}


int main(){

    char * file_name = "mymap_test.txt";
    int fd1, golds;
    char buf [100];
    vector<vector< char > > mapVector =readMapFromFile(file_name, golds) ;

    /*
    cout<<"golds "<<golds<<endl;

    for(unsigned i=0;i<mapVector.size();i++){
      for(unsigned j=0;j<mapVector[i].size();j++){
        cout<<mapVector[i][j];
      }
      cout<<endl;
    }
    */
	 unsigned char  map_mem[5] = "****";

   Map gameMap(reinterpret_cast<const unsigned char*>(map_mem),2,2);

   /*
   gameMap.getKey();
	 gameMap.drawMap();


	 gameMap.getKey();
	 gameMap.drawMap();

	 gameMap.getKey();
	 gameMap.postNotice("");
	 gameMap.drawMap();
   */


	 //char a;
	//cin>>a;
	//cout<<a;

	 //commenting out as goldchase should not use standard output
	 //std::cout<<"out in main";



	return 0;
}
