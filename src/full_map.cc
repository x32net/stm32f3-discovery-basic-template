/*
 * full_map.c
 *
 *  Created on: Jan 31, 2013
 *      Author: matt
 */

#include <iostream>
using namespace std;

int Map[6][6] =
{
    {1, 1, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0},
    {0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 1, 1},
    {0, 0, 0, 0, 1, 1}
};
unsigned long int NumPaths = 0;

typedef enum{
  NORTH = 0,
  SOUTH = 1,
  EAST = 2,
  WEST = 3,
  UP = 4,
  DOWN = 5
} Direction_t;

void print_pos(int pos){
  switch(pos){
  case 0:
    cout << "NORTH\n";
    break;
  case 1:
    cout << "SOUTH\n";
    break;
  case 2:
    cout << "EAST\n";
    break;
  case 3:
    cout << "WEST\n";
    break;
  case 4:
    cout << "UP\n";
    break;
  case 5:
    cout << "DOWN\n";
    break;
  }
}

int rec_find_path(Direction_t pos, int depth ){
  if(depth == 24){
    print_pos(pos);
    return 1;
  }
  for(int i=0; i<6; i++){
    if(Map[pos][i]==0){
      Map[pos][i] =1;
      if(rec_find_path((Direction_t)i, depth+1)){
        print_pos(pos);
        Map[pos][i] =0;
        return 1;
      }
      Map[pos][i] =0;
    }
  }
  return 0;
}

void rec_find_all_path(Direction_t pos, int depth ){
  if(depth == 24){
    NumPaths++;
  }
  for(int i=0; i<6; i++){
    if(Map[pos][i]==0){
      Map[pos][i] =1;
      rec_find_all_path((Direction_t)i, depth+1);
      Map[pos][i] =0;
    }
  }
}

int main(){
  if(rec_find_path(UP,0)){
    cout << "Found path!\n";
  }
  else{
    cout << "No path Found\n";
  }
  rec_find_all_path(UP,0);
  cout << NumPaths << " Paths found!\n";
  return 0;
}
