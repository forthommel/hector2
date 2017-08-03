#include "Hector/Beamline/Beamline.h"
#include "Hector/Elements/Marker.h"

int
main( int argc, char* argv[] )
{
  Hector::Beamline line( 10. );
  
  /*Hector::Element::Marker marker( "test" );
  line.addElement( &marker );*/
  line.addElement( new Hector::Element::Marker( "test", 0., 0. ), true );

  line.dump();

  //std::cout << "haha" << std::endl;

  return 0;
}
