#include "Hector/Core/Exception.h"
#include "Hector/Core/ParticleStoppedException.h"

#include "Hector/IO/MADXHandler.h"
#include "Hector/Beamline/Beamline.h"

#include "Hector/Propagator/Propagator.h"
#include "Hector/Utils/BeamProducer.h"
#include "Hector/Utils/ArgsParser.h"

#include <CLHEP/Random/RandGauss.h>

using namespace std;

int main( int argc, char* argv[] )
{
  string twiss_file, ip;
  double min_s, max_s;
  unsigned int num_part = 100;
  Hector::ArgsParser( argc, argv, {
    { "--twiss-file", "beamline Twiss file", &twiss_file }
  }, {
    { "--interaction-point", "name of the interaction point", "IP5", &ip },
    { "--min-s", "minimum arc length s to parse", 0., &min_s },
    { "--max-s", "maximum arc length s to parse", 250., &max_s },
    { "--num-part", "number of particles to shoot", 10, &num_part },
  } );

  Hector::IO::MADX parser( twiss_file.c_str(), ip.c_str(), 1, max_s, min_s );
  parser.printInfo();
  cout << "+---------------------+--------------------+----------------------" << endl;
  cout << Hector::Form( "| %-19s | %-18s | %20s|", "Name", "Type", "Position along s (m)" ) << endl;
  cout << "+---------------------+--------------------+----------------------" << endl;
  for ( const auto& elem : parser.beamline()->elements() ) {
    //if ( elem->type() == Hector::Element::aDrift ) continue;
    cout << Hector::Form( "|%20s | %-18s [ ", elem->name().c_str(), ( elem->type() != Hector::Element::aDrift ) ? elem->typeName().c_str() : "" );
    string pos_ini = Hector::Form( "%7s", Hector::Form( "%#0.3f", elem->s() ).c_str() );
    if ( elem->length() > 0. ) cout << Hector::Form( "%.7s → %7s m", pos_ini.c_str(), Hector::Form( "%#0.3f", elem->s()+elem->length() ).c_str() );
    else cout << Hector::Form( "%17s m", pos_ini.c_str() );
    cout << " ]";
    cout << endl;
  }
  cout << "+---------------------+-------------------------------------------" << endl;
  //parser.beamline()->dump();

  cout << "beamline matrix at s = " << max_s << " m: " << parser.beamline()->matrix( 100., Hector::Parameters::get()->beamParticlesMass(), +1 ) << endl;

  Hector::Propagator prop( parser.beamline() );
  //parser.beamline()->dump();

  Hector::BeamProducer::GaussianParticleGun gun;
  gun.smearEnergy( Hector::Parameters::get()->beamEnergy()/1.25, Hector::Parameters::get()->beamEnergy() );
  //Hector::BeamProducer::TXscanner gun( num_part, Hector::Parameters::get()->beamEnergy(), 0., 1. );
  map<string,unsigned int> stopping_elements;
  for ( unsigned int i = 0; i < num_part; ++i ) {
    Hector::Particle p = gun.shoot();
    p.setCharge( +1 );
    try { prop.propagate( p, 203.826 ); }
    catch ( Hector::ParticleStoppedException& e ) {
      stopping_elements[e.stoppingElement()->name()]++;
      continue;
    }
    catch ( Hector::Exception& e ) { e.dump(); }
  }

  ostringstream os; os << "Summary\n\t-------";
  for ( const auto& el : stopping_elements )
    os << Hector::Form( "\n\t*) %.2f%% of particles stopped in %s", 100.*el.second/num_part, el.first.c_str() );
  PrintInfo( os.str().c_str() );

  return 0;
}
