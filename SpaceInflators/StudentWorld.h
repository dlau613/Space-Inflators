#ifndef _STUDENTWORLD_H_
#define _STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "actor.h"
#include <list>
#include <vector>
#include <stdlib.h>

using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, actor.h, and actor.cpp
class Actor;
class StudentWorld : public GameWorld
{
public:

	virtual void init();
   

	virtual int move();
    

	virtual void cleanUp();
	
    bool addActor(int type, int x, int y, int firedBy);
	int getRoundNumber() const;
	double getEnergyPct() const;
	int numberOfActiveAliens() const;
	void updateDisplayText();
	list<Alien*> getCollidingAliens(Actor* a);
	PlayerShip* getCollidingPlayer(Actor* a);
	PlayerShip* getPlayerShip();
	int getNumAlienFiredProjectiles() const;
	string formatDisplay(int score, int round, double energyPercent, int torpedos, int shipsLeft);
	StudentWorld();
	~StudentWorld();
private:
	vector<Actor*> m_actors;
	int m_nStars;
	PlayerShip* ptrToShip;
	int m_nActiveAliens;
	int m_RoundNumber;
	int m_AliensDestroyed;
	
};

#endif // _GAMEWORLD_H_
