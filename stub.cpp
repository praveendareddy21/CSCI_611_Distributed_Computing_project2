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

int main(){

	 //unsigned char  map_mem[5] = "****";
	 //Map gameMap(reinterpret_cast<const unsigned char*>(map_mem),2,2);
	char a;
	cin>>a;
	cout<<a;

	 //commenting out as goldchase should not use standard output
	 //std::cout<<"out in main";



	return 0;
}


