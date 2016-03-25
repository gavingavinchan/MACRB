#include <QueueList.h>
#include <StackArray.h>
#include "Configuration.h"

#include "Map.h"

// Constructor
Maptile::Maptile(){
  walls = 0x00;
  type = normal;
  visits = 0;
  victim =  false;
}

boolean Maptile::hasWall(Direction dir){
  return walls & dir;
}

void Maptile::setWall(Direction dir, boolean on){
  if (on){
    walls |= dir;
  }
}


Map::Map(int width, int height){
  for(int y=0; y<height; y++){
    for(int x=0; x<width; x++){
      tiles[x][y] = Maptile();
    } 
  }
}

void Map::setWall(int x, int y, Direction dir){
  if(dir==North){
    tiles[x][y].setWall(North,true);
    tiles[x][y-1].setWall(South,true);
  }
  if(dir==East){
    tiles[x][y].setWall(East,true);
    tiles[x+1][y].setWall(West,true);
  }
  if(dir==South){
    tiles[x][y].setWall(South,true);
    tiles[x][y+1].setWall(North,true);
  }
  if(dir==West){
    tiles[x][y].setWall(West,true);
    tiles[x-1][y].setWall(East,true);
  }
}

void Map::setWall(Coordinate coor, Direction dir){
  setWall(coor.x, coor.y, dir);
}


void Map::printMap(){
  //Serial.println("Map:");
  for(int j=0; j<MAP_HEIGHT; j++){
    for(int i=0; i<MAP_WIDTH; i++){
      Serial.print(tiles[i][j].type);
      Serial.print(" ");
      Serial.print(tiles[i][j].visits);
      Serial.print("\t");
      if (tiles[i][j].hasWall(East)) Serial.print("|");
      Serial.print("\t");
    }
    
    Serial.println();
    
    for(int i=0; i<MAP_WIDTH; i++){
      if (tiles[i][j].hasWall(South)) Serial.print("---");
      else Serial.print("   ");      
      Serial.print("\t");
      Serial.print(" \t");
    }
    
    Serial.println();
  }
}

StackArray <Coordinate> Map::findPath(Coordinate start, Direction currentDirection, Coordinate entrance){
  QueueList <Coordinate> Q; // Queue
  Coordinate t;
  Coordinate p; //present
  Coordinate n; //next
  boolean V[MAP_WIDTH][MAP_HEIGHT]; // Visited Set
  backTrack track[MAP_WIDTH][MAP_HEIGHT];
  for(int i=0;i<MAP_WIDTH; i++)
      for(int j=0;j<MAP_HEIGHT; j++)
        V[i][j] = false;
  V[start.x][start.y] = true;
  for(int j=0;j<MAP_WIDTH; j++){
        for(int k=0;k<MAP_HEIGHT; k++){
          track[j][k].cost=9999;
          track[j][k].prevTile.x = -1;
          track[j][k].prevTile.y = -1;
        }
  }
  track[start.x][start.y].cost = 0;
  track[start.x][start.y].facing = currentDirection;
  
  Q.push(start);
  while(!Q.isEmpty()){
    t = Q.pop();
    
    int x = t.x;
    int y = t.y;
    
    findAvailableTile(x,y,North,V,track,Q);
    findAvailableTile(x,y,South,V,track,Q);
    findAvailableTile(x,y,East,V,track,Q);
    findAvailableTile(x,y,West,V,track,Q);
  }
  
// FOR DEBUG
#if DEBUGLEVEL > 5
  for(int j=0;j<MAP_HEIGHT; j++){
        for(int i=0;i<MAP_WIDTH; i++){
          Serial.print(track[i][j].cost);
          Serial.print("(");
          Serial.print(track[i][j].prevTile.x);
          Serial.print(",");
          Serial.print(track[i][j].prevTile.y);
          Serial.print(")");
          Serial.print("\t");
        }
        Serial.println();
        Serial.println();
   }
#endif

   Coordinate destination;
   destination = determineDestination(start, track);
   if(destination.x<0) destination = entrance;
#if DEBUGLEVEL > 5   
   printCoordinate(destination);
#endif

  int evalX = destination.x;
  int evalY = destination.y;
  
  StackArray <Coordinate> St; //Stack
  St.push(destination);
  
  while(!(evalX==start.x && evalY==start.y)){
    backTrack evaluating = track[evalX][evalY];
    St.push(evaluating.prevTile);
    evalX = evaluating.prevTile.x;
    evalY = evaluating.prevTile.y;
  }
  
  return St;
};

void Map::findAvailableTile(
  int cx, int cy, // Current Evaluating Tile 
  Direction dir, // Look at this direction
  boolean V[MAP_WIDTH][MAP_HEIGHT], 
  backTrack track[MAP_WIDTH][MAP_HEIGHT], 
  QueueList<Coordinate>& Q
){
  int destx = cx, desty = cy;
  switch(dir){
    case North:
      desty=cy-1; break;
    case East:
      destx=cx+1; break;
    case South:
      desty=cy+1; break;
    case West:
      destx=cx-1; break;
  }
  
  if (destx<0 || desty<0 || destx>MAP_WIDTH-1 || desty>MAP_HEIGHT-1) return;

  if (tiles[cx][cy].hasWall(dir) || V[destx][desty]) return;

  V[destx][desty] = true;
  Coordinate n;
  n.x = destx;
  n.y = desty;  
  Q.push(n);
  
  backTrack cTrack = track[cx][cy]; //current
  backTrack dTrack = track[destx][desty];
  int orientCost = 0;
  if(cTrack.facing==dir) {
    orientCost=0;
  }else{
    if ((dir>South && cTrack.facing > South) ||
      (dir<=South && cTrack.facing <=South)) 
      orientCost = 2;
    else
      orientCost = 1;
  }
  
  dTrack.prevTile.x = cx;
  dTrack.prevTile.y = cy;
  dTrack.facing=dir;
  dTrack.cost = cTrack.cost + tiles[destx][desty].type + orientCost;
  track[destx][desty] = dTrack;
}

Coordinate Map::determineDestination(Coordinate start, backTrack track[MAP_WIDTH][MAP_HEIGHT]){
  int smallest = 888;
  Coordinate output;
  output.x = -1;
  output.y = -1;
  for(int y=0; y<MAP_HEIGHT; y++){
    for(int x=0; x<MAP_WIDTH; x++){
      if (track[x][y].cost < smallest){
        if (start.x == x && start.y == y) continue;
        if (tiles[x][y].visits > 0) continue;
        output.x = x;
        output.y = y;
        smallest = track[x][y].cost;
      }
    }
  }
  return output;
}

void Map::printCoordinate(Coordinate coor){
  Serial.print("(");
  Serial.print(coor.x);
  Serial.print(", ");
  Serial.print(coor.y);
  Serial.print(")");
}

void Map::addVisit(Coordinate coor){
  tiles[coor.x][coor.y].visits++;
}

boolean Map::hasVictim(Coordinate coor){
  return tiles[coor.x][coor.y].victim;
}

void Map::setVictim(Coordinate coor){
   tiles[coor.x][coor.y].victim = true;
}

void Map::setBlackTile(Coordinate coor){
  tiles[coor.x][coor.y].type = black;
}

