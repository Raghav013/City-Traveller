#include<string.h>
#include<stdio.h>
#include<tm_pair.h>
#include<stdlib.h>
#include<tm_pqueue.h>
#include<tm_avl_tree.h>
void createGraphFile();
void removeAdjacentVertex();

typedef struct __city
{
int code;
char name [52];
}City;

typedef struct _city_header
{
int lastGeneratedCode;
int recordCount;
}CityHeader;

CityHeader cityHeader;
AVLTree *cities;
AVLTree *citiesByName;
AVLTree *graph;

int cityCodeComparator(void *p,void *q)
{
City *a=(City *)p;
City *b=(City *)q;
return a->code-b->code;
}

int cityNameComparator(void *p,void *q)
{
City *a=(City *)p;
City *b=(City *)q;
return stricmp(a->name,b->name);
}

int adjacentVertexComparator(void *left,void *right)
{
Pair *leftPair,*rightPair;
City *leftCity,*rightCity;
leftPair=(Pair *)left;
rightPair=(Pair *)right;
leftCity=(City *)leftPair->first;
rightCity=(City *)rightPair->first;
stricmp(leftCity->name,rightCity->name);
}

int graphVertexComparator(void *left,void *right)
{
Pair *leftPair,*rightPair;
City *leftCity,*rightCity;
leftPair=(Pair *)left;
rightPair=(Pair *)right;
leftCity=(City *)leftPair->first;
rightCity=(City *)rightPair->first;
stricmp(leftCity->name,rightCity->name);
}

void drawLine(char chr,int sz)
{
int e;
for(e=1;e<=sz;e++) printf("%c",chr);
printf("\n");
}

void releaseData()
{
destroyAVLTree(cities);
destroyAVLTree(citiesByName);
destroyAVLTree(graph);
}

void populateDataStructure(int *success)
{
FILE *cityFile,*graphFile;
int succ,code,acode;
City *advCity,*city;
Pair *p1,*p2;
char m;
AVLTree *advTree;
int weight,*edgeWeight;
City c;
if(success)*success=false;
cities=createAVLTree(&succ,cityCodeComparator);
if(succ==false)
{
printf("Unable to load data..\n");
return;
}
citiesByName=createAVLTree(&succ,cityNameComparator);
if(succ==false)
{
printf("Unable to load data...\n");
destroyAVLTree(cities);
return;
}

cityFile=fopen("city.dat","rb+");
if(cityFile!=NULL)
{
fread(&cityHeader,sizeof(CityHeader),1,cityFile);
if(!feof(cityFile))
{
while(1)
{
fread(&c,sizeof(City),1,cityFile);
if(feof(cityFile))break;
city=(City *)malloc(sizeof(City));
city->code=c.code;
strcpy(city->name,c.name);
insertIntoAVLTree(cities,city,&succ);
insertIntoAVLTree(citiesByName,city,&succ);
}
fclose(cityFile);
}
}
if(cities->size==0)
{
cityHeader.lastGeneratedCode=0;
cityHeader.recordCount=0;
}
graph=createAVLTree(&succ,graphVertexComparator);
graphFile=fopen("graph.dat","r");
if(graphFile!=NULL)
{
while(1)
{
code=0;
m=fgetc(graphFile);
if(feof(graphFile))break;
code=m-48;
while(1)
{
m=fgetc(graphFile);
if(m==',' || m=='#')break;
code=(code*10)+m-48;
}
c.code=code;
city=(City *)getFromAVLTree(cities,&c,&succ);
p1=(Pair *)malloc(sizeof(Pair));
p1->first=(void *)city;
advTree=createAVLTree(&succ,adjacentVertexComparator);
p1->second=(AVLTree *)advTree;
while(1)
{
acode=0;
while(1)
{
m=fgetc(graphFile);
if(m==',')break;
acode=(acode*10)+m-48;
}
weight=0;
while(1)
{
m=fgetc(graphFile);
if(m==',' || m=='#' || feof(graphFile))break;
weight=(weight*10)+m-48;
}
c.code=acode;
advCity=(City *)getFromAVLTree(cities,&c,&succ);
edgeWeight=(int *)malloc(sizeof(int));
*edgeWeight=weight;
p2=(Pair *)malloc(sizeof(Pair));
p2->first=(void *)advCity;
p2->second=(void *)edgeWeight;
insertIntoAVLTree(advTree,p2,&succ);
if(m=='#' || feof(graphFile))break;
}
insertIntoAVLTree(graph,p1,&succ);
}
fclose(graphFile);
}
if(success)*success=true;
}


