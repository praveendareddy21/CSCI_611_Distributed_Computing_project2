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
public:
	int children_count;
	int rows;
	int cols;
	int p1_noticeCount;
	int p1_drawMapCount;
	int p2_noticeCount;
	int p2_drawMapCount;

	int p_to_c1[2];
	int c1_to_p[2];

	int p_to_c2[2];
	int c2_to_p[2];

	pid_t parent_pid;
	pid_t child_1_pid;
	pid_t child_2_pid;
	char * map_mem;
	char mapfile_content[200];
	char p1_initial_map[200];
	char p2_initial_map[200];

	string mapfile_cont;


	BaseAutoTest(int, int, string, int);
	virtual ~BaseAutoTest();
	virtual void doTest();
	void setUpTestEnv();
	void cleanUpTestEnv();

};
BaseAutoTest::BaseAutoTest(int r,int c,string m, int child):rows(r), cols(c), mapfile_cont(m), children_count(child){
	p1_noticeCount=0;
	p1_drawMapCount=0;
	p2_noticeCount=0;
	p2_drawMapCount=0;
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
	cout<<"Cleaned up Test Environment"<<endl<<	endl;
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

			//execl("stub","",NULL);
			execl("test_main","",NULL);
		}
		else{ // parent process
			child_1_pid = temp_pid;
			//char * testMap ="2\n****\n*  *\n** *";

			strcpy(mapfile_content, mapfile_cont.c_str() );

			char * file_name = "mymap.txt";

			mkfifo(file_name, S_IRUSR | S_IWUSR);
			int fd = open(file_name, O_WRONLY );

			write(fd, mapfile_content, strlen(mapfile_content));
			close(fd);
			unlink(file_name);

			if(children_count == 1){ // children_count ==1

				cout<<"in parent process : parent pid :"<<parent_pid<<" child1 pid : "<<child_1_pid<<endl;

				// closing useless pipe ends
				close(p_to_c1[P_to_C1_READ_END]);
				close(c1_to_p[C1_to_P_WRITE_END]);

				read(c1_to_p[C1_to_P_READ_END],&rows, sizeof(int));
				read(c1_to_p[C1_to_P_READ_END],&cols,sizeof(int));

				read(c1_to_p[C1_to_P_READ_END],p1_initial_map,rows*cols + 1);
				//cout<<"r "<<rows<<"c "<<cols<<"map length "<<strlen(p1_initial_map)<<endl;
				//cout<<"map "<<endl;
				//printf("map : %s\n", p1_initial_map);


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

					//execl("stub","",NULL);
					execl("test_main","",NULL);
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
			read(c1_to_p[C1_to_P_READ_END],p1_initial_map,rows*cols+1);

			//cout<<"in c1 r "<<rows<<"c "<<cols<<endl;
			//cout<<"map "<<endl;
			//printf("%s\n", p1_initial_map);

			read(c2_to_p[C2_to_P_READ_END],&rows, sizeof(int));
			read(c2_to_p[C2_to_P_READ_END],&cols,sizeof(int));
			read(c2_to_p[C2_to_P_READ_END],p2_initial_map,rows*cols+1);

			//cout<<"in c2  r "<<rows<<"c "<<cols<<endl;
			//cout<<"map "<<endl;
			//printf("%s\n", p2_initial_map);
		}


		}




}



class TestPlayerCantMoveIntoWall:public BaseAutoTest{
public:
	TestPlayerCantMoveIntoWall(int r, int c, string m, int ch):BaseAutoTest(r,c,m,ch){

	}
	virtual void doTest();
};

void TestPlayerCantMoveIntoWall::doTest(){

	char key = 'l';
	write(p_to_c1[P_to_C1_WRITE_END],&key,sizeof(char));

	char msgType,  map[rows*cols+1];
	map[rows*cols] = '\0';
	read(c1_to_p[C1_to_P_READ_END],&msgType, sizeof(char));
	read(c1_to_p[C1_to_P_READ_END],&p1_noticeCount, sizeof(int));
	read(c1_to_p[C1_to_P_READ_END],&p1_drawMapCount,sizeof(int));
	read(c1_to_p[C1_to_P_READ_END],map,rows*cols);

	cout<<"msgtype : "<<msgType<<" notice : "<<p1_noticeCount<<" drawcont : "<<p1_drawMapCount<<endl;

	key = 'Q';
	write(p_to_c1[P_to_C1_WRITE_END],&key,sizeof(char));


	// check for map didn't change after attempting move into wall
	bool all_equal = true;
	for(int i=0; i < rows * cols; i++){
		if(map[i] != p1_initial_map[i])
			all_equal = false;
	}

	if(all_equal)
		cout<<"TestPlayerCantMoveIntoWall Success"<<endl;
	else
		cout<<"TestPlayerCantMoveIntoWall Success"<<endl;

	return;
}


