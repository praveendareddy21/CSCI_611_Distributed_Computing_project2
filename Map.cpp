// Map.cpp, Version 1.2 (11 Mar 2015)
#include<ncurses.h>
#include<panel.h>
#include<cstdlib>
#include<cstring>
#include<utility> //for std::pair
#include<iostream>
#include<stdexcept>

#include"goldchase.h"
#include"Screen.h"
#include"Map.h"


//Initialize the object and draw the map
Map::Map(const unsigned char* mmem, int ylength, int xwidth)
  : mapHeight(ylength), mapWidth(xwidth), mapmem(mmem), theMap(ylength, xwidth)
{
	/*
	write(1, &rows, sizeof(int));
	  write(1, &cols, sizeof(int));
	  write(1, mapptr, rows*cols);
	 */
}

int Map::getKey()
{
	/*
	write(1, &drawmaps, sizeof(int));
	  write(1, &postnotices, sizeof(int));
	  write(1, mapptr, rows*cols);
	  char xyz;
	  read(0, &xyz, sizeof(char));
	  return xyz;
	 */
	return 106;
}
void Map::postNotice(const char* msg)
{
  // write(1, &postnotices, sizeof(int));

}

void Map::drawMap()
{
	// write(1, &postnotices, sizeof(int));
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
  std::vector<int> players;
  //Map shouldn't be aware of this mapping.
  //Oh, well.  So much to do and so little time.
  if(G_PLR0 & playerMask)
    players.push_back(1);
  if(G_PLR1 & playerMask)
    players.push_back(2);
  if(G_PLR2 & playerMask)
    players.push_back(3);
  if(G_PLR3 & playerMask)
    players.push_back(4);
  if(G_PLR4 & playerMask)
    players.push_back(5);
  if(players.size()==0)
  {
    postNotice("ERROR: no players to select from!");
    return 0;
  }
  int choice=theMap.getOrdinal("Player?",players);
  switch(choice)
  {
    case 1:
      return G_PLR0;
    case 2:
      return G_PLR1;
    case 3:
      return G_PLR2;
    case 4:
      return G_PLR3;
    case 5:
      return G_PLR4;
    default:
      return -1;
  }
}

std::string Map::getMessage()
{
  return theMap.getText();
}

//Draw and refresh map from memory array

