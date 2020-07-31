#pragma once
#include "Party.h"
#include "Share.h"

class Party;
class Share;

class PartyShare
{
		Share* _s;
		Party* _owner;
public:
	Share* getShare()const { return _s; }
	Party* getParty()const { return _owner; }
};

