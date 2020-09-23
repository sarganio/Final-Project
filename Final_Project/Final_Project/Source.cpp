#include "Party.h"
#include "Helper.h"

#include "MultiplicationGate.h"
#include "PartyShare.h" 
#include <iostream>
#include <set>
using std::cout;
using std::endl;
using std::string;
using std::set;
/*
argv[1] - my IP
argv[2],argv[3] - IPs of other parties
*/

int main(int argc, char* argv[]) {
	//NTL::ZZ p;
	//NTL::GenPrime(p, 5);
	//NTL::ZZ_p::init(p);
	//NTL::ZZ_pX f;
	//NTL::vec_ZZ_p x, y;
	//x.SetLength(4);
	//y.SetLength(4);
	//for (int i = 0; i < 4; i++)
	//	x[i] = i;
	////for (int i = 0; i < 10; i++)
	////	y[i] = 2 * i;
	//y[0] = 0;
	//y[1] = 0;
	//y[2] = 2;
	//y[3] = 6;
	//NTL::interpolate(f, x, y);
	//cout << f;
	//getchar();






	short i,myID = 0;
	string IPs[NUM_OF_PARTIES] = { string(argv[1]) };
	//determine the ID of this party based on the IPs provided to the program.
	for (i = 2; i < argc; i++) {
		IPs[i - 1] = string(argv[i]);
		if (Helper::IPCompare(IPs[0], IPs[i - 1]))//if my IP is bigger than the other IP increment my ID
			myID++;
	}
	
	cout << "My ID id: "<<myID << endl;
	try {
		Party p = Party(myID,1);
		p.verifyRound1();
		p.connectToAllParties(IPs);
		p.fInput();
		Share& circuitOutput= *p.calcCircuit();
		long result = p.finalReconstruct(circuitOutput);
		cout << "The result of the function is:" << result << endl;
		cout<<"The result is:"<< circuitOutput.toString() << endl;
		p.fVerify();
	}
	catch (std::exception & exc) {
		//TO DO: send an abort message.
		cout << exc.what() << endl;
	}
	
	TRACE("Bye Bye!");
	
	getchar();
	
	return 0;
}