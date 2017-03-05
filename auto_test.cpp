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
	int children_count;
	int rows;
	int cols;
	int noticeCount;
	int drawMapCount;
	int p_to_c1[2];
	int c1_to_p[2];

	int p_to_c2[2];
	int c2_to_p[2];

	pid_t parent_pid;
	pid_t child_1_pid;
	pid_t child_2_pid;
	char * map_mem;

public:
	BaseAutoTest(int, int, char*, int);
	virtual ~BaseAutoTest();
	virtual void doTest();
	void setUpTestEnv();
	void cleanUpTestEnv();

};
BaseAutoTest::BaseAutoTest(int r,int c,char *m, int child):rows(r), cols(c), map_mem(m), children_count(child){
	noticeCount=0;
	drawMapCount=0;
	parent_pid = -1;
	child_1_pid = -1;
	child_2_pid = -1;
	setUpTestEnv();
}

BaseAutoTest::~BaseAutoTest(){
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

			if(children_count == 1){ // children_count ==1

				cout<<"in parent process : parent pid :"<<parent_pid<<" child1 pid : "<<child_1_pid<<endl;

				// closing useless pipe ends
				close(p_to_c1[P_to_C1_READ_END]);
				close(c1_to_p[C1_to_P_WRITE_END]);

				read(c1_to_p[C1_to_P_READ_END],&rows, sizeof(int));
				read(c1_to_p[C1_to_P_READ_END],&cols,sizeof(int));
				char map [rows* cols +1 ];
				read(c1_to_p[C1_to_P_READ_END],map,rows*cols);
				cout<<"r "<<rows<<"c "<<cols<<endl;
				cout<<"map "<<map<<endl;

		}
		else{ // children_count ==2
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




}


class TestMapInit:public BaseAutoTest{
public:
	TestMapInit(int r, int c, char* m, int ch):BaseAutoTest(r,c,m,ch){

	}
	virtual void doTest();
};

void TestMapInit::doTest(){

	cout<<"TestMapInit Success"<<endl;
	return;
}

class TestOnlyRightKey:public BaseAutoTest{
public:
	TestOnlyRightKey(int r, int c, char* m, int ch):BaseAutoTest(r,c,m,ch){

	}
	virtual void doTest();
};

void TestOnlyRightKey::doTest(){

	cout<<"TestOnlyRightKey Success"<<endl;
	return;
}
int main(){
	TestMapInit b(1,1,NULL, 1);
	b.doTest();
	b.cleanUpTestEnv();


}
