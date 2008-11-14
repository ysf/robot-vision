//	RV02: Geometrische Transformationen und Grauwertinterpolation
//   
//	Autor: youssef
//	HAW-University of Applied Sciences - Hamburg, Germany
 
#ifndef LTI_RV02_H
#define LTI_RV02_H

#include "ltiImage.h"

#define WITHOUT_GTK_WIDGETS 

namespace lti {

	class RV02 {
	public:
		void operator()(int argc,char *argv[]);
		void RV02::median( const channel8& src, channel8& dst, unsigned int x_mask, unsigned int y_mask );
	};
}

#endif
