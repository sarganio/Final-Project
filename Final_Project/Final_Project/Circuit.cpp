#include "Circuit.h"


Circuit::Circuit(byte seed[SEQ_LEN], Party* party) : _party(party) {
	
	srand(*(unsigned int*)seed);

	_numOfLayers = rand() % 100 + 10;
	_circuit.resize(_numOfLayers);
	_circuit[0].resize(NUM_OF_PARTIES);
	_gatesPerLayer.push_back(3);

	//set the first layer with Parties Shares
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		_circuit[0][i]->setOutput(party->getShare(i));
	}
	
	
	for (int i = 1; i < _numOfLayers; i++) {
		_gatesPerLayer.push_back(rand() % 10 + 10);
		_circuit[i].resize(_gatesPerLayer[i]);

		for (int j = 0; j < _gatesPerLayer[i]; j++) {
			if (rand() % 10 > 2) { //Share

				if(true){//------------------TEMP------------------------------------
				//if (rand() % 3 == 1) { //addition gate
					int inputLayerLeft = rand() % i;
					int gateIndexLeft = rand() % j;
					int inputLayerRight = rand() % i;
					int gateIndexRight = rand() % j;

					_circuit[i][j] = new AddGate<Share>(_circuit[inputLayerLeft][gateIndexLeft]->getOutput(), _circuit[inputLayerRight][gateIndexRight]->getOutput());

				}
				else {// multiplication gate


				}
			}
			else { //const
			
			}
		}
		
	}


}