void displayListOfCity()
{
City *city;
int pageNo,sno,succ;
AVLTreeInOrderIterator iterator;
sno=0;
int newPage=5;
if(cities->size==0)
{
drawLine('-',50);
printf("NO Records added.press ENTER to continue....\n"); 
drawLine('-',50);
getchar();
fflush(stdin);
return;
}
iterator=getAVLTreeInOrderIterator(citiesByName,&succ);
if(succ)
{
pageNo=0;
while(hasNextInOrderElementInAVLTree(&iterator))
{
city=(City *)getNextInOrderElementFromAVLTree(&iterator,&succ);
if(sno%newPage==0)
{
pageNo++;
drawLine('-',50);
printf("S NO.   NAME OF THE CITIES %d\n",pageNo);
drawLine('-',50);
}
sno++;
printf("%2d       %-52s %d\n",sno,city->name,city->code);
if(sno%newPage==0)
{
drawLine('-',50);
printf("press ENTER to continue...");
getchar();
fflush(stdin);
}
}
if(cities->size==0)
{
drawLine('-',50);
printf("NO Records added.press ENTER to continue....\n"); 
drawLine('-',50);
getchar();
fflush(stdin);
return;
}
}
drawLine('-',50);
}


void removeCity()
{
FILE *file;
FILE *tmpFile;
CityHeader cHeader;
char name[52],m;
int x,succ;
City c,*city;
printf("DELETE MODULE\n");
printf("Enter name of the City to delete : ");
fgets(name,52,stdin);
fflush(stdin);
name[strlen(name)-1]='\0';
strcpy(c.name,name);
city=(City *)getFromAVLTree(citiesByName,&c,&succ);
if(!succ)
{
printf("City doesn't exists\n");
return;
}
printf("City Found\nName : %s\n",city->name);
printf("Do you wanna delete City(Y/N) : ");
m=getchar();
fflush(stdin);
if(m!='y' && m!='Y')
{
printf("City not removed\n");
return;
}
file=fopen("city.dat","rb");
tmpFile=fopen("tmp.tmp","wb");
fread(&cHeader,sizeof(CityHeader),1,file);
cityHeader.recordCount--;
fwrite(&cityHeader,sizeof(CityHeader),1,tmpFile);
while(1)
{
fread(&c,sizeof(City),1,file);
if(feof(file))break;
if(stricmp(city->name,c.name)!=0)
{
fwrite(&c,sizeof(City),1,tmpFile);
}
}
fclose(file);
fclose(tmpFile);
file=fopen("city.dat","wb");
tmpFile=fopen("tmp.tmp","rb");
fread(&cHeader,sizeof(CityHeader),1,tmpFile);
fwrite(&cHeader,sizeof(CityHeader),1,file);
while(1)
{
fread(&c,sizeof(City),1,tmpFile);
if(feof(tmpFile))break;
fwrite(&c,sizeof(City),1,file);
}
fclose(file);
fclose(tmpFile);
tmpFile=fopen("tmp.tmp","wb");
fclose(tmpFile);
c.code=city->code;
strcpy(c.name,city->name);
removeFromAVLTree(cities,&c,&succ);
removeFromAVLTree(citiesByName,&c,&succ);
free(city);
printf("City %s deleted, Press ENTER to continue....",name);
getchar();
fflush(stdin);
}


