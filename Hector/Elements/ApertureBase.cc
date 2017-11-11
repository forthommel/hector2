#include "Hector/Elements/ApertureBase.h"
#include "Hector/Core/Utils.h"

namespace Hector
{
  namespace Aperture
  {
    ApertureBase::ApertureBase( const Type& type, const CLHEP::Hep2Vector& pos, const Parameters& param ) :
      type_( type ), pos_( pos ), param_( param )
    {}

    ApertureBase::~ApertureBase()
    {
      param_.clear();
    }

    bool
    ApertureBase::operator==( const ApertureBase& rhs ) const
    {
      if ( type_!=rhs.type_ ) return false;
      if ( pos_!=rhs.pos_ ) return false;
      if ( param_!=rhs.param_ ) return false;
      return true;
    }

    std::ostream&
    operator<<( std::ostream& os, const ApertureBase& ap )
    {
      os << ap.type()
         << Form( " (param. (%.3f, %.3f, %.3f, %.3f),", ap.p( 0 ), ap.p( 1 ), ap.p( 2 ), ap.p( 3 ) )
         << " center at " << ap.position() << ")";
      return os;
    }

    std::ostream&
    operator<<( std::ostream& os, const ApertureBase* ap )
    {
      os << *( ap );
      return os;
    }
  }
}
