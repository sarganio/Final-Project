
#include "Party.h"
#include "Helper.h"
#include <iostream>
#include <set>

#include "MultiplicationGate.h"
#include "PartyShare.h"

using std::cout;
using std::endl;
using std::string;
using std::set;

/*
TODO: add encryption (TLS).
	  D'tor of Party.
*/
/*
argv[1] - my IP
argv[2],argv[3] - IPs of other parties
*/

//Just for vitaly & Osher!
int main(int argc, char* argv[]) {
	//Party p = Party(1, 100);
	//Share s1(0, 'a');
	//Share s2(0, 'b');
	//Share s3(0, 'c');

	//s1[0].setValue(1);
	//s1[1].setValue(2);

	//s2[0].setValue(3);
	//s2[1].setValue(4);
	//
	//s3[0].setValue(5);
	//s3[1].setValue(6);

	//p.setShare(&s1, 0);
	//p.setShare(&s2, 1);
	//p.setShare(&s3, 2);
	//byte seq[4] = { 1,2,3,4 };
	//Circuit c(seq, &p);
	//c.calculateOutput();

	
	short i,myID = 0;
	string IPs[NUM_OF_PARTIES] = { string(argv[1]) };
	for (i = 2; i < argc; i++) {
		IPs[i - 1] = string(argv[i]);
		if (Helper::IPCompare(IPs[0], IPs[i - 1]))//if my IP is bigger than the other IP increment my ID
			myID++;
	}
	cout << myID << endl;
	try {
		Party p = Party(myID,1);
		p.connectToAllParties(IPs);
		p.fInput();
		Share* circuitOutput= p.calcCircuit();
		long result = p.reconstruct(p.getAllShares());
		cout << "The result of the function is:" << result << endl;

		cout<<"The result is:"<< p.calcCircuit()->toString() << endl;
	}
	catch (std::exception & exc) {
		cout << exc.what() << endl;
	}
	
	TRACE("Bye Bye!");
	
	getchar();
	
	return 0;
}