void addAdjacentVertex()
{
AVLTreeInOrderIterator graphIterator,advIterator;
FILE *graphFile;
char m,name[52],advName[52];
int shouldAppend,succ,weight,*edgeWeight,*wgt;
City *gct,c,*city,*advCity,*act;
Pair *graphPair,*advPair,*gp,*ap,aPair,gPair;
AVLTree *advt,*advTree;

printf("Enter City against which you wanna add adjacent city : ");
fgets(name,52,stdin);
fflush(stdin);
name[strlen(name)-1]='\0';
strcpy(c.name,name);
city=(City *)getFromAVLTree(citiesByName,&c,&succ);
if(!succ)
{
printf("This City doesn't exist\n");
printf("Press ENTER to continue...");
getchar();
fflush(stdin);
return;
}
shouldAppend=0;
gPair.first=(void *)city;
graphPair=(Pair *)getFromAVLTree(graph,&gPair,&succ);
if(!succ)
{
shouldAppend=1;
graphPair=NULL;
}
if(graphPair!=NULL)advTree=(AVLTree *)graphPair->second;

while(1)
{
printf("Enter adjacent  City : ");
fgets(advName,52,stdin);
fflush(stdin);
advName[strlen(advName)-1]='\0';
strcpy(c.name,advName);
advCity=(City *)getFromAVLTree(citiesByName,&c,&succ);
if(!succ)
{
printf("City %s doesn't exist\n",advName);
printf("Wanna add another city as adjacent to %s(Y/N) : ",city->name);
m=getchar();
fflush(stdin);
if(m!='y' && m!='Y')break;
continue;
}
aPair.first=(void *)advCity;
if(graphPair!=NULL)
{
advPair=(Pair *)getFromAVLTree(advTree,&aPair,&succ);
if(succ)
{
printf("%s is already adjacent to %s\n",advCity->name,city->name);
printf("Wanna add another city (Y/N) : ");
m=getchar();
fflush(stdin);
if(m!='y' && m!='Y')break;
continue;
}
}
printf("Enter weight from %s to %s : ",city->name,advCity->name);
scanf("%d",&weight);
fflush(stdin);
if(weight<=0)
{
printf("Invalid Entry\nWanna add another adjacent city (Y/N) : ");
m=getchar();
fflush(stdin);
if(m!='y' && m!='Y');break;
continue;
}
if(graphPair!=NULL)advTree=(AVLTree *)graphPair->second;
advPair=(Pair *)malloc(sizeof(Pair));
advPair->first=(void *)advCity;
edgeWeight=(int *)malloc(sizeof(int));
*edgeWeight=weight;
advPair->second=(void *)edgeWeight;
if(graphPair==NULL)
{
graphPair=(Pair *)malloc(sizeof(Pair));
graphPair->first=(void *)city;
advTree=createAVLTree(&succ,adjacentVertexComparator);
graphPair->second=(void *)advTree;
insertIntoAVLTree(advTree,advPair,&succ);
insertIntoAVLTree(graph,graphPair,&succ);
graphFile=fopen("graph.dat","a");
fprintf(graphFile,"%d,%d,%d#",city->code,advCity->code,weight);
fclose(graphFile);
}
else if(shouldAppend==1)
{
insertIntoAVLTree(advTree,advPair,&succ);
graphFile=fopen("graph.dat","r+");
fseek(graphFile,-1,SEEK_END);
fprintf(graphFile,",%d,%d#",advCity->code,weight);
fclose(graphFile);
}
else
{
insertIntoAVLTree(advTree,advPair,&succ);
createGraphFile();
}
printf("Information saved\n");
printf("Wanna add more adjacent city to %s(Y/N) : ",city->name);
m=getchar();
fflush(stdin);
if(m!='y' && m!='Y')break;
continue;
}
}


void removeAdjacentVertex()
{
AVLTreeInOrderIterator graphIterator,advIterator;
FILE *graphFile;
char m,name[52],advName[52];
int shouldAppend,succ,weight,*edgeWeight,*wgt;
City *gct,c,*city,*advCity,*act;
Pair *graphPair,*advPair,*gp,*ap,aPair,gPair;
AVLTree *advt,*advTree;

printf("Enter City against which you wanna remove adjacent city : ");
fgets(name,52,stdin);
fflush(stdin);
name[strlen(name)-1]='\0';
strcpy(c.name,name);
city=(City *)getFromAVLTree(citiesByName,&c,&succ);
if(!succ)
{
printf("This City doesn't exist\n");
printf("Press ENTER to continue...");
getchar();
fflush(stdin);
return;
}
gPair.first=(void *)city;
graphPair=(Pair *)getFromAVLTree(graph,&gPair,&succ);
if(graphPair==NULL)
{
printf("There is no Adjacent City of this City\nPress ENTER to continue...");
getchar();
fflush(stdin);
return;
}
advTree=(AVLTree *)graphPair->second;
printf("Enter Adjacent City : ");
fgets(advName,52,stdin);
fflush(stdin);
advName[strlen(advName)-1]='\0';
strcpy(c.name,advName);
advCity=(City *)getFromAVLTree(citiesByName,&c,&succ);
if(!succ)
{
printf("City doesn't exist\nPress ENTER to continue...");
getchar();
fflush(stdin);
return;
}
aPair.first=(void *)advCity;
advPair=(Pair *)getFromAVLTree(advTree,&aPair,&succ);
if(!succ)
{
printf("City doesn't exist\nPress ENTER to continue...");
getchar();
fflush(stdin);
return;
}
removeFromAVLTree(advTree,advPair,&succ);
createGraphFile();
printf("Information saved\nPress ENTER to continue...");
getchar();
fflush(stdin);
}


