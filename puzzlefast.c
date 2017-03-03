#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

/*Contains information about the tile*/
struct tile{
  int num; //tile number
  char* t; //top encoding
  char* r; //right encoding 
  char* b; //bottom encoding
  char* l; //left encoding
};

typedef struct tile* Tile;

/*Contains information about a given list of tiles*/
struct tileListInfo{
	Tile* list; /*The actual list of tiles*/
	char* sequence; /*A string representation of the tiles, used for rotation checking*/
};
typedef struct tileListInfo* Tinfo;

/*Globals*/
Tile* tList;
Tinfo* solList;
int solCounter = 0;
struct timeval start;
struct timeval end;

/*Forward Declarations*/
void readAndInit();
void print(Tile* s);
int pTS(int x, int y);
void rotate(Tile t);
void findSol(Tile* candidateL, int x, int y, int size, Tile* sol);
void printT(Tile t);
Tile* copyTList(Tile* src);
int isRotation(char*a, char*b);
char* makeString(Tile* list);
int solExists(char*a, Tinfo* ti);

int main(){
	gettimeofday(&start,NULL);
	readAndInit();
	 //Read in from input file, and initialize some globals
	Tile* sol = (Tile*)malloc(sizeof(Tile)*9);/*Initialize list of tiles that form 1 solution*/
	solList = (Tinfo*)malloc(sizeof(Tinfo)*9);/*Initiallize list of solutions, which has max of 9 solutions*/
	  
	for(int i = 0; i<9; i++){
		sol[i] = NULL;
		solList[i] = NULL;
	}

	findSol(tList, 0,0,9,sol );
	if(solCounter<1){
		printf("No Solutions Found\n");
	}else{
		printf("%i unique solution", solCounter);
		if(solCounter>1){ printf("s");}
		printf(" found:\n");
		for(int i = 0; i<solCounter; i++){
			print(solList[i]->list);
			printf("\n");
		}
	}
	gettimeofday(&end,NULL);
	long secs_used=(end.tv_sec - start.tv_sec); //avoid overflow by subtracting first
    long micros_used= ((secs_used*1000000) + end.tv_usec) - (start.tv_usec);
	float secs= micros_used/1e6;
	printf("Time elapsed: %f\n", secs);
	return 0;
}


void findSol(Tile* candidateL, int x, int y, int size, Tile* sol){	
	int fit = 0;
	int lT = pTS(x-1,y);
	int tT= pTS(x, y-1); 
	if((y==3)){
		char* s = makeString(sol);
		if(!solExists(s, solList)){
			Tinfo ti = (Tinfo)malloc(sizeof(struct tileListInfo));
			ti->list = copyTList(sol);
			ti->sequence= s;
			solList[solCounter]= ti;
			solCounter++;
		}
		return;
	}
	for(int i = 0; i<size; i++){
		//if(candidateL[i] !=NULL){
			fit = 0;
			for(int j = 0; j<4 && !fit; j++){
				if((lT>=0 && sol[lT]->r[0] == candidateL[i]->l[0] && ((sol[lT]->r[1]-'0') + (candidateL[i]->l[1]-'0') ==1 ))&&
					(tT>=0 && sol[tT]->b[0] == candidateL[i]->t[0] &&  ((sol[tT]->b[1]-'0') + (candidateL[i]->t[1]-'0') ==1 ))){
					/*found fitting tile in bottom right quadrant*/
					fit = 1;
					sol[pTS(x,y)] = candidateL[i];
					
					int tempx=x;
					int tempy = y;
					if(x==2){
						tempx=0;
						tempy++;
					}else{
						tempx++;
					}
					Tile* cTemp = (Tile*)malloc(sizeof(Tile)*size-1);
					//cTemp[i] = NULL;
					int index = 0;
					for(int m = 0; m<size; m++){
						if(m!=i){
							cTemp[index] = candidateL[m];
							index++;
						}
					}
					
					findSol(cTemp, tempx,tempy, size-1,sol);
					sol[pTS(x,y)] =NULL;
				}else if(tT<0 &&lT>=0 &&(sol[lT]->r[0] == candidateL[i]->l[0] && ((sol[lT]->r[1]-'0') + (candidateL[i]->l[1]-'0') ==1))){
					/*in top row and matching */
					fit = 1;
					sol[pTS(x,y)] = candidateL[i];
					
					int tempx=x;
					int tempy = y;
					if(x==2){
						tempx=0;
						tempy++;
					}else{
						tempx++;
					}
					Tile* cTemp = (Tile*)malloc(sizeof(Tile)*size-1);
					//cTemp[i] = NULL;
					int index = 0;
					for(int m = 0; m<size; m++){
						if(m!=i){
							cTemp[index] = candidateL[m];
							index++;
						}
					}
					findSol(cTemp, tempx,tempy, size-1,sol);
					sol[pTS(x,y)] =NULL;
					
				}else if( lT<0 &&(tT>=0 && sol[tT]->b[0] == candidateL[i]->t[0] &&  ((sol[tT]->b[1]-'0') + (candidateL[i]->t[1]-'0') ==1 ))){
					/*in left row and matching*/
					fit = 1;
					sol[pTS(x,y)] = candidateL[i];
					
					Tile* cTemp = (Tile*)malloc(sizeof(Tile)*size-1);
					//cTemp[i] = NULL;
					int index = 0;
					for(int m = 0; m<size; m++){
						if(m!=i){
							cTemp[index] = candidateL[m];
							index++;
						}
					}
					findSol(cTemp, x+1,y, size-1,sol);
					sol[pTS(x,y)] =NULL;
				}else if (lT<0 && tT<0){
					sol[pTS(x,y)] = candidateL[i];
					Tile* cTemp = (Tile*)malloc(sizeof(Tile)*size-1);
					//cTemp[i] = NULL;
					int index = 0;
					for(int m = 0; m<size; m++){
						if(m!=i){
							cTemp[index] = candidateL[m];
							index++;
						}
					}
					findSol(cTemp, x+1,y, size-1,sol);
					sol[pTS(x,y)] =NULL;
					rotate(candidateL[i]);
				}else{
					rotate(candidateL[i]);
				}
			}
		//}
	}
	free(candidateL);
}

