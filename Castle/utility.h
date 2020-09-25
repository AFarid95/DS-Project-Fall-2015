#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

#include <windows.h>

//The four regions: A, B, C , and D
enum REGION {	
	A_REG,
	B_REG,
	C_REG,
	D_REG
};

//Enemy types: Paver, Fighter, and Shielded Fighter
enum Etype {
	PAVER,
	FIGHTER,
	SHIELDED
};



//constants
#define CmdWidth		150
#define CmdHeight		50
#define CastleWidth		30
#define CastleLength	20
#define CastleXStrt		(CmdWidth/2-(CastleWidth/2))
#define CastleYStrt		(CmdHeight/2-(CastleLength/2))
#define TowerWidth      7
#define TowerLength     3

#define EnemyShape		219  //ASCII code of enemy char shape 



//Tower
struct Tower
{
	int Width; //Tower width
	int Height; //Tower Height
	float Health;
	int N; // maximum numbers of enemies a tower can attack at any time step 
	float FirePower; // tower fire power 
	REGION Region;
};

struct castle
{
	//starting x,y
	int Xstrt;
	int Ystrt;
	int W;	//width
	int L;  //Height
	Tower towers[4];	//Castle has 4 towers
};

struct Enemy
{
	int ID;			//Each enemy has a unique ID (sequence number)
	REGION Region;	//Region of this enemy
	int Distance;	//Distance to the castle
	float Health;	//Enemy health
	Etype Type;		//PVR, FITR, SHLD_FITR
	int ArrivalTime;
	float FirePower;
	int ReloadPeriod;
	int RemainingTimeToShoot;
	int FD;
	int KD;
	int FT;
	int Speed;
};

struct EnemyNode
{
	Enemy enemy;
	EnemyNode* next;
};

struct PriorityNode
{
	float data;
	EnemyNode* enemynode;
	PriorityNode* next; 
};

//Functions Prototype

/*A function to set the position of cursor on the screen*/
void gotoxy(int x, int y);

/*A function to set the command window lenght and height for the game specification*/
void SetWindow();

/*A function to color the cmd text*/
void color(int thecolor);

/*A function to partition the castle into regions (A,B,C,D)*/
void DrawRegions(const castle & C);

/*A function to draw the castle and the towers*/
void DrawCastle(const castle & C,int SimulationTime);

/*A function to draw a single enemy using its distance from the castle*/
void DrawEnemy(const Enemy& E, int Ypos=0);

/*A function to draw a list of enemies exist in all regions and ensure there is no overflow in the drawing*/
void DrawEnemies(Enemy* Enemies[],int size);

/*Student use this function to print his/her message*/
void PrintMsg(char*msg);

//Load enemy info from file
void LoadFile(string filename, castle &cstl, float* constants, EnemyNode* &inactivelist);

//Insert enemy in linked list according to its priority in being attacked
void InsertNode(EnemyNode* &list, EnemyNode* node);

//Insert enemy in linked list ordered ascendingly by FD. This list will be output to the output file
//before being deleted
void InsertNodeIntoDeletedList(EnemyNode* &list, EnemyNode* node);

//Removes enemy from linked list and returns the enemy node
bool RemoveNode(EnemyNode* &list, EnemyNode* &node);

//Enemy damages tower
void AttackTower(Enemy* Enemyptr, Tower* towerptr);

//Tower damages enemy. Returns the damage to be able to calculate the total damage of the towers
float AttackEnemy(Tower* towerptr, Enemy* enemyptr);

//True if there are enemies remaining
bool EnemiesRemaining(EnemyNode** Lists);

//True if there are towers remaining
bool TowersRemaining(castle* C);

//Counts enemies in a linked list
int CountEnemies(EnemyNode* list);

//Counts shielded enemies in a linked list
int CountShieldedEnemies(EnemyNode* list);

//Calculates priorities of shielded enemies in being attacked
void UpdatePriorityList(PriorityNode* plist, EnemyNode* elist, float* constants);

//Sorts priorities of shielded enemies in being attacked by merge sort
void SortPriorityList(PriorityNode* &plist);

//Used by SortPriorityList function
void MergeSortedPriorityLists(PriorityNode* &plist, PriorityNode* sublist2);

//Insert priority node in priority list
void InsertPriorityNode(PriorityNode* &prioritylist, EnemyNode* enode);

//Remove priority node from priority list
void RemovePriorityNode(PriorityNode* &prioritylist, EnemyNode* enode);

//Output the info of all enemies in the linked list to the output file
void OutputEnemiesInfoToFile(EnemyNode* list, int timestep);

//Deletes a linked list of enemies
void DeleteList(EnemyNode* &list);

//Merges two priority lists in one list
void MergePriorityLists(PriorityNode* &SrcList, PriorityNode* &DstList);

//Pave unpaved distance. The new unpaved distance and the enemy who paved are returned. Only the
//enemy who will cause the maximum paving will pave
void Pave(EnemyNode* list, int &UnpavedDistance, EnemyNode* &paver);

//Decrements remaining time to shoot for all the enemies in the list
void DecrementRemainingTimeToShoot(EnemyNode* list);

//Output simulation results to the output file at the end of the simulation
void OutputSimulationResultToFile(bool win, float* TowerTotalDamage, int* UnpavedDistance,
	int NumberOfKilledEnemies, int NumberOfAliveEnemies, int TotalFD, int TotalKD);