void createGraphFile()
{
FILE *graphFile;
AVLTreeInOrderIterator graphIterator,advIterator;
Pair *gp,*ap;
City *gct,*act;
AVLTree *advt;
int *wgt,succ;
graphFile=fopen("graph.dat","w");
graphIterator=getAVLTreeInOrderIterator(graph,&succ);
while(hasNextInOrderElementInAVLTree(&graphIterator))
{
gp=(Pair *)getNextInOrderElementFromAVLTree(&graphIterator,&succ);
gct=(City *)gp->first;
advt=(AVLTree *)gp->second;
fprintf(graphFile,"%d,",gct->code);
advIterator=getAVLTreeInOrderIterator(advt,&succ);
while(hasNextInOrderElementInAVLTree(&advIterator))
{
ap=(Pair *)getNextInOrderElementFromAVLTree(&advIterator,&succ);
act=(City *)ap->first;
wgt=(int *)ap->second;
if(hasNextInOrderElementInAVLTree(&advIterator))
{
fprintf(graphFile,"%d,%d,",act->code,*wgt);
}
else 
{
fprintf(graphFile,"%d,%d#",act->code,*wgt);
}
}
}
fclose(graphFile);

}



void editCity()
{
CityHeader cHeader;
int x,succ,code;
char name[52],newName[52],m;
City *city1,*city2,c;
FILE *file;
printf("EDIT MODULE\n");
printf("Enter name of the City to EDIT : ");
fgets(name,52,stdin);
fflush(stdin);
name[strlen(name)-1]='\0';
strcpy(c.name,name);
city1=(City *)getFromAVLTree(citiesByName,&c,&succ);
if(!succ)
{
printf("City doesn't exist\n");
return;
}
printf("Enter new Name : ");
fgets(newName,52,stdin);
fflush(stdin);
newName[strlen(newName)-1]='\0';
if(strcmp(newName,name)==0)
{
printf("No need to update\n");
return;
}
strcpy(c.name,newName);
city2=(City *)getFromAVLTree(citiesByName,&c,&succ);
if(succ && city1->code!=city2->code)
{
printf("This city already exists\n");
return;
}
printf("Do you wanna update(Y/N) : ");
m=getchar();
fflush(stdin);
if(m!='y' && m!='Y')
{
printf("City NOT edited\n");
return;
}
file=fopen("city.dat","rb+");
fread(&cityHeader,sizeof(CityHeader),1,file);
while(1)
{
x=ftell(file);
fread(&c,sizeof(City),1,file);
if(strcmp(name,c.name)==0)break;
}
fseek(file,x,SEEK_SET);
strcpy(c.name,newName);
c.code=city1->code;
fwrite(&c,sizeof(City),1,file);
fclose(file);
strcpy(c.name,name);
c.code=city1->code;
strcpy(city1->name,newName);
removeFromAVLTree(citiesByName,&c,&succ);
removeFromAVLTree(cities,&c,&succ);
insertIntoAVLTree(citiesByName,city1,&succ);
insertIntoAVLTree(cities,city1,&succ);
printf("City EDITED, press ENTER to continue...");
getchar();
fflush(stdin);
}


void searchCity()
{
int succ,*advWeight;
char name[52];
Pair *graphPair,*advPair,gPair;
AVLTree *advTree;
AVLTreeInOrderIterator advIterator;
City *city,c,*advCity;
printf("Enter a city name to search : ");
fgets(name,52,stdin);
fflush(stdin);
name[strlen(name)-1]='\0';
strcpy(c.name,name);
city=(City *)getFromAVLTree(citiesByName,&c,&succ);
if(succ)
{
printf("Found\n");
printf("%s\n",city->name);
}
else 
{
printf("%s doesn't exists\n",name);
return;
}
gPair.first=(void *)city;
graphPair=(Pair *)getFromAVLTree(graph,&gPair,&succ);
if(graphPair==NULL)return;
advTree=(AVLTree *)graphPair->second;
advIterator=getAVLTreeInOrderIterator(advTree,&succ);
printf("Following are the cities adjacent to %s\n",city->name);
while(hasNextInOrderElementInAVLTree(&advIterator))
{
advPair=(Pair *)getNextInOrderElementFromAVLTree(&advIterator,&succ);
advCity=(City *)advPair->first;
advWeight=(int *)advPair->second;
printf("City : %-18s | Distance : %d \n",advCity->name,*advWeight);
}
}


