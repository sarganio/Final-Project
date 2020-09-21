
#include "Party.h"
#include "Helper.h"
#include <iostream>
#include <set>

#include "MultiplicationGate.h"
#include "PartyShare.h"

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "interpolation.h"

using namespace alglib;

using std::cout;
using std::endl;
using std::string;
using std::set;

/*
argv[1] - my IP
argv[2],argv[3] - IPs of other parties
*/

//Just for vitaly & Osher!
int main(int argc, char* argv[]) {
	{	Party p = Party(0, 1);}
	//
	// Here we demonstrate polynomial interpolation and differentiation
	// of y=x^2-x sampled at [0,1,2]. Barycentric representation of polynomial is used.
	//
	real_1d_array x = "[0,1,2]";
	real_1d_array y = "[0,0,2]";
	double t = -1;
	double v;
	double dv;
	double d2v;
	barycentricinterpolant p;

	// barycentric model is created
	polynomialbuild(x, y, p);

	// barycentric interpolation is demonstrated
	v = barycentriccalc(p, t);
	printf("%.4f\n", double(v)); // EXPECTED: 2.0

	// barycentric differentation is demonstrated
	barycentricdiff1(p, t, v, dv);
	printf("%.4f\n", double(v)); // EXPECTED: 2.0
	printf("%.4f\n", double(dv)); // EXPECTED: -3.0

	// second derivatives with barycentric representation
	barycentricdiff1(p, t, v, dv);
	printf("%.4f\n", double(v)); // EXPECTED: 2.0
	printf("%.4f\n", double(dv)); // EXPECTED: -3.0
	barycentricdiff2(p, t, v, dv, d2v);
	printf("%.4f\n", double(v)); // EXPECTED: 2.0
	printf("%.4f\n", double(dv)); // EXPECTED: -3.0
	printf("%.4f\n", double(d2v)); // EXPECTED: 2.0
	short i,myID = 0;
	string IPs[NUM_OF_PARTIES] = { string(argv[1]) };
	for (i = 2; i < argc; i++) {
		IPs[i - 1] = string(argv[i]);
		if (Helper::IPCompare(IPs[0], IPs[i - 1]))//if my IP is bigger than the other IP increment my ID
			myID++;
	}
	cout << "My ID id: "<<myID << endl;
	try {
		Party p = Party(myID,1);
		p.connectToAllParties(IPs);
		p.fInput();
		Share& circuitOutput= *p.calcCircuit();
		long result = p.finalReconstruct(circuitOutput);
		cout << "The result of the function is:" << result << endl;

		cout<<"The result is:"<< circuitOutput.toString() << endl;
	}
	catch (std::exception & exc) {
		//TO DO: send an abort message.
		cout << exc.what() << endl;
	}
	
	TRACE("Bye Bye!");
	
	getchar();
	
	return 0;
}