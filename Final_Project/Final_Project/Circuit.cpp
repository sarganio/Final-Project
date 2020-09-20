#include "Circuit.h"
#include "AddGate.h"
#include "MultiplicationGate.h"
#include "PartyShare.h"

#define MIN_NUM_OF_LAYERS 15
#define MIN_NUM_OF_GATES 15
#define RANGE_OF_LAYERS 1
#define RANGE_OF_GATES 2

Circuit::Circuit(byte seed[SEQ_LEN], Party* party) : _party(party) {
	
	srand(21);
	//srand(*(unsigned int*)seed);

	_numOfLayers = rand() % RANGE_OF_LAYERS + MIN_NUM_OF_LAYERS;
	_circuit.resize(_numOfLayers);
	_circuit[0].resize(NUM_OF_PARTIES);
	_gatesPerLayer.push_back(3);

	//set the first layer with Parties Shares
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		_circuit[0][i] = new AddGate<Share>();
		_circuit[0][i]->setOutput(new PartyShare(party->getShare(i),_party));
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
					//take the output of some previuos layer and extend it.From share to be PartyShare
					//_circuit[inputLayerRight][gateIndexRight]->setOutput(new PartyShare(_circuit[inputLayerRight][gateIndexRight]->getOutput(), _party));
					//create the multiplication with share gate
					_circuit[inputLayerRight][gateIndexRight]->getOutput()->setParty(_party);
					_circuit[i][j] = new MultiplicationGate<PartyShare>(_circuit[inputLayerLeft][gateIndexLeft]->getOutput(),_circuit[inputLayerRight][gateIndexRight]->getOutput());
					_numOfMulGates++;
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
	return this->_circuit[lastLayerIndex][lastGate]->getOutput();
}
Circuit::~Circuit() {
	for(int i = 0;i< this->_numOfLayers;i++)
		for(int j = 0;j<this->_gatesPerLayer[i];j++)
			if (_circuit[i][j]) {
				delete _circuit[i][j];
				_circuit[i][j] = nullptr;
			}
}