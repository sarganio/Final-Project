#include "Party.h"
#include <iostream>
#include <thread>
#include <iostream>
#include<set>
#include "Helper.h"
#include "Share.h"

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
	short i,myID = 0;
	string IPs[NUM_OF_PARTIES] = { string(argv[1]) };
	for (i = 2; i < argc; i++) {
		IPs[i - 1] = string(argv[i]);
		if (Helper::IPCompare(IPs[0], IPs[i - 1]))//if my IP is bigger than the other IP increment my ID
			myID++;
	}
	cout << myID << endl;
	try {
		Party p = Party(myID,1000);
		p.connectToAllParties(IPs);
		p.fInput();
	}
	catch (std::exception & exc) {
		cout << exc.what() << endl;
	}
	
	TRACE("Bye Bye!");
	getchar();
	return 0;
}