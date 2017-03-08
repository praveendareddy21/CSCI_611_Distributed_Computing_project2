// Map.cpp, Version 1.2 (11 Mar 2015)
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

#include <cstring>

#include"goldchase.h"
#include"Screen.h"
#include"Map.h"


//Initialize the object and draw the map
Map::Map(const unsigned char* mmem, int ylength, int xwidth)
  : mapHeight(ylength), mapWidth(xwidth), mapmem(mmem)  //, theMap(ylength, xwidth)
{
  noticeCount = 0;
  drawMapCount = 0;
  int rows, cols;
  rows = ylength;
  cols =  xwidth;
	write(1, &rows, sizeof(int));
	write(1, &cols, sizeof(int));
	write(1, mapmem, mapHeight*mapWidth+1);

}

int Map::getKey()
{
  //write(1, &noticeCount, sizeof(int));
	//write(1, &drawMapCount, sizeof(int));
	//write(1, mapmem, mapHeight*mapWidth);

	char key;
	read(0, &key, sizeof(char));
	return key;

}
void Map::postNotice(const char* msg)
{
  noticeCount = noticeCount + 1;

  char msgType = 'n';

  //write(1, &msgType, sizeof(char));
  //write(1, &noticeCount, sizeof(int));
  //write(1, &drawMapCount, sizeof(int));
  //write(1, mapmem, mapHeight*mapWidth);

}

void Map::drawMap()
{
  drawMapCount = drawMapCount + 1;
  char msgType = 'm';

  write(1, &msgType, sizeof(char));
  write(1, &noticeCount, sizeof(int));
  write(1, &drawMapCount, sizeof(int));
  write(1, mapmem, mapHeight*mapWidth);
}

//Calculate offset into memory array
char Map::operator()(int y, int x)
{
  if(y<0 || y>=mapHeight)
    throw std::out_of_range("Y coordinate out of range");
  if(x<0 || x>=mapWidth)
    throw std::out_of_range("X Coordinate out of range");
  return *(mapmem+y*mapWidth+x);
}

unsigned int Map::getPlayer(unsigned int playerMask)
{
	return -1;
}

std::string Map::getMessage()
{
  return "stub";
}

//Draw and refresh map from memory array
