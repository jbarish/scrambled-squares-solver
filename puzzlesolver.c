#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

/**
 * Justin Barish
 * James Murphy
 * Karan Shah
 *
 * We pledge our honor that we have abided by the Stevens Honor System 
 */


/*Contains information about the tile*/
struct tile{
  int num; //tile number
  char t[3]; //top encoding
  char r[3]; //right encoding 
  char b[3]; //bottom encoding
  char l[3]; //left encoding
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
char* fileName;
int frstCounter = 0;

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


int main(int argc, char *argv[]){
	if(argc<2){
		perror("Need to give at least 1 argument.");
		exit(1);
	}
	fileName = argv[1];
	gettimeofday(&start,NULL);
	readAndInit();
	 //Read in from input file, and initialize some globals
	Tile* sol = (Tile*)malloc(sizeof(Tile)*9);/*Initialize list of tiles that form 1 solution*/
	solList = (Tinfo*)malloc(sizeof(Tinfo)*9);/*Initiallize list of solutions, which has max of 9 solutions*/
	  
	for(int i = 0; i<9; i++){
		sol[i] = NULL;
		solList[i] = NULL;
	}

	findSol(tList, 0,0,9,sol ); //Find solution to puzzle. Give it list of tiles, and coordinates 0,0
	if(solCounter<1){
		printf("No Solutions Found\n");
	}else{ //print out all unique solutions
		printf("%i unique solution", solCounter);
		if(solCounter>1){ printf("s");}
		printf(" found:\n");
		for(int i = 0; i<solCounter; i++){
			print(solList[i]->list);
			printf("\n");
		}
	}
	/*get running time*/
	gettimeofday(&end,NULL);
	long secs_used=(end.tv_sec - start.tv_sec); 
    long micros_used= ((secs_used*1000000) + end.tv_usec) - (start.tv_usec);
	float secs= micros_used/1e6;
	printf("Time elapsed: %f\n", secs);
	return 0;
}

/*
 * Recursive method to find all solutions to the puzzle
 * candidateL -> list of tiles that can be chosen from, shrinks by 1 tile every calloc
 * x   ->        x coordinate on grid that is being filled in
 * y   ->        y coordinate on grid that is being filled in
 * size ->       size of the candidateL (In this implementation, always is 9)
 * Sol  ->       current running solution to the puzzle, with n tile in place 
 */
void findSol(Tile* candidateL, int x, int y, int size, Tile* sol){	
	int fit = 0; /*Boolean to check if tile fits and no longer needs rotation*/
	int lT = pTS(x-1,y); /*tile at the current tile's left */
	int tT= pTS(x, y-1); /*tile at the current tile's top */
	
	/*success condition, found a solution*/
	if(y==3){ 
		char* s = makeString(sol); /*get string representation of solution*/
		if(!solExists(s, solList)){ /*check if already exists*/
			Tinfo ti = (Tinfo)malloc(sizeof(struct tileListInfo));
			ti->list = copyTList(sol);
			ti->sequence= s;
			solList[solCounter]= ti;
			solCounter++;
		}
		return;
	}
	/*loop through all possible tiles left in candidate list to fit the x,y square*/
	for(int i = 0; i<size; i++){
		if(candidateL[i] !=NULL){
			fit = 0;
			for(int j = 0; j<4 && !fit; j++){
					//printf("Candidate List:\n");
					//print(candidateL);
					//printf("Solution List:\n");
					//print(sol);
					//sleep(2);
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
					Tile* cTemp = (Tile*)malloc(sizeof(Tile)*size);
	
					memcpy(cTemp, candidateL, 9*sizeof(Tile));
					cTemp[i] = NULL;
					findSol(cTemp, tempx,tempy, size,sol);
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
					Tile* cTemp = (Tile*)malloc(sizeof(Tile)*size);
					
					memcpy(cTemp, candidateL, 9*sizeof(Tile));
					cTemp[i] = NULL;	
					findSol(cTemp, tempx,tempy, size,sol);
					sol[pTS(x,y)] =NULL;
					
				}else if( lT<0 &&(tT>=0 && sol[tT]->b[0] == candidateL[i]->t[0] &&  ((sol[tT]->b[1]-'0') + (candidateL[i]->t[1]-'0') ==1 ))){
					/*in left row and matching*/
					fit = 1;
					sol[pTS(x,y)] = candidateL[i];
					
					Tile* cTemp = (Tile*)malloc(sizeof(Tile)*size);
				
					memcpy(cTemp, candidateL, 9*sizeof(Tile));
					cTemp[i] = NULL;
					findSol(cTemp, x+1,y, size,sol);
					sol[pTS(x,y)] =NULL;
				}else if (lT<0 && tT<0){
					
					/*first tile*/
					sol[pTS(x,y)] = candidateL[i];
					Tile* cTemp = (Tile*)malloc(sizeof(Tile)*size);
					
					memcpy(cTemp, candidateL, 9*sizeof(Tile));
					cTemp[i] = NULL;
					findSol(cTemp, x+1,y, size,sol);
					sol[pTS(x,y)] =NULL;
					frstCounter++;
					if(frstCounter<2){
						rotate(candidateL[i]);
					}else{
						frstCounter=0;
						fit = 1;
					}
				}else{
					rotate(candidateL[i]);
				}
			}
		}
	}
	free(candidateL);
}

/*rotate clockwise */
void rotate(Tile t){
	char temp[3];
	
	memcpy(&temp,&(t->t),3);
	memcpy(&(t->t),&(t->l),3);
	memcpy(&(t->l),&(t->b),3);
	memcpy(&(t->b),&(t->r),3);
	memcpy(&(t->r),&temp,3);	
}

/*Convert x, y coordinate to point in single list */
int pTS(int x, int y){
	if(y<0)return  y=-1;
	if(x<0)return x=-1;
	return y*3+x;
}

/*Read in all data from the file*/
void readAndInit(){

  char ch;
  FILE* fp;
  char str[100];
  fp = fopen(fileName, "r");
  
  tList = (Tile*)malloc(sizeof(Tile)*9);
   printf("input tiles:\n");
  for(int i = 0; i<9; i++){
    fgets(str, 100, fp);
	
    Tile nT = (Tile)malloc(sizeof(struct tile));	
	sscanf(str, "%[^','],%[^','],%[^','],%s",nT->t, nT->r, nT->b,  nT->l);
	printf("%i. <%s,%s,%s,%s>\n", i+1,nT->t,nT->r,nT->b, nT->l);

    nT->num = i+1;
    tList[i] = nT;
    
  }
  printf("\n\n");
  fclose(fp);
}

/*Make a deep copy of the tile list*/
Tile* copyTList(Tile* src){
	Tile* new = (Tile*)malloc(sizeof(Tile)*9);
	for(int i = 0; i<9; i++){
		new[i] = (Tile)malloc(sizeof(struct tile));
		new[i]->num = src[i]->num;
		memcpy(new[i]->t,src[i]->t,3);
		memcpy(new[i]->r ,src[i]->r,3);
		memcpy(new[i]->l, src[i]->l,3);
		memcpy(new[i]->b , src[i]->b,3);
	}
	return new;
}

/*make a string representation of the list*/
char* makeString(Tile* list){
	char* s = (char*)malloc(sizeof(char)*10);
	int i;
	for( i = 0; i<9; i++){
		s[i] = list[i]->num;
	}
	s[i] = '\0';
	return s;
}

/*check if found solution already exists*/
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

/*check if two tiles lists are rotations of eachother */
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