void addCity()
{
char m;
int succ,code;
FILE *file;
char name[52];
City c,*city;
printf("Enter name of the City you want to add : ");
fgets(name,52,stdin);
fflush(stdin);
name[strlen(name)-1]='\0';
strcpy(c.name,name);
city=(City *)getFromAVLTree(citiesByName,&c,&succ);
if(city!=NULL) 
{
printf("City already exists\n");
return;
}
printf("Do you wanna add City(Y/N) : ");
m=getchar();
fflush(stdin);
if(m!='Y' && m!='y')
{
printf("City not added\n");
return;
}
cityHeader.lastGeneratedCode++;
cityHeader.recordCount++;
code=cityHeader.lastGeneratedCode;
c.code=code;
file=fopen("city.dat","rb+");
if(file==NULL)
{
file=fopen("city.dat","wb+");
fwrite(&cityHeader,sizeof(CityHeader),1,file);
}
else
{
fseek(file,0,SEEK_END);
}
fwrite(&c,sizeof(City),1,file);
fseek(file,0,SEEK_SET);
fwrite(&cityHeader,sizeof(CityHeader),1,file);
fclose(file);
city=(City *)malloc(sizeof(City));
city->code=c.code;
strcpy(city->name,c.name);
insertIntoAVLTree(cities,city,&succ);
insertIntoAVLTree(citiesByName,city,&succ);
printf("City added\n");
drawLine('-',50);
printf("Press ENTER to continue...");
getchar();
fflush(stdin);
}

