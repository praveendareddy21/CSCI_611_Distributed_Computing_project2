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

#define P_to_C1_READ_END 0
#define P_to_C1_WRITE_END 1

#define C1_to_P_READ_END 0
#define C1_to_P_WRITE_END 1

#define P_to_C2_READ_END 0
#define P_to_C2_WRITE_END 1

#define C2_to_P_READ_END 0
#define C2_to_P_WRITE_END 1

class BaseAutoTest{
private:
	int rows;
	int cols;
	char * map;


	int noticeCount;
	int drawMapCount;
	int p_to_c1[2];
	int c1_to_p[2];

	int p_to_c2[2];
	int c2_to_p[2];

	pid_t parent_pid;
	pid_t child_1_pid;
	pid_t child_2_pid;

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
	child_1_pid = -1;
	child_2_pid = -1;
	setUpTestEnv();
}

BaseAutoTest::~BaseAutoTest(){
	cleanUpTestEnv();
}
void BaseAutoTest::doTest(){

}


void BaseAutoTest::cleanUpTestEnv(){

	if(getpid() == parent_pid)
	{
		int status;
		close(p_to_c1[P_to_C1_WRITE_END]);
		close(c1_to_p[C1_to_P_READ_END]);
		close(p_to_c2[P_to_C2_WRITE_END]);
		close(c2_to_p[C2_to_P_READ_END]);
		if( wait(&status) )
		{

		}
		//exit(0);
	}
	cout<<"Cleaned up Test Environment"<<endl;
}




void BaseAutoTest::setUpTestEnv(){
	cout<<"Setting up Test Environment"<<endl;
	pipe(p_to_c1);
	pipe(c1_to_p);
	pipe(p_to_c2);
	pipe(c2_to_p);

	int temp_pid;
	parent_pid = getpid();

	temp_pid = fork();
		if(temp_pid == 0 ){ // child process
			child_1_pid = getpid();
			cout<<"in child process : parent pid :"<<parent_pid<<" child1 pid : "<<child_1_pid<<endl;

			// closing useless pipe ends
			close(p_to_c1[P_to_C1_WRITE_END]);
			close(c1_to_p[C1_to_P_READ_END]);

			dup2(p_to_c1[P_to_C1_READ_END], 0);
			close(p_to_c1[P_to_C1_READ_END]);
			dup2(c1_to_p[C1_to_P_WRITE_END], 1);
			close(c1_to_p[C1_to_P_WRITE_END]);

			execl("stub","",NULL);
		}
		else{ // parent process
			child_1_pid = temp_pid;

			temp_pid = fork();
				if(temp_pid == 0 ){// child 2
					child_2_pid = getpid();
					cout<<"in child process : parent pid :"<<parent_pid<<" child2 pid : "<<child_2_pid<<endl;

					// closing useless pipe ends
					close(p_to_c2[P_to_C2_WRITE_END]);
					close(c2_to_p[C2_to_P_READ_END]);

					dup2(p_to_c2[P_to_C2_READ_END], 0);
					close(p_to_c2[P_to_C2_READ_END]);

					dup2(c2_to_p[C2_to_P_WRITE_END], 1);
					close(c2_to_p[C2_to_P_WRITE_END]);

					execl("stub","",NULL);
				}


			child_2_pid = temp_pid;
			cout<<"in parent process : parent pid :"<<parent_pid<<" child1 pid : "<<child_1_pid<<" child2 pid : "<<child_2_pid<<endl;

			// closing useless pipe ends
			close(p_to_c1[P_to_C1_READ_END]);
			close(c1_to_p[C1_to_P_WRITE_END]);

			close(p_to_c2[P_to_C2_READ_END]);
			close(c2_to_p[C2_to_P_WRITE_END]);


			read(c1_to_p[C1_to_P_READ_END],&rows, sizeof(int));
			read(c1_to_p[C1_to_P_READ_END],&cols,sizeof(int));
			char map [rows* cols +1 ];
			read(c1_to_p[C1_to_P_READ_END],map,rows*cols);
			cout<<"r "<<rows<<"c "<<cols<<endl;
			cout<<"map "<<map<<endl;\

			read(c2_to_p[C2_to_P_READ_END],&rows, sizeof(int));
			read(c2_to_p[C2_to_P_READ_END],&cols,sizeof(int));
			char map2 [rows* cols +1 ];
			read(c2_to_p[C2_to_P_READ_END],map2,rows*cols);
			cout<<"r "<<rows<<"c "<<cols<<endl;
			cout<<"map2 "<<map2<<endl;



		}




}