class TestPlayerCanMoveToEmpty:public BaseAutoTest{
public:
	TestPlayerCanMoveToEmpty(int r, int c, string m, int ch):BaseAutoTest(r,c,m,ch){

	}
	virtual void doTest();
};

void TestPlayerCanMoveToEmpty::doTest(){
	bool test_result = false;

	if(p1_initial_map[4] & G_PLR0){ // player on 4
		char key = 'l';
		write(p_to_c1[P_to_C1_WRITE_END],&key,sizeof(char));

		char msgType,  map[rows*cols+1];
		map[rows*cols] = '\0';
		read(c1_to_p[C1_to_P_READ_END],&msgType, sizeof(char));
		read(c1_to_p[C1_to_P_READ_END],&p1_noticeCount, sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],&p1_drawMapCount,sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],map,rows*cols);

		// check for Player moved to empty space on right
		if(map[5] & G_PLR0 && p1_drawMapCount == 1) //&& !(map[4] & G_PLR0))
		{
			test_result =  true;
			cout<<"in 4"<<endl;
		}

	}
	else if(p1_initial_map[5] & G_PLR0){ // player on 5
		char key = 'h';
		write(p_to_c1[P_to_C1_WRITE_END],&key,sizeof(char));

		char msgType,  map[rows*cols+1];
		map[rows*cols] = '\0';
		read(c1_to_p[C1_to_P_READ_END],&msgType, sizeof(char));
		read(c1_to_p[C1_to_P_READ_END],&p1_noticeCount, sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],&p1_drawMapCount,sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],map,rows*cols);

		// check for Player moved to empty space on left
		if(map[4] & G_PLR0 && p1_drawMapCount == 1 ) //
		{
			test_result =  true;
			cout<<"in 5"<<endl;
		}

	}

	// Testing complete, sending Q to quit GoldChase game process
	char key = 'Q';
	write(p_to_c1[P_to_C1_WRITE_END],&key,sizeof(char));

	if(test_result)
		cout<<"TestPlayerCanMoveToEmpty Success"<<endl;
	else
		cout<<"TestPlayerCanMoveToEmpty Failed"<<endl;

	return;
}

class TestSimlpleGamePlayWithGold:public BaseAutoTest{
public:
	TestSimlpleGamePlayWithGold(int r, int c, string m, int ch):BaseAutoTest(r,c,m,ch){

	}
	virtual void doTest();
};