/*rotate clockwise */
void rotate(Tile t){
	char* temp= t->t;
	t->t = t->l;
	t->l = t->b;
	t->b = t->r;
	t->r = temp;

}
int pTS(int x, int y){
	if(y<0)return  y=-1;
	if(x<0)return x=-1;
	return y*3+x;
}

void readAndInit(){
  char* t = (char*)malloc(sizeof(char)*3);
  char*  r = (char*)malloc(sizeof(char)*3);
  char* b = (char*)malloc(sizeof(char)*3);
  char* l = (char*)malloc(sizeof(char)*3);
  char ch;
  FILE* fp;
  char str[100];
  fp = fopen("input.txt", "r");
  
  tList = (Tile*)malloc(sizeof(Tile)*9);
   printf("input tiles:\n");
  for(int i = 0; i<9; i++){
    fgets(str, 100, fp);
	printf("%i. %s", i+1,str);
	sscanf(str, "%[^','],%[^','],%[^','],%s",t, r, b,  l);
    Tile nT = (Tile)malloc(sizeof(struct tile));	
	nT->t =(char*)malloc(sizeof(char)*3);
	nT->r =(char*)malloc(sizeof(char)*3);
	nT->b =(char*)malloc(sizeof(char)*3);
	nT->l =(char*)malloc(sizeof(char)*3);
	
    strcpy(nT->t,t);
    strcpy(nT->r,r);
    strcpy(nT->b, b);
    strcpy(nT->l ,l);
    nT->num = i+1;
    tList[i] = nT;
    
  }
  printf("\n\n");
  fclose(fp);
}

Tile* copyTList(Tile* src){
	Tile* new = (Tile*)malloc(sizeof(Tile)*9);
	for(int i = 0; i<9; i++){
		new[i] = (Tile)malloc(sizeof(struct tile));
		new[i]->num = src[i]->num;
		new[i]->t = src[i]->t;
		new[i]->r = src[i]->r;
		new[i]->l = src[i]->l;
		new[i]->b = src[i]->b;
	}
	return new;
}
char* makeString(Tile* list){
	char* s = (char*)malloc(sizeof(char)*10);
	int i;
	for( i = 0; i<9; i++){
		s[i] = list[i]->num;
	}
	s[i] = '\0';
	return s;
}
int solExists(char*a, Tinfo* ti){
	int index = 0;
		while(solList[index]!=NULL){
			if(isRotation(a, solList[index]->sequence)){
				return 1;
			}
			index++;
		}
		return 0;
}
int isRotation(char*a, char*b){
	if(a==NULL){
		return 0;
	}else {
		//first rotation
		if(a[0]==b[6] && a[1]==b[3] && a[2]==b[0] &&
		   a[3]==b[7] && a[4]==b[4] && a[5]==b[1] && 
		   a[6]==b[8] && a[7]==b[5] && a[8]==b[2]){
				return 1;
			}
		//second rotation
		if(a[0]==b[8] && a[1]==b[7] && a[2]==b[6]&&
		   a[3]==b[5] && a[4]==b[4] && a[5]==b[3] &&
		   a[6]==b[2] && a[7]==b[1] && a[8]==b[0]){
				return 1;
		}
		//third rotation
		if(a[0]==b[2] && a[1]==b[5] && a[2]==b[8] &&
		   a[3]==b[1] && a[4]==b[4] && a[5]==b[7] &&
		   a[6]==b[0] && a[7]==b[3] && a[8]==b[6]){
			   return 1;
		   }
	}
	return 0;
}

void printT(Tile t){
	printf("+----------+\n");
	printf("|%i    %s   |\n", t->num, t->t);
	printf("|%s   %s   |\n", t->l, t->r);
	printf("|   %s     |\n", t->b);
	printf("+----------+\n");
}
void print(Tile* s){
  for(int j = 0; j<3; j++){
    printf("+--------+--------+--------+\n");
    for(int i = j*3; i<j*3+3; i++){
		if(s[i]!=NULL){
			printf("|%i  %s   ",s[i]->num, s[i]->t);
		}else{
			printf("|        ");
		}
    }
    printf("|\n");
    
    for(int i = j*3; i<3*j+3; i++){
		if(s[i]!=NULL){
			printf("|%s    %s",s[i]->l, s[i]->r);
		}else{
			printf("|        ");
		}
    }
    printf("|\n");
    
    for(int i = j*3; i<3*j+3; i++){
		if(s[i]!=NULL){
			printf("|   %s   ",s[i]->b);
		}else{
			printf("|        ");
		}
    }
    printf("|\n");
  }
   printf("+--------+--------+--------+\n");
}