void searchRoute()
{
int distanceComparator(void *left,void *right)
{
Pair *leftPair,*rightPair;
int *leftDistance,*rightDistance;
leftPair=(Pair *)left;
rightPair=(Pair *)right;
leftDistance=(int *)leftPair->second;
rightDistance=(int *)rightPair->second;
return *leftDistance-*rightDistance;
}
typedef struct _path
{
City *city;
City *arrivedFromCity;
int totalDistanceFromStarting;
}Path;
int pathComparator(void *left,void *right)
{
Path *leftPath,*rightPath;
leftPath=(Path *)left;
rightPath=(Path *)right;
return leftPath->city->code-rightPath->city->code;;
}

AVLTreeInOrderIterator iterator;
int i,*td,*totalDistance,*totalDistanceFromSourceCity,*edgeWeight,sum,found,routeExists;
int succ;
Path *pathStructurePointer,pathStructure;
Pair *graphPair,gPair,*advPair;
AVLTree *pathTree,*advTree;
PQueue *pqueue;
Stack *stack;
Pair *tmpPair,*PQueuePair;
char sourceCityName[52],targetCityName[52];
City c,*city,*sourceCity,*targetCity,*advCity,*tmpCity;

printf("Enter SOURCE city : ");
fgets(sourceCityName,52,stdin);
fflush(stdin);
sourceCityName[strlen(sourceCityName)-1]='\0';
strcpy(c.name,sourceCityName);
sourceCity=(City *)getFromAVLTree(citiesByName,&c,&succ);
if(!succ)
{
printf("City DOESN'T exists\n");
return;
}

printf("Enter TARGET city : ");
fgets(targetCityName,52,stdin);
fflush(stdin);
targetCityName[strlen(targetCityName)-1]='\0';
strcpy(c.name,targetCityName);
targetCity=(City *)getFromAVLTree(citiesByName,&c,&succ);
if(!succ)
{
printf("City DOESN'T exists\n");
return;
}
pathTree=createAVLTree(&succ,pathComparator);
pqueue=createPQueue(distanceComparator,&succ);
PQueuePair=(Pair *)malloc(sizeof(Pair));
PQueuePair->first=sourceCity;
totalDistanceFromSourceCity=(int *)malloc(sizeof(int));
*totalDistanceFromSourceCity=0;
PQueuePair->second=totalDistanceFromSourceCity;
addToPQueue(pqueue,PQueuePair,&succ);
pathStructurePointer=(Path *)malloc(sizeof(Path));
pathStructurePointer->city=sourceCity;
pathStructurePointer->arrivedFromCity=NULL;
pathStructurePointer->totalDistanceFromStarting=0;
insertIntoAVLTree(pathTree,pathStructurePointer,&succ);
routeExists=0;
while(!isPQueueEmpty(pqueue))
{
PQueuePair=(Pair *)removeFromPQueue(pqueue,&succ);
city=(City *)PQueuePair->first;
if(city==targetCity)
{
routeExists=1;
break;
}
totalDistance=PQueuePair->second;
gPair.first=city;
graphPair=(Pair *)getFromAVLTree(graph,&gPair,&succ);
if(!succ)continue;
if(graphPair->second==NULL)continue;
advTree=(AVLTree *)graphPair->second;
iterator=getAVLTreeInOrderIterator(advTree,&succ);
while(hasNextInOrderElementInAVLTree(&iterator))
{
advPair=getNextInOrderElementFromAVLTree(&iterator,&succ);
advCity=(City *)advPair->first;
edgeWeight=(int *)advPair->second;
sum=*edgeWeight+*totalDistance;
pathStructure.city=advCity;
pathStructurePointer=getFromAVLTree(pathTree,&pathStructure,&succ);
if(succ)
{
if(sum>pathStructurePointer->totalDistanceFromStarting)continue;
pathStructurePointer->totalDistanceFromStarting=sum;
pathStructurePointer->arrivedFromCity=city;
found=false;
for(i=0;i<getSizeOfPQueue(pqueue);i++)
{
tmpPair=(Pair *)getElementFromPQueue(pqueue,i,&succ);
tmpCity=tmpPair->first;
if(tmpCity->code==advCity->code)
{
found=true;
break;
}
}
if(found)
{
*td=sum;
tmpPair->second=td;
updateElementInPQueue(pqueue,i,tmpPair,&succ);
continue;
}
PQueuePair=(Pair *)malloc(sizeof(Pair));
PQueuePair->first=advCity;
totalDistanceFromSourceCity=(int *)malloc(sizeof(int));
*totalDistanceFromSourceCity=sum;
PQueuePair->second=totalDistanceFromSourceCity;
addToPQueue(pqueue,PQueuePair,&succ);
}
else
{
PQueuePair=(Pair *)malloc(sizeof(Pair));
PQueuePair->first=advCity;
td=(int *)malloc(sizeof(int));
*td=sum;
PQueuePair->second=td;
addToPQueue(pqueue,PQueuePair,&succ);

pathStructurePointer=(Path *)malloc(sizeof(Path));
pathStructurePointer->city=advCity;
pathStructurePointer->arrivedFromCity=city;
pathStructurePointer->totalDistanceFromStarting=sum;
insertIntoAVLTree(pathTree,pathStructurePointer,&succ);
}
}
}
if(!routeExists)
{
printf("NO ROUTE EXISTS from %s to %s\n",sourceCityName,targetCityName);
return;
}
stack=createStack(&succ);
pathStructure.city=targetCity;
while(1)
{
pathStructurePointer=getFromAVLTree(pathTree,&pathStructure,&succ);
pushOnStack(stack,pathStructurePointer,&succ);
if(pathStructurePointer->city==sourceCity)break;
pathStructure.city=pathStructurePointer->arrivedFromCity;
}
while(!isStackEmpty(stack))
{
pathStructurePointer=(Path *)popFromStack(stack,&succ);

printf("%s (%d)\n",pathStructurePointer->city->name,pathStructurePointer->totalDistanceFromStarting);
}
}

void cityMainMenu()
{
int ch;
while(1)
{
printf("1. Add City\n");
printf("2. Remove City\n");
printf("3. Edit City\n");
printf("4. Display List of City\n");
printf("5. Add adjacent City\n");
printf("6. Remove adjacent City\n");
printf("7. Search City\n");
printf("8. Exit\n");
printf("Enter your choice : ");
scanf("%d",&ch);
fflush(stdin);
if(ch==1) addCity();
if(ch==2)removeCity();
if(ch==3)editCity();
if(ch==4)displayListOfCity();
if(ch==5)addAdjacentVertex(); 
if(ch==6)removeAdjacentVertex();
if(ch==7)searchCity();
if(ch==8)break;
}
}


int main()
{
int ch,success;
populateDataStructure(&success);
while(1)
{
printf("1. CITY MASTER\n");
printf("2. GET ROUTE\n");
printf("3. EXIT\n");
printf("Enter your choice : ");
scanf("%d",&ch);
fflush(stdin);
if(ch==1) cityMainMenu();
if(ch==2)searchRoute();
if(ch==3) break;
}
void releaseData();
return 0;
}