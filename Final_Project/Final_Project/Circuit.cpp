#include "Circuit.h"
#include "AddGate.h"
#include "MultiplicationGate.h"


#define MIN_NUM_OF_LAYERS 10
#define MIN_NUM_OF_GATES 2
#define RANGE_OF_LAYERS 1
#define RANGE_OF_GATES 1

#define PERCENTAGE_FOR_MUL_GATE 100 // 1-100
 

Circuit::Circuit(byte seed[SEQ_LEN], Party* party) : _party(party), _numOfMulGates(0) {
	 
	//srand with constant is for debug only - use seed for release
	srand(24);
	//srand(*(unsigned int*)seed);

	_numOfLayers = rand() % RANGE_OF_LAYERS + MIN_NUM_OF_LAYERS;
	_circuit.resize(_numOfLayers);
	_gatesPerLayer.resize(_numOfLayers);
	_circuit[0].resize(NUM_OF_PARTIES);
	_gatesPerLayer[0] = NUM_OF_PARTIES;

	//set the first layer with Parties Shares
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		_circuit[0][i] = new AddGate<Share>();
		_circuit[0][i]->setOutput(new PartyShare(party->getShare(i),_party));
	}
	
	
	for (int i = 1; i < _numOfLayers; i++) {
		_gatesPerLayer[i] = (rand() % RANGE_OF_GATES + MIN_NUM_OF_GATES);
		_circuit[i].resize(_gatesPerLayer[i]);

		for (int j = 0; j < _gatesPerLayer[i]; j++) {
			
			//Layer and gae index for the left input to gate
			int inputLayerLeft = rand() % i;
			int gateIndexLeft = rand() % _gatesPerLayer[inputLayerLeft];

			if (rand() % 2) { // add new gate with share - else add new gate with const
				
				//Layer and gae index for the right input to gate
				int inputLayerRight = rand() % i;
				int gateIndexRight = rand() % _gatesPerLayer[inputLayerRight];
				
				if(rand() % 100 > PERCENTAGE_FOR_MUL_GATE){
					//add a add gate
					_circuit[i][j] = new AddGate<Share>(_circuit[inputLayerLeft][gateIndexLeft]->getOutput(), _circuit[inputLayerRight][gateIndexRight]->getOutput());
				}
				else {// multiplication gate
					//add a multiplication gate
					//take the output of some previuos layer and extend it.From share to be PartyShare
					//create the multiplication with share gate
					_circuit[inputLayerRight][gateIndexRight]->getOutput()->setParty(_party);
					_circuit[i][j] = new MultiplicationGate<PartyShare>(_circuit[inputLayerLeft][gateIndexLeft]->getOutput(),_circuit[inputLayerRight][gateIndexRight]->getOutput());
					//count multiplication gates
					_numOfMulGates++;
				}
			}
			else { //const
				
				if (rand() % 100 > PERCENTAGE_FOR_MUL_GATE) {
					// add gate
					_circuit[i][j] = new AddGate<long>(_circuit[inputLayerLeft][gateIndexLeft]->getOutput(), new long(rand() % ZP));
				}
				else {
					// multiplication gate
					_circuit[i][j] = new MultiplicationGate<long>(_circuit[inputLayerLeft][gateIndexLeft]->getOutput(), new long(rand() % ZP));
				}
			
			}
		}
		
	}
	TRACE("Arithmetic circuit was created succssesfully")
}

void Circuit::calculateOutput() {
	for (int i = 1; i < _numOfLayers; i++) {
		for (int j = 0; j < _gatesPerLayer[i]; j++) {
				std::cout << "Gate:" << i << " " << j << ": ";
				//calculte the output of gate[i][j]
				_circuit[i][j]->calculateOutput();
				//reconstruct the output for debugging, remove when banchmarking
				cout<<"Output:"<< _party->finalReconstruct(*_circuit[i][j]->getOutput()) << std::endl;
		}
	}
}

Share* Circuit::getOutput() {
	calculateOutput();
	unsigned short lastLayerIndex = _numOfLayers - 1;
	unsigned short lastGate = this->_gatesPerLayer[_numOfLayers - 1] - 1;

	//output of thr circuit defined to be the last gate output
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

unsigned int Circuit::getNumOfMulGates()const {
	return _numOfMulGates;
}
