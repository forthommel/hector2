#ifndef Hector_IO_MADXParser_h
#define Hector_IO_MADXParser_h

#include "beamline/Beamline.h"

#include "elements/ElementDict.h"

#include "elements/Quadrupole.h"
#include "elements/Dipole.h"
#include "elements/RectangularCollimator.h"
#include "elements/Kicker.h"
#include "elements/Marker.h"

#include "elements/EllipticAperture.h"
#include "elements/CircularAperture.h"
#include "elements/RectangularAperture.h"
#include "elements/RectEllipticAperture.h"

#include "core/OrderedParametersMap.h"
#include "core/UnorderedParametersMap.h"

#include <fstream>
#include <regex>
#include <map>
#include <string>

using std::ostream;

namespace Hector
{
  namespace Parser
  {
    /// Parsing tool for MAD-X output stp files
    class MADX
    {
      public:
        /// A collection of values to be propagated through this parser
        typedef std::vector<std::string> ValuesCollection;
        /// Type of content stored in the parameters map
        enum ValueType { Unknown = -1, String, Float, Integer };
        /// Human-readable printout of a value type
        friend std::ostream& operator<<( std::ostream&, const ValueType& );

      public:
        /// Class constructor
        /// \param[in] max_s Maximal s-coordinate at which the Twiss file must be parsed
        MADX( const char*, const char*, int, float max_s=-1. );
        ~MADX();

        /// Retrieve the sequenced beamline parsed from the MAD-X output file
        Beamline* beamline() const {
          if ( !beamline_ ) {
            PrintWarning( "Sequenced beamline not computed from the MAD-X output file. Retrieving the raw version. You may encounter some numerical issues." );
            return raw_beamline_;
          }
          return beamline_;
        }
        /// Retrieve the raw beamline parsed from the MAD-X output file
        Beamline* rawBeamline() const { return raw_beamline_; }

        static Element::Type findTypeByName( std::string name );

        typedef enum { allPots, horizontalPots, verticalPots } RPType;
        Elements romanPots( const RPType& type=allPots ) const;

        /// Print all useful information parsed from the MAD-X output file
        void printInfo() const;

      private:
        void parseHeader();
        void parseElementsFields();
        void parseElements();
        void findInteractionPoint();
        Element::ElementBase* parseElement( const ValuesCollection& );

        ParametersMap::Ordered<std::string> header_str_;
        ParametersMap::Ordered<float> header_float_;

        ParametersMap::Unordered<ValueType> elements_fields_;

        std::ifstream in_file_;
        std::streampos in_file_lastline_;

        Beamline* beamline_;
        Beamline* raw_beamline_;

        int dir_;
        std::string ip_name_;
        float s_offset_;
        bool found_interaction_point_;
        // quantities needed whenever direction == 1 (FIXME)
        CLHEP::Hep2Vector previous_relpos_, previous_disp_, previous_beta_;

        static std::regex rgx_typ_, rgx_hdr_, rgx_elm_hdr_;
        static std::regex rgx_rp_horiz_name_, rgx_rp_vert_name_;
        static std::regex rgx_drift_name_, rgx_ip_name_, rgx_monitor_name_;
        static std::regex rgx_quadrup_name_;
        static std::regex rgx_sect_dipole_name_, rgx_rect_dipole_name_;
        static std::regex rgx_rect_coll_name_;

        bool has_next_element_;
    };
  }
}

#endif
