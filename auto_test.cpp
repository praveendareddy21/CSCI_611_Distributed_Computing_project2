/*
 * auto_test.cpp
 *
 *  Created on: Feb 21, 2017
 *      Author: red
 */


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

#define P2C_READ_END 0
#define P2C_WRITE_END 1

#define C2P_READ_END 0
#define C2P_WRITE_END 1


class BaseAutoTest{
private:
	int rows;
	int cols;
	char * map;


	int noticeCount;
	int drawMapCount;
	int p2c[2];
	int c2p[2];
	pid_t parent_pid;
	pid_t child_pid;

public:
	BaseAutoTest(int, int, char*);
	virtual ~BaseAutoTest();
	virtual void doTest();
	void setUpTestEnv();
	void cleanUpTestEnv();

};
BaseAutoTest::BaseAutoTest(int r,int c,char *m):rows(r),cols(c),map(m){
	noticeCount=0;
	drawMapCount=0;
	parent_pid = -1;
	child_pid = -1;
	setUpTestEnv();
}

BaseAutoTest::~BaseAutoTest(){
	cleanUpTestEnv();
}
void BaseAutoTest::doTest(){

}


void BaseAutoTest::cleanUpTestEnv(){
	if(getpid() == child_pid)
	{
		close(p2c[P2C_READ_END]);
		close(c2p[C2P_WRITE_END]);
		//exit(0);
	}

	if(getpid() == parent_pid)
	{
		int status;
		close(p2c[P2C_WRITE_END]);
		close(c2p[C2P_READ_END]);
		if( wait(&status) )
		{

		}
		//exit(0);
	}
	cout<<"Cleaned up Test Environment"<<endl;
}

class TestMapInit:public BaseAutoTest{
	virtual void doTest();
};


void BaseAutoTest::setUpTestEnv(){
	cout<<"Setting up Test Environment"<<endl;
	pipe(p2c);
	pipe(c2p);
	int temp_pid;
	parent_pid = getpid();

	temp_pid = fork();
		if(temp_pid > 0 ){ // parent process

			child_pid = temp_pid;
			cout<<"in parent process : parent pid :"<<parent_pid<<" child pid : "<<child_pid<<endl;


			char * testMap ="2\n****\n*  *\n** *";
			char * file_name = "mymap_test.txt";

	    mkfifo(file_name, S_IRUSR | S_IWUSR);
	    int fd = open(file_name, O_WRONLY );

	    write(fd, testMap, strlen(testMap));
	    close(fd);
	    unlink(file_name);


			// closing useless pipe ends
			close(p2c[P2C_READ_END]);
			close(c2p[C2P_WRITE_END]);

			//cin>>msg;
			//write(p2c[P2C_WRITE_END],"j",1);

			//char msg2[6];
			read(c2p[C2P_READ_END],&rows, sizeof(int));
			read(c2p[C2P_READ_END],&cols,sizeof(int));

			
			//char map [rows* cols ];
			char  * map;

			read(c2p[C2P_READ_END],map,rows*cols);
			cout<<"r "<<rows<<"c "<<cols<<" map_size "<<strlen(map)<<endl;
			cout<<"map "<<map<<endl;

			for(int i=0; i< rows * cols; i++){
				printf("%c", map[i]);
			}
			printf("\n");

			//printf("%s\n", map);

			char msgType;

		}
		else{ // child process
			child_pid = getpid();
			cout<<"in child process : parent pid :"<<parent_pid<<" child pid : "<<child_pid<<endl;

			// closing useless pipe ends
			close(p2c[P2C_WRITE_END]);
			close(c2p[C2P_READ_END]);

			dup2(p2c[P2C_READ_END], 0);
			close(p2c[P2C_READ_END]);

			dup2(c2p[C2P_WRITE_END], 1);
			close(c2p[C2P_WRITE_END]);

			execl("stub","",NULL);
		}




}

int main(){
	BaseAutoTest b(1,1,NULL);
	//b.setUpTestEnv();
	 //b.cleanUpTestEnv();
}

int main1(){
	int p2c[2],c2p[2]; // file descriptors  for pipes

	pid_t parent_pid, child_pid;

	char  msg [] ="h";

	pipe(p2c);
	pipe(c2p);
	int rows, cols, noticeCount, drawMapCount;

	parent_pid = getpid();

	child_pid = fork();
	if(child_pid > 0 ){ // parent process
		cout<<"in parent process : parent pid :"<<parent_pid<<" child pid : "<<child_pid<<endl;

		// closing useless pipe ends
		close(p2c[P2C_READ_END]);
		close(c2p[C2P_WRITE_END]);

		//cin>>msg;
		//write(p2c[P2C_WRITE_END],"j",1);

		//char msg2[6];
		read(c2p[C2P_READ_END],&rows, sizeof(int));
		read(c2p[C2P_READ_END],&cols,sizeof(int));
		char map [rows* cols +1 ];
		read(c2p[C2P_READ_END],map,rows*cols);
		cout<<"r "<<rows<<"c "<<cols<<endl;
		cout<<"map "<<map<<endl;




		char msgType;

		for(int i=0; i<3;i++){
			write(p2c[P2C_WRITE_END],"l",1);

			read(c2p[C2P_READ_END],&msgType, sizeof(char));
			read(c2p[C2P_READ_END],&noticeCount, sizeof(int));
			read(c2p[C2P_READ_END],&drawMapCount,sizeof(int));
			read(c2p[C2P_READ_END],map,rows*cols);


			cout<<"msgtype : "<<msgType<<" notice : "<<noticeCount<<" drawcont : "<<drawMapCount<<endl;
					cout<<"map "<<map<<endl;

			if(msgType=='n'){
				read(c2p[C2P_READ_END],&msgType, sizeof(char));
				read(c2p[C2P_READ_END],&noticeCount, sizeof(int));
				read(c2p[C2P_READ_END],&drawMapCount,sizeof(int));
				read(c2p[C2P_READ_END],map,rows*cols);


				cout<<"msgtype : "<<msgType<<" notice : "<<noticeCount<<" drawcont : "<<drawMapCount<<endl;
						cout<<"map "<<map<<endl;
			}
		}

		//cout<<"message read in parent as : "<<msg2[0];




	}
	else{ // child process
		child_pid = getpid();
		cout<<"in child process : parent pid :"<<parent_pid<<" child pid : "<<child_pid<<endl;

		// closing useless pipe ends
		close(p2c[P2C_WRITE_END]);
		close(c2p[C2P_READ_END]);

		dup2(p2c[P2C_READ_END], 0);
		close(p2c[P2C_READ_END]);



		dup2(c2p[C2P_WRITE_END], 1);
		close(c2p[C2P_WRITE_END]);

		execl("stub","",NULL);



	}


	if(getpid() == child_pid)
	{
		close(p2c[P2C_READ_END]);
		close(c2p[C2P_WRITE_END]);
		exit(0);
	}

	if(getpid() == parent_pid)
	{
		int status;
		close(p2c[P2C_WRITE_END]);
		close(c2p[C2P_READ_END]);
		if( wait(&status) )
		{

		}
		exit(0);
	}



	cout<<"out";
}
