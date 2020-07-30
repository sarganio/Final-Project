#pragma once
#include "Party.h"
#include "Share.h"
class Share;
class Party;
class PartyShare
{
		Share* _s;
		Party* _owner;
public:
	Share* getShare()const { return _s; }
	Party* getParty()const { return _owner; }
};

