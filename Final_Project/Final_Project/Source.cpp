#include "Party.h"
#include <iostream>
#include <thread>
using std::cout;
using std::endl;

//Just for vitaly & Osher!
int main() {

	try {
		Party p = Party(1, 122323);
		p.connectToAllParties();
	}
	catch (std::exception & exc) {
		cout << exc.what() << endl;
	}
	cout << "Bye Bye!" << endl;
	getchar();
	return 0;
}