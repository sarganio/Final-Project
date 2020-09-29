#include "Party.h"
#include "Helper.h"

#include "MultiplicationGate.h"
#include "PartyShare.h" 
#include <iostream>
#include <set>
#include <condition_variable>
using std::cout;
using std::endl;
using std::string;
using std::set;
/*
argv[1] - my IP
argv[2],argv[3] - IPs of other parties
*/

int main(int argc, char* argv[]) {
	cout << sizeof(Message);
	
	short i,myID = 0;
	string IPs[NUM_OF_PARTIES] = { string(argv[1]) };
	//determine the ID of this party based on the IPs provided to the program.
	for (i = 2; i < argc; i++) {
		IPs[i - 1] = string(argv[i]);
		if (Helper::IPCompare(IPs[0], IPs[i - 1]))//if my IP is bigger than the other IP increment my ID
			myID++;
	}
	
	cout << "My ID:"<<myID << endl;
	try {
		Party p = Party(myID,1);
		//p.verifyRound1();
		p.connectToAllParties(IPs);
		p.fInput();
		Share& circuitOutput= *p.calcCircuit();
		long result = p.finalReconstruct(circuitOutput);
		cout << "The result of the function is:" << result << endl;
		cout<<"The result is:"<< circuitOutput.toString() << endl;
		//byte PI[120]{};
		//for (int i = 0; i < 120; i++)
		//	PI[i] = i + 48;
		//p.sendTo(1,F_VERIFY_ROUND1_MESSAGE, PI);
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