void TestSimlpleGamePlayWithGold::doTest(){
	bool test_result_0 = false;
	bool test_result_1 = false;
	bool test_result_2 = false;

	if(p1_initial_map[1] & G_PLR0){ // player on 1
		char key = 'k'; // key for moving up
		char msgType,  map[rows*cols+1];
		map[rows*cols] = '\0';

		write(p_to_c1[P_to_C1_WRITE_END],&key,sizeof(char));

		read(c1_to_p[C1_to_P_READ_END],&msgType, sizeof(char));
		read(c1_to_p[C1_to_P_READ_END],&p1_noticeCount, sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],&p1_drawMapCount,sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],map,rows*cols);
		cout<<"msgtype : "<<msgType<<" notice : "<<p1_noticeCount<<" drawcont : "<<p1_drawMapCount<<endl;

		//Player cannot leave map without gold
		//drawMap has increased by 1
		if( map[1] & G_PLR0 && p1_drawMapCount == 1 ) //
		{
			test_result_0 = true;
		}

		key = 'j';
		write(p_to_c1[P_to_C1_WRITE_END],&key,sizeof(char));

		read(c1_to_p[C1_to_P_READ_END],&msgType, sizeof(char));
		read(c1_to_p[C1_to_P_READ_END],&p1_noticeCount, sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],&p1_drawMapCount,sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],map,rows*cols);
		cout<<"msgtype : "<<msgType<<" notice : "<<p1_noticeCount<<" drawcont : "<<p1_drawMapCount<<endl;

		// Player moved on to the Gold
		//drawMap has increased by 1
		// G_GOLD disappears
		//notice has increased by 1
		if( map[4] & G_PLR0 && p1_drawMapCount == 2 && !(map[4] & G_GOLD) && p1_noticeCount == 1)
		{
			test_result_1 = true;
		}

		key = 'j';
		write(p_to_c1[P_to_C1_WRITE_END],&key,sizeof(char));

		read(c1_to_p[C1_to_P_READ_END],&msgType, sizeof(char));
		read(c1_to_p[C1_to_P_READ_END],&p1_noticeCount, sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],&p1_drawMapCount,sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],map,rows*cols);
		cout<<"msgtype : "<<msgType<<" notice : "<<p1_noticeCount<<" drawcont : "<<p1_drawMapCount<<endl;


		//Player leaves map with gold
		// G_GOLD disappears
		// notice has increased by 1
		if( p1_drawMapCount == 3 && !(map[4] & G_GOLD) && p1_noticeCount == 2) //&& !(map[5] & G_PLR0))
		{
			test_result_2 = true;
			cout<<"in 1"<<endl;
		}

	}
	else if(p1_initial_map[4] & G_PLR0){ // player on 4
		char key = 'j';  // key for moving down
		char msgType,  map[rows*cols+1];
		map[rows*cols] = '\0';

		write(p_to_c1[P_to_C1_WRITE_END],&key,sizeof(char));

		read(c1_to_p[C1_to_P_READ_END],&msgType, sizeof(char));
		read(c1_to_p[C1_to_P_READ_END],&p1_noticeCount, sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],&p1_drawMapCount,sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],map,rows*cols);
		cout<<"msgtype : "<<msgType<<" notice : "<<p1_noticeCount<<" drawcont : "<<p1_drawMapCount<<endl;

		//Player cannot leave map without gold
		//drawMap has increased by 1
		if( map[4] & G_PLR0 && p1_drawMapCount == 1 ) //
		{
				test_result_0 = true;
		}

		key = 'k';
		write(p_to_c1[P_to_C1_WRITE_END],&key,sizeof(char));

		read(c1_to_p[C1_to_P_READ_END],&msgType, sizeof(char));
		read(c1_to_p[C1_to_P_READ_END],&p1_noticeCount, sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],&p1_drawMapCount,sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],map,rows*cols);
		cout<<"msgtype : "<<msgType<<" notice : "<<p1_noticeCount<<" drawcont : "<<p1_drawMapCount<<endl;

		// Player moved on to the Gold
		//drawMap has increased by 1
		// G_GOLD disappears
		//notice has increased by 1
		if( map[1] & G_PLR0 && p1_drawMapCount == 2 && !(map[1] & G_GOLD) && p1_noticeCount == 1 ) //&& !(map[5] & G_PLR0))
		{
				test_result_1 = true;
		}

		key = 'k';
		write(p_to_c1[P_to_C1_WRITE_END],&key,sizeof(char));

		read(c1_to_p[C1_to_P_READ_END],&msgType, sizeof(char));
		read(c1_to_p[C1_to_P_READ_END],&p1_noticeCount, sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],&p1_drawMapCount,sizeof(int));
		read(c1_to_p[C1_to_P_READ_END],map,rows*cols);
		cout<<"msgtype : "<<msgType<<" notice : "<<p1_noticeCount<<" drawcont : "<<p1_drawMapCount<<endl;

		//Player leaves map with gold
		// G_GOLD disappears
		// notice has increased by 1
		if( p1_drawMapCount == 3 && !(map[4] & G_GOLD) && p1_noticeCount == 2) //&& !(map[5] & G_PLR0))
		{
			test_result_2 = true;
			cout<<"in 4"<<endl;
		}

	}


	if(test_result_0 && test_result_1 && test_result_2 )
		cout<<"TestSimlpleGamePlayWithGold Success"<<endl;
	else
		cout<<"TestSimlpleGamePlayWithGold Failed"<<endl;

	return;
}

class TestTwoPlayersOnMap:public BaseAutoTest{
public:
	TestTwoPlayersOnMap(int r, int c, string m, int ch):BaseAutoTest(r,c,m,ch){

	}
	virtual void doTest();
};

