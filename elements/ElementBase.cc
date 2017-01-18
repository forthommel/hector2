#include "ElementBase.h"

namespace Hector
{
  namespace Element
  {
    ElementBase::ElementBase( const Type& type, const std::string& name ) :
      type_( type ), name_( name ), aperture_( 0 ),
      length_( 0. ), magnetic_strength_( 0. ), s_( 0. )
    {}

    ElementBase::~ElementBase()
    {
      if ( aperture_ ) delete aperture_;
    }

    float
    ElementBase::fieldStrength( float eloss, float mp, int qp ) const
    {
      if ( qp==0 ) return 0.;

      const float eini = Constants::beam_energy,
                  mp0 = Constants::beam_particles_mass,
                  e = eini-eloss;
      const float p0 = sqrt( ( eini-mp0 )*( eini+mp0 ) ), // e_ini^2 - p_0^2 = mp0^2
                  p = sqrt( ( e-mp )*( e+mp ) ); // e^2 - p^2 = mp^2
      return magnetic_strength_*( p0/p )*( qp/Constants::beam_particles_charge );
    }

    /// Human-readable printout of a beamline element type
    std::ostream&
    operator<<( std::ostream& os, const ElementBase::Type& type )
    {
      switch ( type ) {
        case ElementBase::Invalid: os << "invalid"; break;
        case ElementBase::Marker: os << "marker"; break;
        case ElementBase::Drift: os << "drift"; break;
        case ElementBase::Monitor: os << "monitor"; break;
        case ElementBase::RectangularDipole: os << "rectangular dipole"; break;
        case ElementBase::SectorDipole: os << "sector dipole"; break;
        case ElementBase::Quadrupole: os << "quadrupole"; break;
        case ElementBase::Sextupole: os << "sextupole"; break;
        case ElementBase::Multipole: os << "multipole"; break;
        case ElementBase::VerticalKicker: os << "vertical kicker"; break;
        case ElementBase::HorizontalKicker: os << "horizontal kicker"; break;
        case ElementBase::RectangularCollimator: os << "rectangular collimator"; break;
        case ElementBase::EllipticalCollimator: os << "elliptical collimator"; break;
        case ElementBase::CircularCollimator: os << "circular collimator"; break;
        case ElementBase::Placeholder: os << "placeholder"; break;
        case ElementBase::Instrument: os << "instrument"; break;
      }
      return os;
    }

    /// Human-readable printout of a beamline element object
    std::ostream&
    operator<<( std::ostream& os, const ElementBase& elem )
    {
      os << "[" << elem.type() << "] " << elem.name() << " at s=" << elem.s() << " m.";
      if ( elem.aperture() ) {
        os << " with " << elem.aperture();
      }
      return os;
    }

    /// Human-readable printout of a beamline element object
    std::ostream&
    operator<<( std::ostream& os, const ElementBase* elem )
    {
      os << *( elem );
      return os;
    }
  }
}
