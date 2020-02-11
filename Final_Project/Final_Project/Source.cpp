#include "Party.h"
#include <iostream>
#include <thread>
#include <iostream>
#include<set>

using std::cout;
using std::endl;
using std::string;
using std::set;
//for F_{input}
//unsigned char seq[SEQ_LEN] = {};
//if (!RAND_bytes(seq, SEQ_LEN))
//throw std::exception("Failed to generate random bytes at" __FUNCTION__);
//
////"broadcast" the seq number to the other parties
//from.writeBuffer(seq, SEQ_LEN);
//to.writeBuffer(seq, SEQ_LEN);
//
//char toSeq[SEQ_LEN] = {};
//to.readBuffer(toSeq, SEQ_LEN);
////waiting for thread to receive the seq number of _id-1 party
//while (!fromSeq[0]);


//Just for vitaly & Osher!
int main(int argc, char* argv[]) {
	int i,myID = 0;
	string IPs[NUM_OF_PARTIES] = { string(argv[1]) };
	for (i = 2; i < argc; i++)
		if (IPs[0] > (IPs[i - 1] = string(argv[i])))//if my IP is bigger than the other IP increment my ID
			myID++;
	try {
		Party p = Party(myID,122323);
		p.connectToAllParties(IPs);
		
	}
	catch (std::exception & exc) {
		cout << exc.what() << endl;
	}
	cout << "Bye Bye!" << endl;
	getchar();
	return 0;
}