class TestMapInit:public BaseAutoTest{
public:
	TestMapInit(int r, int c, char* m):BaseAutoTest(r,c,m){
		
	}
	virtual void doTest();
};

void TestMapInit::doTest(){

	cout<<"TestMapInit Success"<<endl;
	return;
}
int main(){
	TestMapInit b(1,1,NULL);
	b.doTest();

}

int main1(){
	int p_to_c1[2],c1_to_p[2]; // file descriptors  for pipes

	pid_t parent_pid, child_pid;

	char  msg [] ="h";

	pipe(p_to_c1);
	pipe(c1_to_p);
	int rows, cols, noticeCount, drawMapCount;

	parent_pid = getpid();

	child_pid = fork();
	if(child_pid > 0 ){ // parent process
		cout<<"in parent process : parent pid :"<<parent_pid<<" child pid : "<<child_pid<<endl;

		// closing useless pipe ends
		close(p_to_c1[P_to_C1_READ_END]);
		close(c1_to_p[C1_to_P_WRITE_END]);

		//cin>>msg;
		//write(p_to_c1[P_to_C1_WRITE_END],"j",1);

		//char msg2[6];
		read(c1_to_p[C1_to_P_READ_END],&rows, sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],&cols,sizeof(int));
		char map [rows* cols +1 ];
		read(c1_to_p[C1_to_P_READ_END],map,rows*cols);
		cout<<"r "<<rows<<"c "<<cols<<endl;
		cout<<"map "<<map<<endl;




		char msgType;

		for(int i=0; i<3;i++){
			write(p_to_c1[P_to_C1_WRITE_END],"l",1);

			read(c1_to_p[C1_to_P_READ_END],&msgType, sizeof(char));
			read(c1_to_p[C1_to_P_READ_END],&noticeCount, sizeof(int));
			read(c1_to_p[C1_to_P_READ_END],&drawMapCount,sizeof(int));
			read(c1_to_p[C1_to_P_READ_END],map,rows*cols);


			cout<<"msgtype : "<<msgType<<" notice : "<<noticeCount<<" drawcont : "<<drawMapCount<<endl;
					cout<<"map "<<map<<endl;

			if(msgType=='n'){
				read(c1_to_p[C1_to_P_READ_END],&msgType, sizeof(char));
				read(c1_to_p[C1_to_P_READ_END],&noticeCount, sizeof(int));
				read(c1_to_p[C1_to_P_READ_END],&drawMapCount,sizeof(int));
				read(c1_to_p[C1_to_P_READ_END],map,rows*cols);


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
		close(p_to_c1[P_to_C1_WRITE_END]);
		close(c1_to_p[C1_to_P_READ_END]);

		dup2(p_to_c1[P_to_C1_READ_END], 0);
		close(p_to_c1[P_to_C1_READ_END]);



		dup2(c1_to_p[C1_to_P_WRITE_END], 1);
		close(c1_to_p[C1_to_P_WRITE_END]);

		execl("stub","",NULL);



	}


	if(getpid() == child_pid)
	{
		close(p_to_c1[P_to_C1_READ_END]);
		close(c1_to_p[C1_to_P_WRITE_END]);
		exit(0);
	}

	if(getpid() == parent_pid)
	{
		int status;
		close(p_to_c1[P_to_C1_WRITE_END]);
		close(c1_to_p[C1_to_P_READ_END]);
		if( wait(&status) )
		{

		}
		exit(0);
	}



	cout<<"out";
}