void TestTwoPlayersOnMap::doTest(){
	bool test_result_0 = false;
	bool test_result_1 = false;
	char key = 'l';
	char msgType,  map[rows*cols+1];
	map[rows*cols] = '\0';

	if(p1_initial_map[1] & G_PLR0 )// && p2_initial_map[4] & G_PLR1 ){
	{
			cout<<"-- 1 and 4"<<endl;

			key = 'j';
			write(p_to_c1[P_to_C1_WRITE_END],&key,sizeof(char));

			read(c1_to_p[C1_to_P_READ_END],&msgType, sizeof(char));
			read(c1_to_p[C1_to_P_READ_END],&p1_noticeCount, sizeof(int));
			read(c1_to_p[C1_to_P_READ_END],&p1_drawMapCount,sizeof(int));
			read(c1_to_p[C1_to_P_READ_END],map,rows*cols);
			cout<<"msgtype : "<<msgType<<" notice : "<<p1_noticeCount<<" drawcont : "<<p1_drawMapCount<<endl;

			//Change of map, both G_PLR0 & G_PLR1 bits set on a square
			if(map[4] & G_PLR0  && map[4] & G_PLR1 ){
				test_result_0 = true;
			}

			key = 'k';
			write(p_to_c2[P_to_C2_WRITE_END],&key,sizeof(char));

			read(c2_to_p[C2_to_P_READ_END],&msgType, sizeof(char));
			read(c2_to_p[C2_to_P_READ_END],&p2_noticeCount, sizeof(int));
			read(c2_to_p[C2_to_P_READ_END],&p2_drawMapCount,sizeof(int));
			read(c2_to_p[C2_to_P_READ_END],map,rows*cols);
			cout<<"msgtype : "<<msgType<<" notice : "<<p2_noticeCount<<" drawcont : "<<p2_drawMapCount<<endl;

			//Move off: Bits successfully separated onto two different squares
			if(map[4] & G_PLR0  && map[1] & G_PLR1 ){
				test_result_1 = true;
			}

	}
	else if(p1_initial_map[4] & G_PLR0 ) //&& p2_initial_map[1] & G_PLR1 ){
	{
			cout<<"-- 4 and 1"<<endl;

			key = 'k';
			write(p_to_c1[P_to_C1_WRITE_END],&key,sizeof(char));

			read(c1_to_p[C1_to_P_READ_END],&msgType, sizeof(char));
			read(c1_to_p[C1_to_P_READ_END],&p1_noticeCount, sizeof(int));
			read(c1_to_p[C1_to_P_READ_END],&p1_drawMapCount,sizeof(int));
			read(c1_to_p[C1_to_P_READ_END],map,rows*cols);
			cout<<"msgtype : "<<msgType<<" notice : "<<p1_noticeCount<<" drawcont : "<<p1_drawMapCount<<endl;

			//Change of map, both G_PLR0 & G_PLR1 bits set on a square
			if(map[1] & G_PLR0  && map[1] & G_PLR1 ){
				test_result_0 = true;
			}
			key = 'j';
			write(p_to_c2[P_to_C2_WRITE_END],&key,sizeof(char));

			read(c2_to_p[C2_to_P_READ_END],&msgType, sizeof(char));
			read(c2_to_p[C2_to_P_READ_END],&p2_noticeCount, sizeof(int));
			read(c2_to_p[C2_to_P_READ_END],&p2_drawMapCount,sizeof(int));
			read(c2_to_p[C2_to_P_READ_END],map,rows*cols);
			cout<<"msgtype : "<<msgType<<" notice : "<<p2_noticeCount<<" drawcont : "<<p2_drawMapCount<<endl;

			//Move off: Bits successfully separated onto two different squares
			if(map[1] & G_PLR0  && map[4] & G_PLR1 ){
				test_result_1 = true;
			}

	}


	key = 'Q';
	write(p_to_c1[P_to_C1_WRITE_END],&key,sizeof(char));

	key = 'Q';
	write(p_to_c2[P_to_C2_WRITE_END],&key,sizeof(char));


	if(test_result_0 && test_result_1)
		cout<<"TestTwoPlayersOnMap Success"<<endl;
	else
		cout<<"TestTwoPlayersOnMap Failed"<<endl;

	return;
}

int main(){

	TestPlayerCantMoveIntoWall t1(3, 3, "0\n***\n* *\n***", 1);
	t1.doTest();
	t1.cleanUpTestEnv();

	TestPlayerCanMoveToEmpty t2(3, 3, "0\n***\n*  \n***", 1);
	t2.doTest();
	t2.cleanUpTestEnv();

	TestSimlpleGamePlayWithGold t3_4_5(2, 3, "1\n* *\n* *", 1);
	t3_4_5.doTest();
	t3_4_5.cleanUpTestEnv();

	TestTwoPlayersOnMap t6_7(2, 3, "0\n* *\n* *", 2);
	t6_7.doTest();
	t6_7.cleanUpTestEnv();

	return 0;

}
