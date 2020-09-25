#include "utility.h"

/*A function to set the position of cursor on the screen*/
void gotoxy(int x, int y)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	_COORD pos;
	pos.X = x;
	pos.Y = y;

	SetConsoleCursorPosition(hConsole, pos);
}
///////////////////////////////////////////////////////////////////////////////////////

/*A function to set the command window lenght and height for the game specification*/
void SetWindow()
{
	system("mode 150,50");   //Set mode to ensure window does not exceed buffer size
	SMALL_RECT WinRect = {0, 0, CmdWidth, CmdHeight};   //New dimensions for window in 8x12 pixel chars
	SMALL_RECT* WinSize = &WinRect;
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), true, WinSize);   //Set new size for window
}
///////////////////////////////////////////////////////////////////////////////////////

/*A function to color the cmd text*/
void color(int thecolor)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
	SetConsoleTextAttribute(hStdout, 
		thecolor|FOREGROUND_INTENSITY);
}
///////////////////////////////////////////////////////////////////////////////////////

/*A function to partition the castle into regions (A,B,C,D)*/
void DrawRegions(const castle & C)
{
	color(FOREGROUND_RED|FOREGROUND_BLUE);
	gotoxy(0, C.Ystrt + C.L/2);
	for(int i=0;i<C.W+2*C.Xstrt;i++)
		cout<<"-";
	for(int j=0;j<C.L+2*C.Ystrt;j++)
	{
		gotoxy(C.Xstrt + C.W/2, j);
		cout<<"|";		
	}
	color(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
}
///////////////////////////////////////////////////////////////////////////////////////

/*A function to draw the castle and the towers*/
void DrawCastle(const castle & C,int SimulationTime)
{
	system("CLS");
	
	//Draw the castle
	cout<<endl;
	gotoxy(C.Xstrt,C.Ystrt);
	for(int i=0;i<C.W;i++)
		cout<<"=";
	for(int j=1;j<C.L; j++)
	{
		gotoxy(C.Xstrt, C.Ystrt +j);
		cout<<"|";
		gotoxy(C.Xstrt+C.W-1, C.Ystrt +j);
		cout<<"|";
	}
	gotoxy(C.Xstrt,C.Ystrt+C.L-1);
	for(int i=0;i<C.W;i++)
		cout<<"=";

	//Draw towers
	int T1X,T2X,T3X,T4X,T1Y,T2Y,T3Y,T4Y;
	for (int i=0;i<4;i++)
	{

		if(i==0)
		{
			T1X=C.Xstrt+1;
			T1Y=C.Ystrt + TowerLength;
			gotoxy(T1X, T1Y);
			for(int i=0;i<TowerWidth;i++)
				cout<<"=";
			gotoxy(T1X +1, T1Y-1);
			cout<<"T1"<<"("<<C.towers[0].Health<<")";

			gotoxy(C.Xstrt+C.W/4, C.Ystrt+C.L/4);
			cout<<"(A)";
		}
		else if(i==1)
		{
			T2X=C.Xstrt+C.W-2-TowerWidth;
			T2Y=T1Y;
			gotoxy(T2X,T2Y );
			for(int i=0;i<TowerWidth+1;i++)
				cout<<"=";
			gotoxy(T2X +1, T2Y-1);
			cout<<"T2"<<"("<<C.towers[1].Health<<")";
			gotoxy(C.Xstrt+(3*C.W/4), C.Ystrt+C.L/4);
			cout<<"(B)";
		}
		else if(i==2)
		{
			T3X=T2X;
			T3Y=C.Ystrt + C.L - TowerLength-1;;
			gotoxy(T3X,T3Y );
			for(int i=0;i<TowerWidth+1;i++)
				cout<<"=";
			gotoxy(T3X +1, T3Y+1);
			cout<<"T3"<<"("<<C.towers[2].Health<<")";
			gotoxy(C.Xstrt+(3*C.W/4), C.Ystrt+(3*C.L/4)-1);
			cout<<"(C)";

		}
		else 
		{
			T4X=T1X;
			T4Y=C.Ystrt + C.L - TowerLength-1;
			gotoxy(T4X, T4Y);
			for(int i=0;i<TowerWidth;i++)
				cout<<"=";
			gotoxy(T4X +1, T4Y+1);
			cout<<"T4"<<"("<<C.towers[3].Health<<")";
			gotoxy(C.Xstrt+(C.W/4), C.Ystrt+(3*C.L/4)-1);
			cout<<"(D)";

		}
	}


	DrawRegions(C);

	//****************************
	gotoxy(CmdWidth/2 -10, CmdHeight-1);
	cout<<"Simulation Time : " << SimulationTime<<endl;
}

///////////////////////////////////////////////////////////////////////////////////////

/*A function to draw a single enemy using its distance from the castle*/
void DrawEnemy(const Enemy& E, int Ypos)
{
	int x,y;
	//First calcuale x,y position of the enemy on the output screen
	//It depends on the region and the enemy distance
	REGION EnemyRegion = E.Region;
	switch (EnemyRegion)
	{
	case A_REG:
		x = CastleXStrt-E.Distance+1;
		y = (CmdHeight/2)-(CastleLength/4)-Ypos;
		break;
	case B_REG:
		x = CastleXStrt+CastleWidth+E.Distance-1;
		y = (CmdHeight/2)-(CastleLength/4)-Ypos;
		break;
	case C_REG:
		x = CastleXStrt+CastleWidth+E.Distance-1;
		y = (CmdHeight/2)+(CastleLength/4)+Ypos;
		break;
	default:
		x= CastleXStrt-E.Distance+1;
		y= (CmdHeight/2)+(CastleLength/4)+Ypos;

	}

	gotoxy(x, y);

	//set enemy color according to it type
	Etype EnemyType = E.Type;
	switch(EnemyType)
	{
	case PAVER:
		color(FOREGROUND_GREEN);
		break;
	case FIGHTER:
		color(FOREGROUND_RED|FOREGROUND_GREEN);//Yellow
		break;
	default:
		color(FOREGROUND_RED);
	}

	
	cout<<(char)EnemyShape; //Draw the enemy

	color(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
}
///////////////////////////////////////////////////////////////////////////////////////

/*A function to draw a list of enemies and ensure there is no overflow in the drawing*/
//To use this function, you must prepare its input parameters as specified
//Input Parameters:
// enemies [ ] : array of enemy pointers (ALL enemies from all regions in one array)
// size : the size of the array (total no. of enemies)
void DrawEnemies(Enemy* enemies[],int size)
{
	//draw enemies at each region 
	for(int region=A_REG;region<=D_REG;region++)
	{
		int CountEnemies =0;	//count Enemies at the same distance to draw them vertically if they are <= 15 enemy else print number of enemy in the region
		bool draw=true;
		for(int distance=((CmdWidth/2)-(CastleWidth/2));distance>1;distance--)
		{
			CountEnemies=0;

			for(int i=0;i<size;i++)
			{	
				if(enemies[i]->Distance==distance && enemies[i]->Region == region)
				{
					CountEnemies++;
				}
			}
			if(CountEnemies>15)
			{
				draw=false;
				break;
			}

		}
		if(draw)
		{
			for(int distance=((CmdWidth/2)-(CastleWidth/2));distance>1;distance--)
			{
				CountEnemies=0;

				for(int i=0;i<size;i++)
				{	
					if(enemies[i]->Distance==distance && enemies[i]->Region == region)
					{
						DrawEnemy(*(enemies[i]),CountEnemies);
						CountEnemies++;
					}
				}

			}


		}
		else // print message maximum reached in this region
		{
			int x;int y;
			if(region==A_REG)
			{
				x= CastleXStrt-30;
				y= (CmdHeight/2)-(CastleLength/4);
			}
			else if(region==B_REG)
			{
				x= CastleXStrt+CastleWidth+30;
				y= (CmdHeight/2)-(CastleLength/4);


			}
			else if(region==C_REG)
			{
				x= CastleXStrt+CastleWidth+30;
				y= (CmdHeight/2)+(CastleLength/4);

			}
			else
			{
				x= CastleXStrt-30;
				y= (CmdHeight/2)+(CastleLength/4);

			}
			gotoxy(x, y);
			cout<<"Maximum limit";
		}

	}
	gotoxy(0, CmdHeight-1);
}
///////////////////////////////////////////////////////////////////////////////////////

/*Use this function to print a message, statistics to the user*/
void PrintMsg(char*msg)
{
	cout << msg;
}

void LoadFile(string filename, castle &cstl, float* constants, EnemyNode* &inactivelist)
{
	ifstream file;
	file.open(filename);
	float TH,TP;
	int N;
	file>>TH>>N>>TP;
	REGION Regs[4]={A_REG,B_REG,C_REG,D_REG};
	char RegsChars[4]={'A','B','C','D'};
	for(int i=0;i<4;i++)
	{
		cstl.towers[i].Health=TH;
		cstl.towers[i].N=N;
		cstl.towers[i].FirePower=TP;
		cstl.towers[i].Region=Regs[i];
	}
	for(int i=0;i<3;i++)
		file>>constants[i];
	EnemyNode *ptr;
	int S,TypeInd;
	Etype Types[3]={PAVER,FIGHTER,SHIELDED};
	char Reg;
	file>>S;
	while(S!=-1)
	{
		ptr=new EnemyNode;
		ptr->enemy.ID=S;
		file>>TypeInd;
		ptr->enemy.Type=Types[TypeInd];
		file>>ptr->enemy.ArrivalTime;
		InsertNode(inactivelist,ptr);
		file>>ptr->enemy.Health;
		file>>ptr->enemy.FirePower;
		file>>ptr->enemy.ReloadPeriod;
		ptr->enemy.RemainingTimeToShoot=0;
		file>>Reg;
		for(int i=0;i<4;i++)
		{
			if(Reg==RegsChars[i])
			{
				ptr->enemy.Region=Regs[i];
				break;
			}
		}
		file >> ptr->enemy.Speed;
		ptr->enemy.Distance=60;
		ptr->enemy.FD=-1;
		ptr->enemy.KD=-1;
		file>>S;
	}
	file.close();
}

void InsertNode(EnemyNode* &list, EnemyNode* node)
{
	if(!list)
	{
		list=node;
		node->next=NULL;
	}
	else if(node->enemy.Type==SHIELDED)
	{
		node->next=list;
		list=node;
	}
	else
	{
		EnemyNode *ptr1=list,*ptr2=list;
		while(ptr1->enemy.Type==SHIELDED)
		{
			if (!ptr1->next)
				break;
			ptr1=ptr1->next;
		}
		if (ptr1->next)
		{
			while (ptr1->enemy.ArrivalTime < node->enemy.ArrivalTime)
			{
				if (!ptr1->next)
				{
					ptr1->next = node;
					node->next = NULL;
					return;
				}
				ptr1 = ptr1->next;
			}
			if (ptr1 == ptr2)
			{
				node->next = list;
				list = node;
			}
			else
			{
				while (ptr2->next != ptr1)
					ptr2 = ptr2->next;
				ptr2->next = node;
				node->next = ptr1;
			}
		}
		else
		{
			ptr1->next = node;
			node->next = NULL;
		}
	}
}

void InsertNodeIntoDeletedList(EnemyNode* &list, EnemyNode* node)
{
	if (!list)
		list = node;
	else
	{
		EnemyNode *ptr1=list, *ptr2=list;
		while (ptr1)
		{
			if (ptr1 == ptr2)
			{
				node->next = list;
				list = node;
				break;
			}
			if (ptr1->enemy.FD >= node->enemy.FD)
			{
				while (ptr2->next != ptr1)
					ptr2 = ptr2->next;
				node->next = ptr2->next;
				ptr2->next = node;
				break;
			}
			else if (!ptr1->next)
			{
				while (ptr2->next)
					ptr2 = ptr2->next;
				ptr2->next = node;
				node->next = NULL;
			}
			ptr1 = ptr1->next;
		}
	}
}

bool RemoveNode(EnemyNode* &list, EnemyNode* &node)
{
	if(!list)
		return false;
	if(node==list)
	{
		list=list->next;
		node->next=NULL;
		return true;
	}
	else
	{
		EnemyNode *ptr1=list,*ptr2=list;
		while(ptr1)
		{
			ptr1=ptr1->next;
			if(!ptr1)
				return false;
			if(ptr1==node)
				break;
		}
		while(ptr2->next!=node)
			ptr2=ptr2->next;
		ptr2->next=node->next;
		node->next=NULL;
		return true;
	}
}

void AttackTower(Enemy* enemyptr, Tower* towerptr)
{
	towerptr->Health-=enemyptr->FirePower/enemyptr->Distance;
	if(towerptr->Health<0)
		towerptr->Health=0;
}

float AttackEnemy(Tower* towerptr, Enemy* enemyptr)
{
	float damage=towerptr->FirePower/enemyptr->Distance;
	if(enemyptr->Type==SHIELDED)
		damage/=2;
	enemyptr->Health-=damage;
	if(enemyptr->Health<0)
		enemyptr->Health=0;
	return damage;
}

bool EnemiesRemaining(EnemyNode** Lists)
{
	for(int i=0;i<5;i++)
		if(Lists[i])
			return true;
	return false;
}

bool TowersRemaining(castle* C)
{
	for(int i=0;i<3;i++)
		if(C->towers[i].Health!=0)
			return true;
	return false;
}

int CountEnemies(EnemyNode* list)
{
	int count=0;
	while(list)
	{
		count++;
		list=list->next;
	}
	return count;
}

int CountShieldedEnemies(EnemyNode* list)
{
	if(!list)
		return 0;
	EnemyNode* ptr=list;
	int count=0;
	while(ptr->enemy.Type==SHIELDED)
	{
		count++;
		ptr=ptr->next;
		if(!ptr)
			return count;
	}
	return count;
}

void UpdatePriorityList(PriorityNode* plist, EnemyNode* elist, float* constants)
{
	while(plist)
	{
		plist->data=constants[0]*elist->enemy.FirePower/elist->enemy.Distance
			+constants[1]/(elist->enemy.RemainingTimeToShoot)
			+constants[2]*elist->enemy.Health;
		plist=plist->next;
		elist=elist->next;
	}
}

void SortPriorityList(PriorityNode* &plist) //merge sort
{
	if (plist)
	{
		if (plist->next)
		{
			PriorityNode *SecondHalf=plist,*ptr=plist;
			while (ptr)
			{
				if (ptr->next)
				{
					SecondHalf = SecondHalf->next;
					ptr = ptr->next->next;
				}
				else
					ptr = ptr->next;
			}
			ptr = plist;
			while (ptr->next != SecondHalf)
				ptr = ptr->next;
			ptr->next = NULL;
			SortPriorityList(plist);
			SortPriorityList(SecondHalf);
			MergeSortedPriorityLists(plist, SecondHalf);
		}
	}
}

void MergeSortedPriorityLists(PriorityNode* &plist, PriorityNode* sublist2)
{
	PriorityNode* sublist1 = plist;
	plist = NULL;
	PriorityNode* ptr=plist;
	while (sublist1&&sublist2)
	{
		if (sublist1->data > sublist2->data)
		{
			if (!ptr)
			{
				plist = sublist1;
				ptr = plist;
			}
			else
			{
				ptr->next = sublist1;
				ptr = ptr->next;
			}
			sublist1 = sublist1->next;
		}
		else
		{
			if (!ptr)
			{
				plist = sublist2;
				ptr = plist;
			}
			else
			{
				ptr->next = sublist2;
				ptr = ptr->next;
			}
			sublist2 = sublist2->next;
		}
	}
	if (sublist1)
	{
		while (sublist1)
		{
			ptr->next = sublist1;
			ptr = ptr->next;
			sublist1 = sublist1->next;
		}
	}
	else
	{
		while (sublist2)
		{
			ptr->next = sublist2;
			ptr = ptr->next;
			sublist2 = sublist2->next;
		}
	}
}

void InsertPriorityNode(PriorityNode* &prioritylist, EnemyNode* enode)
{
	PriorityNode* pnode = new PriorityNode;
	pnode->next = prioritylist;
	prioritylist = pnode;
	pnode->enemynode = enode;
}

void RemovePriorityNode(PriorityNode* &prioritylist, EnemyNode* enode)
{
	PriorityNode* priorityptr = prioritylist;
	while (priorityptr)
	{
		if (priorityptr->enemynode == enode)
		{
			PriorityNode* priorityptr2 = prioritylist;
			if (priorityptr==priorityptr2)
			{
				prioritylist = prioritylist->next;
				delete priorityptr;
			}
			else
			{
				while (priorityptr2->next != priorityptr)
					priorityptr2 = priorityptr2->next;
				priorityptr2->next = priorityptr->next;
				delete priorityptr;
			}
			break;
		}
		priorityptr = priorityptr->next;
	}
}

void OutputEnemiesInfoToFile(EnemyNode* list, int timestep)
{
	if (list)
	{
		ofstream f;
		f.open("Output.txt", ios::app);
		EnemyNode* ptr = list;
		while (ptr)
		{
			f << timestep << '\t' << ptr->enemy.ID << '\t'
				<< ptr->enemy.FD << '\t' << ptr->enemy.KD << '\t' << ptr->enemy.FT << '\n';
			ptr = ptr->next;
		}
		f.close();
	}
}

void DeleteList(EnemyNode* &list)
{
	if (list)
	{
		EnemyNode *ptr1 = list, *ptr2 = list;
		while (ptr1)
		{
			ptr1 = ptr1->next;
			list = ptr1;
			delete ptr2;
			ptr2 = ptr1;
		}
		list = NULL;
	}
}

void MergePriorityLists(PriorityNode* &SrcList, PriorityNode* &DstList)
{
	if (SrcList)
	{
		PriorityNode* ptr = SrcList;
		while (ptr->next)
			ptr = ptr->next;
		ptr->next = DstList;
		DstList = SrcList;
		SrcList = NULL;
	}
}

void Pave(EnemyNode* list, int &UnpavedDistance, EnemyNode* &paver)
{
	paver = NULL;
	if (list)
	{
		EnemyNode *ptr = list;
		int Paved,MaxPaved=0;
		//find the enemy who will pave the most (depending on both distance and firepower)
		while (ptr)
		{
			if (ptr->enemy.Type == PAVER&&ptr->enemy.RemainingTimeToShoot==0)
			{
				Paved = (int)(UnpavedDistance - (ptr->enemy.Distance - ptr->enemy.FirePower));
				if (Paved > MaxPaved)
				{
					MaxPaved = Paved;
					paver = ptr;
				}
			}
			ptr = ptr->next;
		}
		if (paver) //the required paver found
		{
			UnpavedDistance -= MaxPaved;
			if (UnpavedDistance < 2)
				UnpavedDistance = 2;
			paver->enemy.Distance = UnpavedDistance;
			paver->enemy.RemainingTimeToShoot = paver->enemy.ReloadPeriod;
		}
	}
}

void DecrementRemainingTimeToShoot(EnemyNode* list)
{
	if (list)
	{
		EnemyNode* ptr = list;
		while (ptr)
		{
			if (ptr->enemy.RemainingTimeToShoot > 0)
				ptr->enemy.RemainingTimeToShoot--;
			ptr = ptr->next;
		}
	}
}

void OutputSimulationResultToFile(bool win, float* TowerTotalDamage, int* UnpavedDistance,
	int NumberOfKilledEnemies, int NumberOfAliveEnemies, int TotalFD, int TotalKD)
{
	ofstream fout;
	fout.open("Output.txt", ios::app);
	fout << endl;
	fout << "T1_Total_Damage\tT2_Total_Damage\tT3_Total_Damage\tT4_Total_Damage" << endl;
	fout << TowerTotalDamage[0] << "\t\t" << TowerTotalDamage[1] << "\t\t" << TowerTotalDamage[2] << "\t\t"
		<< TowerTotalDamage[3] << endl;
	fout << "R1_Distance\tR2_Distance\tR3_Distance\tR4_Distance" << endl;
	fout << UnpavedDistance[0] << "\t\t" << UnpavedDistance[1] << "\t\t" << UnpavedDistance[2] << "\t\t"
		<< UnpavedDistance[3] << endl;
	fout << endl;
	if (win)
	{
		fout << "Game is WIN" << endl;
		fout << "Total Enemies = " << NumberOfKilledEnemies << endl;
		fout << "Fight Delay Average = " << TotalFD / NumberOfKilledEnemies << endl;
		fout << "Kill Delay Average = " << TotalKD / NumberOfKilledEnemies;
	}
	else
	{
		fout << "Game is LOSS" << endl;
		fout << "Total Killed Enemies = " << NumberOfKilledEnemies << endl;
		fout << "Total Alive Enemies = " << NumberOfAliveEnemies << endl;
		fout << "Fight Delay Average = " << TotalFD / NumberOfKilledEnemies << endl;
		fout << "Kill Delay Average = " << TotalKD / NumberOfKilledEnemies << endl;
	}
	fout.close();
}