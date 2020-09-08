#include "Circuit.h"
#include "AddGate.h"
#include "MultiplicationGate.h"
#include "PartyShare.h"

#define MIN_NUM_OF_LAYERS 15
#define MIN_NUM_OF_GATES 15
#define RANGE_OF_LAYERS 1
#define RANGE_OF_GATES 2

Circuit::Circuit(byte seed[SEQ_LEN], Party* party) : _party(party) {
	
	srand(20);
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

				if(rand() % 4 ==1){//------------------TEMP------------
					_circuit[i][j] = new AddGate<Share>(_circuit[inputLayerLeft][gateIndexLeft]->getOutput(), _circuit[inputLayerRight][gateIndexRight]->getOutput());
				}
				else {// multiplication gate
					Share* toDelete = _circuit[inputLayerRight][gateIndexRight]->getOutput();
					_circuit[inputLayerRight][gateIndexRight]->setOutput(new PartyShare(toDelete, _party));
					delete toDelete;
					_circuit[i][j] = new MultiplicationGate<PartyShare>(_circuit[inputLayerLeft][gateIndexLeft]->getOutput(),(PartyShare*) _circuit[inputLayerRight][gateIndexRight]->getOutput());
				}
			}
			else { //const
				
				if (rand()%2) {//------------------TEMP------------------------------------
			
					_circuit[i][j] = new AddGate<long>(_circuit[inputLayerLeft][gateIndexLeft]->getOutput(), new long(rand() % P));

				}
				else {// multiplication gate
					_circuit[i][j] = new MultiplicationGate<long>(_circuit[inputLayerLeft][gateIndexLeft]->getOutput(), new long(rand() % P));
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
				_circuit[i][j]->calculateOutput();
				std::cout<<"Gate:"<<i<<" "<<j<<": "<<_party->finalReconstruct(*_circuit[i][j]->getOutput())<<std::endl;

		}
	}
}
Share* Circuit::getOutput() {
	calculateOutput();
	unsigned short lastLayerIndex = _numOfLayers - 1;
	unsigned short lastGate = this->_gatesPerLayer[_numOfLayers - 1] - 1;
	return this->_circuit[lastLayerIndex][lastGate]->getOutput();///possible crush
}