//	RV02: Geometrische Transformationen und Grauwertinterpolation
//   
//	Autor: youssef
//	HAW-University of Applied Sciences - Hamburg, Germany

#include "RV02.h"

#include <iostream>

using namespace std;

#include "gtk.h"
#include "ltiGtkServer.h"

#include "ltiObject.h"
#include "ltiTimer.h"
#include "ltiBMPFunctor.h"
#include "ltiViewer.h"
#include "ltiSplitImageToHSI.h"

namespace lti {

    double affine_values[6] = {
        160.791667,  0.441667, -0.13,        // a0, a1, a2
        218.741667,  0.001667,  0.4925       // b0, b1, b2
    };

    unsigned fourpoint_values[8] = {
        197, 100,                            // xq0, yq0
        584, 93,                             // xq1, yq1
        665, 473,                            // xq2, yq2
        99,  467                             // xq3, yq3
    };

    bool inline in_border( int x, int y, const channel8& img )
    {
        if( x < 0 || y < 0 ) return false;
        int xmax = img.columns(), ymax = img.rows();       
        return ( x < xmax ) && ( y < ymax ); 
    }
	
    int nearest_neighbor( const double x, const double y, const channel8& src )
    {
		int x0 = x + 0.5 , y0 = y + 0.5;    // + 0.5 damit gerundet und nicht abgeschnitten wird

        if( in_border( x0, y0, src ))
            return src[ y0 ][ x0 ];        

        return 0;
    }

	int bilinear( const double x, const double y, const channel8& src ) 
	{
		int x0 = x, y0 = y;

        if( x0 < 0 || y0 < 0 || !in_border( x0 + 1, y0 + 1, src ))
            return 0;

		double	dx   = x - x0, dy   = y - y0,
				omdx = 1 - dx, omdy = 1 - dy;

        return	src[ y0     ][ x0     ] * omdy * omdx + 
				src[ y0     ][ x0 + 1 ] * omdy * dx   +
				src[ y0 + 1 ][ x0     ] * dy   * omdx +
				src[ y0 + 1 ][ x0 + 1 ] * dy   * dx;
	}    

    void affine_transform(  channel8& dst,
                            const channel8& src,
                            double v[6],
                            int(*interpol)(double, double, const channel8&) ) {               

        double a0 = v[0], a1 = v[1], a2 = v[2];
        double b0 = v[3], b1 = v[4], b2 = v[5];
        
        int rows = dst.rows();
        int cols = dst.columns();

        for( int y = 0; y < rows; ++y ) {
            for( int x = 0; x < cols; ++x ) {
                dst[ y ][ x ] = (* interpol)( a0 + a1 * x + a2 * y, // x' = a0 + a1 * x + a2 * y
                                              b0 + b1 * x + b2 * y, // y' = b0 + b1 * x + b2 * y
                                              src );
            }
        }
    }

    void fourpoint_transform( channel8& dst,
                              const channel8& src,
                              unsigned v[8], // v = { xq0, yq0 .. xq3, yq3 } // 0 .. 7
                              int(*interpol)(double, double, const channel8&) ) {        

        int rows = dst.rows();
        int cols = dst.columns();

        for( int y = 0; y < rows; ++y ) {
            for( int x = 0; x < cols; ++x ) {
                    
                double xz = (double)x / cols;
                double yz = (double)y / rows;

                double phi0 = ( 1 - xz ) * ( 1 - yz );
                double phi1 = xz * ( 1 - yz );
                double phi2 = xz * yz;
                double phi3 = ( 1 - xz ) * yz;                

                double xq = phi0 * v[0] + phi1 * v[2] + phi2 * v[4] + phi3 * v[6];
                double yq = phi0 * v[1] + phi1 * v[3] + phi2 * v[5] + phi3 * v[7];                

                dst[ y ][ x ] = (* interpol)( xq, yq, src );
            }
        }
    }

	void RV02::operator()(int argc,char *argv[])
	{
		gtkServer server;	// has always to be started (AMei) if program is without gtk-Widgets 
		server.start();		

		/**** instantiation of used components ****/
        
        image img;		
		loadBMP loader;                         

		viewer view("original"),
               view_affine_nearest("affine nearest"),
               view_affine_bilinear("affine bilinear"),     
               view_fourpoint_bilinear("fourpoint bilinear");
									    
		channel8    src,
                    affine_nearest,
                    affine_bilinear,
                    fourpoint_bilinear;

		// object to split image into hue ( farbton ), saturation( farbsättigung )  and intensity ( grauwert )
		splitImageToHSI splitter;

		/**** the program ****/

		loader.load( "Kalib.bmp", img);		
		splitter.getIntensity( img, src );	
		view.show( src );
		
        affine_nearest.resize( src.rows(), src.columns(), 0, false, true ); 
        affine_bilinear.resize( src.rows(), src.columns(), 0, false, true );
        fourpoint_bilinear.resize( src.rows(), src.columns(), 0, false, true );
                
        affine_transform( affine_nearest, src, affine_values, nearest_neighbor );
        affine_transform( affine_bilinear, src, affine_values, bilinear );

        fourpoint_transform( fourpoint_bilinear, src, fourpoint_values, bilinear );
                            
		view_affine_nearest.show(affine_nearest); 
        view_affine_bilinear.show(affine_bilinear);  
        view_fourpoint_bilinear.show(fourpoint_bilinear);  

		getchar();
	}
};
