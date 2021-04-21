#include "Hector/Exception.h"
#include "Hector/ParticleStoppedException.h"

#include "Hector/IO/TwissHandler.h"

#include "Hector/Beamline.h"
#include "Hector/Propagator.h"

#include "Hector/Utils/BeamProducer.h"
#include "Hector/Utils/ArgsParser.h"
#include "Hector/Utils/String.h"

#include <CLHEP/Random/RandGauss.h>

using namespace std;

int main(int argc, char* argv[]) {
  string twiss_file, ip;
  double min_s, max_s;
  unsigned int num_part = 100;
  bool shoot;
  hector::ArgsParser(argc,
                     argv,
                     {{"twiss-file", "beamline Twiss file", &twiss_file, 'i'}},
                     {
                         {"interaction-point", "name of the interaction point", "IP5", &ip, 'c'},
                         {"min-s", "minimum arc length s to parse (m)", 0., &min_s},
                         {"max-s", "maximum arc length s to parse (m)", 250., &max_s},
                         {"num-part", "number of particles to shoot", 10, &num_part, 'n'},
                         {"simulate", "simulate a beam propagation", false, &shoot, 's'},
                     });

  hector::io::Twiss parser(twiss_file.c_str(), ip.c_str(), max_s, min_s);
  parser.printInfo();
  cout << "+---------------------+--------------------+----------------------" << endl;
  cout << hector::format("| %-19s | %-18s | %20s|", "Name", "Type", "Position along s (m)") << endl;
  cout << "+---------------------+--------------------+----------------------" << endl;
  for (const auto& elem : parser.beamline()->elements()) {
    //if ( elem->type() == hector::element::aDrift ) continue;
    cout << hector::format("|%20s | %-18s [ ",
                           elem->name().c_str(),
                           (elem->type() != hector::element::aDrift) ? elem->typeName().c_str() : "");
    string pos_ini = hector::format("%7s", hector::format("%#0.3f", elem->s()).c_str());
    if (elem->length() > 0.)
      cout << hector::format(
          "%.7s → %7s m", pos_ini.c_str(), hector::format("%#0.3f", elem->s() + elem->length()).c_str());
    else
      cout << hector::format("%17s m", pos_ini.c_str());
    cout << " ]" << endl;
  }
  cout << "+---------------------+-------------------------------------------" << endl;
  //parser.beamline()->dump();

  cout << "beamline matrix at s = " << max_s
       << " m: " << parser.beamline()->matrix(0., hector::Parameters::get()->beamParticlesMass(), +1) << endl;

  if (shoot) {
    hector::Propagator prop(parser.beamline());
    //parser.beamline()->dump();

    hector::beam::GaussianParticleGun gun;
    gun.smearEnergy(hector::Parameters::get()->beamEnergy(), hector::Parameters::get()->beamEnergy() * 0.);
    //hector::beam::TXscanner gun( num_part, hector::Parameters::get()->beamEnergy(), 0., 1. );
    map<string, unsigned int> stopping_elements;
    for (unsigned int i = 0; i < num_part; ++i) {
      hector::Particle p = gun.shoot();
      p.setCharge(+1);
      try {
        prop.propagate(p, 203.826);
      } catch (const hector::ParticleStoppedException& e) {
        stopping_elements[e.stoppingElement()->name()]++;
        continue;
      } catch (const hector::Exception& e) {
        e.dump(std::cerr);
      }
    }

    ostringstream os;
    os << "Summary\n\t-------";
    for (const auto& el : stopping_elements)
      os << hector::format("\n\t*) %.2f%% of particles stopped in %s", 100. * el.second / num_part, el.first.c_str());
    H_INFO << os.str() << ".";
  }

  return 0;
}