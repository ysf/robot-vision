//	RV02: Geometrische Transformationen und Grauwertinterpolation
//   
//	Autor: youssef
//	HAW-University of Applied Sciences - Hamburg, Germany

#include <iostream>
#include "ltiException.h"
#include "RV02.h"

using namespace std;

int main( int argc,char *argv[] )
{
	try {
		lti::RV02 rv02;
		rv02( argc, argv );
	}

	catch (lti::exception& exp) {
		cout << "An LTI::EXCEPTION was thrown: ";
		cout << exp.what() << endl;
	}
	catch (std::exception& exp) {
		cout << "std::exception was thrown: ";
		cout << exp.what() << endl;
	}
	catch (...) {
		cout << "Unknown exception thrown!" << endl;
	}

	return 0;
}