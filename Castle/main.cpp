#include <iostream>
using namespace std;


#include "utility.h"

int main()
{
	cout<<"Enter the simulation mode (1-3):"<<endl;
	cout<<"1 for interactive mode"<<endl;
	cout<<"2 for step-by-step mode"<<endl;
	cout<<"3 for silent mode"<<endl;
	int mode;
	cin>>mode;
	while (mode<1 || mode>3)
	{
		cout << "Please enter a valid simulation mode (1-3):" << endl;
		cout << "1 for interactive mode" << endl;
		cout << "2 for step-by-step mode" << endl;
		cout << "3 for silent mode" << endl;
		cin >> mode;
	}
	if (mode!=3)
		SetWindow();
	castle C;
	C.Xstrt = CastleXStrt;
	C.Ystrt = CastleYStrt;
	C.W = CastleWidth;
	C.L = CastleLength;
	float constants[3];
	EnemyNode* EnemyLists[5]; //List 0 is the inactive list. Lists 1 to 4 are regions A to D respectively.
	EnemyNode* DeletedList;
	EnemyLists[0]=NULL;
	LoadFile("Sample Input 1.txt",C,constants,EnemyLists[0]);
	for(int i=1;i<=4;i++)
		EnemyLists[i]=NULL;
	EnemyNode* enemyptr;
	PriorityNode* PriorityLists[4];
	for(int i=0;i<4;i++)
		PriorityLists[i]=NULL;
	int Enemies,ShieldedEnemies;
	Enemy** Attacked[4];
	for(int i=0;i<4;i++)
		Attacked[i]=new Enemy*[C.towers[i].N];
	int UnpavedDistance[]={30,30,30,30};
	int OldUnpavedDistance[] = { 30, 30, 30, 30 };
	int timestep=0;
	float TowerTotalDamage[] = { 0, 0, 0, 0 };
	int EnemiesKilledAtCurrentTimeStep[] = { 0, 0, 0, 0 };
	int TotalEnemiesKilled[] = { 0, 0, 0, 0 };
	int TotalFD = 0, TotalKD = 0;
	EnemyNode* paver = NULL;
	cin.ignore();
	ofstream f;
	f.open("Output.txt");
	f << "T\tS\tFD\tKD\tFT\n";
	f.close();
	//Draw initial state
	if (mode != 3)
	{
		DrawCastle(C, 0);
		DrawRegions(C);
		if (mode == 1)
		{
			PrintMsg("\nPress Enter to go to the next time step");
			cin.get();
		}
		else
			Sleep(1000);
	}
	while(EnemiesRemaining(EnemyLists)&&TowersRemaining(&C)) //while there are enemies or towers alive
	{
		timestep++;
		//Decrement remaining time to shoot for all enemies
		for (int i = 1; i <= 4; i++)
			DecrementRemainingTimeToShoot(EnemyLists[i]);
		//New Enemies Arrive
		{
			EnemyNode *ptr1 = EnemyLists[0], *ptr2;
			while (ptr1)
			{
				if (ptr1->enemy.ArrivalTime == timestep)
				{
					ptr2 = ptr1;
					ptr1 = ptr1->next;
					RemoveNode(EnemyLists[0], ptr2);
					int reg;
					switch (ptr2->enemy.Region)
					{
					case A_REG:
						InsertNode(EnemyLists[1], ptr2);
						reg = 0;
						break;
					case B_REG:
						InsertNode(EnemyLists[2], ptr2);
						reg = 1;
						break;
					case C_REG:
						InsertNode(EnemyLists[3], ptr2);
						reg = 2;
						break;
					case D_REG:
						InsertNode(EnemyLists[4], ptr2);
						reg = 3;
						break;
					}
					if (ptr2->enemy.Type == SHIELDED)
						InsertPriorityNode(PriorityLists[reg], ptr2);
				}
				else
					ptr1 = ptr1->next;
			}
		} //scope of ptr1 and ptr2
		//Attack
		for(int i=0;i<4;i++) //for each region
		{
			enemyptr=EnemyLists[i+1];
			//enemies attack tower
			while(enemyptr)
			{
				if(enemyptr->enemy.Type!=PAVER&&enemyptr->enemy.RemainingTimeToShoot==0)
				{
					AttackTower(&enemyptr->enemy,C.towers+i);
					enemyptr->enemy.RemainingTimeToShoot=enemyptr->enemy.ReloadPeriod;
				}
				enemyptr=enemyptr->next;
			}
			//tower attacks enemies
			Enemies=CountEnemies(EnemyLists[i+1]);
			if(Enemies>C.towers[i].N)
			{
				UpdatePriorityList(PriorityLists[i],EnemyLists[i+1],constants);
				SortPriorityList(PriorityLists[i]);
				ShieldedEnemies=CountShieldedEnemies(EnemyLists[i+1]);
				PriorityNode* priorityptr=PriorityLists[i];
				for(int j=0;j<ShieldedEnemies&&priorityptr;j++)
				{
					Attacked[i][j]=&priorityptr->enemynode->enemy;
					priorityptr=priorityptr->next;
				}
				enemyptr=EnemyLists[i+1];
				if(enemyptr)
					while(enemyptr->enemy.Type==SHIELDED)
						enemyptr=enemyptr->next;
				for (int j = ShieldedEnemies; j<C.towers[i].N; j++)
				{
					Attacked[i][j]=&enemyptr->enemy;
					enemyptr=enemyptr->next;
				}
				for(int j=0;j<C.towers[i].N;j++)
				{
					AttackEnemy(C.towers+i,Attacked[i][j]);
					if(Attacked[i][j]->FD==-1)
						Attacked[i][j]->FD=timestep-Attacked[i][j]->ArrivalTime;
				}
			}
			else if(Enemies>=1)
			{
				enemyptr=EnemyLists[i+1];
				while (enemyptr)
				{
					TowerTotalDamage[i] += AttackEnemy(C.towers + i, &enemyptr->enemy);
					if (enemyptr->enemy.FD == -1)
						enemyptr->enemy.FD = timestep - enemyptr->enemy.ArrivalTime;
					enemyptr = enemyptr->next;
				}
			}
		}
		//Killing
		DeletedList = NULL;
		for (int i = 0; i <4 ; i++)
		{
			EnemiesKilledAtCurrentTimeStep[i] = 0;
			enemyptr = EnemyLists[i+1];
			while (enemyptr)
			{
				if (enemyptr->enemy.Health==0)
				{
					enemyptr->enemy.KD = timestep - enemyptr->enemy.FD - enemyptr->enemy.ArrivalTime;
					enemyptr->enemy.FT = enemyptr->enemy.FD + enemyptr->enemy.KD;
					if (enemyptr->enemy.Type == SHIELDED)
						RemovePriorityNode(PriorityLists[i], enemyptr);
					EnemyNode* enemyptr2=enemyptr;
					enemyptr = enemyptr->next;
					RemoveNode(EnemyLists[i+1], enemyptr2);
					InsertNodeIntoDeletedList(DeletedList, enemyptr2);
					EnemiesKilledAtCurrentTimeStep[i]++;
					TotalEnemiesKilled[i]++;
					TotalFD += enemyptr2->enemy.FD;
					TotalKD += enemyptr2->enemy.KD;
				}
				else
					enemyptr = enemyptr->next;
			}
		}
		OutputEnemiesInfoToFile(DeletedList,timestep);
		DeleteList(DeletedList);
		for (int i = 0; i < 4; i++)
		{
			if (C.towers[i].Health == 0)
			{
				EnemyNode* enemyptr2;
				enemyptr = EnemyLists[i + 1];
				int DstReg;
				if (i == 3)
					DstReg = 0;
				else
					DstReg = i + 1;
				while (enemyptr)
				{
					enemyptr2 = enemyptr;
					enemyptr = enemyptr->next;
					RemoveNode(EnemyLists[i + 1], enemyptr2);
					InsertNode(EnemyLists[DstReg + 1], enemyptr2);
					if (enemyptr2->enemy.Distance < UnpavedDistance[DstReg])
						enemyptr2->enemy.Distance = UnpavedDistance[DstReg];
					switch (DstReg)
					{
					case 0:
						enemyptr2->enemy.Region = A_REG;
						break;
					case 1:
						enemyptr2->enemy.Region = B_REG;
						break;
					case 2:
						enemyptr2->enemy.Region = C_REG;
						break;
					case 3:
						enemyptr2->enemy.Region = D_REG;
						break;
					}
				}
				MergePriorityLists(PriorityLists[i], PriorityLists[DstReg]);
			}
		}
		//Move
		for (int i = 0; i < 4; i++)
		{
			Pave(EnemyLists[i + 1], UnpavedDistance[i], paver);
			enemyptr = EnemyLists[i + 1];
			while (enemyptr)
			{
				if (enemyptr != paver)
				{
					enemyptr->enemy.Distance -= enemyptr->enemy.Speed;
					if (enemyptr->enemy.Distance < OldUnpavedDistance[i])
						enemyptr->enemy.Distance = OldUnpavedDistance[i];
				}
				enemyptr = enemyptr->next;
			}
			OldUnpavedDistance[i] = UnpavedDistance[i];
		}
		//Drawing and printing statistics
		if (mode != 3)
		{
			DrawCastle(C, timestep);
			DrawRegions(C);
			int NumberOfEnemies[4];
			Enemy** ArrayOfEnemyPtrs;
			for (int i = 0; i < 4; i++)
			{
				NumberOfEnemies[i] = CountEnemies(EnemyLists[i + 1]);
				ArrayOfEnemyPtrs = new Enemy*[NumberOfEnemies[i]];
				enemyptr = EnemyLists[i + 1];
				int j = 0;
				while (enemyptr)
				{
					ArrayOfEnemyPtrs[j] = &enemyptr->enemy;
					enemyptr = enemyptr->next;
					j++;
				}
				DrawEnemies(ArrayOfEnemyPtrs, NumberOfEnemies[i]);
				delete[] ArrayOfEnemyPtrs;
			}
			PrintMsg("Information for each region:\n");
			PrintMsg("Total number of current enemies:");
			cout << "\tA=" << NumberOfEnemies[0] << "\tB=" << NumberOfEnemies[1] <<
				"\tC=" << NumberOfEnemies[2] << "\tD=" << NumberOfEnemies[3] << "\n";
			PrintMsg("Number of enemies killed at last time step:");
			cout << "\tA=" << EnemiesKilledAtCurrentTimeStep[0]
				<< "\tB=" << EnemiesKilledAtCurrentTimeStep[1]
				<< "\tC=" << EnemiesKilledAtCurrentTimeStep[2]
				<< "\tD=" << EnemiesKilledAtCurrentTimeStep[3] << "\n";
			PrintMsg("Total number of enemies killed:");
			cout << "\tA=" << TotalEnemiesKilled[0] << "\tB=" << TotalEnemiesKilled[1] <<
				"\tC=" << TotalEnemiesKilled[2] << "\tD=" << TotalEnemiesKilled[3] << "\n";
			PrintMsg("Unpaved Distance:");
			cout << "\tA=" << UnpavedDistance[0] << "\tB=" << UnpavedDistance[1] <<
				"\tC=" << UnpavedDistance[2] << "\tD=" << UnpavedDistance[3] << "\n";
			switch (mode)
			{
			case 1:
				PrintMsg("Press Enter to go to the next time step");
				cin.get();
				break;
			case 2:
				Sleep(1000);
				break;
			}
		}
	}
	for (int i = 0; i < 4; i++)
	{
		if (UnpavedDistance[i] < 2)
			UnpavedDistance[i] = 2;
	}
	//Output file
	int NumberOfKilledEnemies = TotalEnemiesKilled[0] + TotalEnemiesKilled[1]
		+ TotalEnemiesKilled[2] + TotalEnemiesKilled[3];
	int NumberOfAliveEnemies;
	bool win = TowersRemaining(&C);
	if (!win)
	{
		NumberOfAliveEnemies = CountEnemies(EnemyLists[1]) + CountEnemies(EnemyLists[2])
			+ CountEnemies(EnemyLists[3]) + CountEnemies(EnemyLists[4]) + CountEnemies(EnemyLists[0]);
	}
	OutputSimulationResultToFile(win, TowerTotalDamage, UnpavedDistance, NumberOfKilledEnemies,
		NumberOfAliveEnemies, TotalFD, TotalKD);
	//Delete dynamically created data
	for (int i = 0; i<4; i++)
		delete[] Attacked[i];
}