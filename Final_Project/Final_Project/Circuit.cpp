#include "Circuit.h"
#include "AddGate.h"
#include "MultiplicationGate.h"
#include "PartyShare.h"

#define MIN_NUM_OF_LAYERS 4
#define MIN_NUM_OF_GATES 2
#define RANGE_OF_LAYERS 1
#define RANGE_OF_GATES 2

Circuit::Circuit(byte seed[SEQ_LEN], Party* party) : _party(party) {
	
	//srand(*(unsigned int*)seed);

	_numOfLayers = rand() % RANGE_OF_LAYERS + MIN_NUM_OF_LAYERS;
	_circuit.resize(_numOfLayers);
	_circuit[0].resize(NUM_OF_PARTIES);
	_gatesPerLayer.push_back(3);

	//set the first layer with Parties Shares
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		_circuit[0][i] = new AddGate<Share>();
		_circuit[0][i]->setOutput(party->getShare(i));
	}
	
	
	for (int i = 1; i < _numOfLayers; i++) {
		_gatesPerLayer.push_back(rand() % RANGE_OF_GATES + MIN_NUM_OF_GATES);
		_circuit[i].resize(_gatesPerLayer[i]);

		for (int j = 0; j < _gatesPerLayer[i]; j++) {
			
			int inputLayerLeft = rand() % i;
			int gateIndexLeft = rand() % _gatesPerLayer[inputLayerLeft];

			if (rand() % 2 == 0) { //Share
				
				int inputLayerRight = rand() % i;
				int gateIndexRight = rand() % _gatesPerLayer[inputLayerRight];

				if(rand() % 2 == 0){//------------------TEMP------------need to set probability---------------------
					_circuit[i][j] = new AddGate<Share>(_circuit[inputLayerLeft][gateIndexLeft]->getOutput(), _circuit[inputLayerRight][gateIndexRight]->getOutput());
				}
				else {// multiplication gate
					_circuit[i][j] = new MultiplicationGate<PartyShare>(_circuit[inputLayerLeft][gateIndexLeft]->getOutput(),new PartyShare(_circuit[inputLayerRight][gateIndexRight]->getOutput(),_party));
				}
			}
			else { //const
				
				if (rand()%2) {//------------------TEMP------------------------------------
			
					_circuit[i][j] = new AddGate<long>(_circuit[inputLayerLeft][gateIndexLeft]->getOutput(), new long(rand() % 10 + 1));

				}
				else {// multiplication gate
					_circuit[i][j] = new MultiplicationGate<long>(_circuit[inputLayerLeft][gateIndexLeft]->getOutput(), new long(rand() % 10 + 1));
				}
			
			}
		}
		
	}


}
//int Circuit::correlatedRandomness(Party& p) const{
//	Share* randomNumbers = p.fRand();//memory needs to be released!!
//	return (*randomNumbers)[p.getID()].getValue() - (*randomNumbers)[(p.getID() + 2) % NUM_OF_PARTIES].getValue();
//}
//Share Circuit::calculateOutputMull(MultiplicationGate<Share*>& g) {
//	Share& multipicationOutput = *g.getOutput();
//	Share& leftInput = *g.getLeft();
//	Share& rightOutput = *(Share*)g.getRight();
//
//	long firstPartOputput;//needs to compute Zi and send it to +1
//	return multipicationOutput;
//}
void Circuit::calculateOutput() {
	for (int i = 1; i < _numOfLayers; i++) {
		for (int j = 0; j < _gatesPerLayer[i]; j++) {
			//if (typeid(*_circuit[i][j])==typeid(MultiplicationGate<Share>)) {
			//	unsigned int alpha = 0;//correlatedRandomness(*_party);
			//	std::cout << "Alpha is: " <<alpha<< std::endl;

			//}
			//else
				_circuit[i][j]->calculateOutput();
		}
	}
}
Share* Circuit::getOutput() {
	calculateOutput();
	unsigned short lastLayerIndex = _numOfLayers - 1;
	unsigned short lastGate = this->_gatesPerLayer[_numOfLayers - 1] - 1;
	return this->_circuit[lastLayerIndex][lastGate]->getOutput();///possible crush
}