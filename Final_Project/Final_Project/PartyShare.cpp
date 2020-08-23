#include "PartyShare.h"

int PartyShare::correlatedRandomness(Party& p) const{
	Share* randomNumbers = p.fRand();//memory needs to be released!!
	return (*randomNumbers)[p.getID()].getValue() - (*randomNumbers)[(p.getID() + 2) % NUM_OF_PARTIES].getValue();
}
Share PartyShare::calculateOutputMull(MultiplicationGate<Share*>& g) {
	Share& multipicationOutput = *g.getOutput();
	Share& leftInput = *g.getLeft();
	Share& rightOutput = *(Share*)g.getRight();

	long firstPartOputput;//needs to compute Zi and send it to +1
	return multipicationOutput;
}