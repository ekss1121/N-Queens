#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;


const int max_level=4;
std::vector <int> pos(max_level, -1);

vector<int> nqueens_by_level(int start_level) {
	
	int colNum = start_level;
	bool cond = false;
	while(1){
		for(int rowNum=0; rowNum<max_level; ++rowNum){ //test all possibles
		
			 for(int takenIdx=0; takenIdx<(colNum+1); ++takenIdx){ //check already taken
				
				 if (pos[takenIdx] == rowNum || abs(pos[takenIdx]-rowNum) == abs(colNum-takenIdx)){
					cond = false;
					break;
				 }
				 cond = true;
				 
			 }
			 if(cond == true){
				 pos[colNum] = rowNum;
				 ++colNum;
				 break;
			 }else{
				 if(rowNum == max_level-1){
				 ++colNum;}
			 }
		}
		if(pos[colNum-1] == -1){
			pos[start_level] = pos[start_level]+1;
			colNum = start_level+1;
		}

		if(colNum >= max_level){
			break;}

		
	}
	pos.resize(max_level, 0xDEADC0DE);
	return pos;
		
}							

int main(){
	nqueens_by_level(0);
	for(int i=0; i<max_level; ++i){
		printf("%d\n",pos[i]);
	}

	return 0;
}
