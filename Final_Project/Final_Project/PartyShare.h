#pragma once
#include "Party.h"
#include "Share.h"

class Party;

class PartyShare
{
		Party* _owner;
public:
	//Share* getShare()const { return new Share(); }
	Party* getParty()const { return _owner; }
};

