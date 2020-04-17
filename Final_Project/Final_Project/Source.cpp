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

	InputPart* a0 = new InputPart(1, 0, '0');
	InputPart* a1 = new InputPart(1, 1, '0');
	InputPart* a2 = new InputPart(2, 0, '1');
	InputPart* a3 = new InputPart(2, 1, '1');
	InputPart* a4 = new InputPart(3, 0, '0');
	InputPart* a5 = new InputPart(3, 1, '0');

	Share s0(a0, a1);
	Share s1(a2, a3);
	Share s2(a4, a5);

	cout << a0->toString() << endl;//(0,1,'a')
	cout << s0.toString() << endl; //(1,1)
	cout << s1.toString() << endl;//(2,2)
	cout << s2.toString() << endl;//(3,3)
	cout << a0->getIndex() << endl;
	cout << a1->getIndex() << endl;
	Share s3 = s0 * 10;

	cout << s3.toString() << endl;//(4,4)

	//s3 = s3 * 10;

	//cout << s3.toString() << endl;//(40,40)
 //





















	short i,myID = 0;
	string IPs[NUM_OF_PARTIES] = { string(argv[1]) };
	for (i = 2; i < argc; i++) {
		IPs[i - 1] = string(argv[i]);
		if (Helper::IPCompare(IPs[0], IPs[i - 1]))//if my IP is bigger than the other IP increment my ID
			myID++;
	}
	cout << myID << endl;
	try {
		Party p = Party(myID,122323);
		p.connectToAllParties(IPs);
		p.calcSeq();
		//p.fInput();	
	}
	catch (std::exception & exc) {
		cout << exc.what() << endl;
	}
	
	TRACE("Bye Bye!");
	getchar();
	return 0;
}