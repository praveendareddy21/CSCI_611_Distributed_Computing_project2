# Project 2: Autotesting Gold Chase
Testing Framework project for testing gold chase Game.

## Summary
The purpose of the second project is to develop a driver program that will automate evaluation
of the Gold Chase game. The driver will automatically start two game processes (that is, two
players), provide input to the game processes, and evaluate the game output for correctness.
To facilitate the automation, the implementation of the Map class will be replaced with code that
writes/reads game data that the driver program evaluates.
As a first step to understanding where the Gold Chase game could exchange information with
the driver program, we need to identify the points where the game currently interacts with the

### Steps in Map class:
1. Open mymap.txt (or some other filename), read in the number of fools/gold and the lines
of the map.
2. Call Map constructor, e.g., renderer=new Map(mapMemory->board,rows,cols);
3. Call Map::getKey()
  1. interpret input, update shared memory if necessary
  2. call Map::drawMap() -- if appropriate
  3. call Map::postNotice() -- if appropriate
4. Repeat, until user wins or quits

In addition to interacting with a reimplemented Map class, the driver program should control the
map that Gold Chase reads in--providing the game with a map which eases the driver's task of
testing the game.
To simplify communications between the game's reimplemented Map class and the driver,
communications should be restricted to two functions: Map's constructor and Map's getKey
member function. The constructor is ideal for providing information back to the driver--a kind of
initial handshaking to confirm that everything is working properly. The getKey member function
should pass information regarding the current state of the game. The other two member
functions (drawMap and postNotice) can simply maintain counters of number of times called.
These counters could then be passed to the driver as part of the state information that getKey
provides.


## Steps in GoldChase Game Process
1. The driver must be able to read from, and write to, a game process. Since a pipe()
command creates a one-way pipe, create two pipes for a game process.
2. Fork the process.
3. Don't forget to have child and parent close all unnecessary file descriptors.
Once the pipelines have been prepared between the parent and child, the game can be exec'd
in the child. The parent must now have the complementary behavior to that of the child
described in the numbered list above:
4. Open mymap.txt (or some other filename), write out the number of fools/gold and the lines
of the map. Close.
5. Read the rows, columns, and map from the game process (sent from Map::Map). Confirm
expected values.
6. Read the "draw map" variable, "post notice" variable, and map from the game process (sent
from Map::getKey). Confirm values are as expected (if necessary). Decide on what the
next move should be and write a character to the game process.
7. Repeat #6 until test(s) are completed. If necessary, write a 'Q' to the game process to end
it.
8. Note that the above description must be expanded to handle two game processes.
