#ifndef Hector_IO_MADXParser_h
#define Hector_IO_MADXParser_h

#include "Hector/Utils/OrderedParametersMap.h"
#include "Hector/Utils/UnorderedParametersMap.h"

#include "Hector/Elements/ElementBase.h"
#include "Hector/Elements/ApertureType.h"

#include <fstream>
#include <regex>
#include <string>
#include <memory>

using std::ostream;

namespace Hector
{
  class Beamline;
  namespace IO
  {
    /// Parsing tool for MAD-X output stp files
    class MADX
    {
      public:
        /// Class constructor
        /// \param[in] filename Path to the MAD-X Twiss file to parse
        /// \param[in] ip_name Name of the interaction point
        /// \param[in] min_s Minimal s-coordinate from which the Twiss file must be parsed
        /// \param[in] max_s Maximal s-coordinate at which the Twiss file must be parsed
        MADX( std::string filename, std::string ip_name, int direction, float max_s=-1., float min_s = 0. );
        /// Class constructor
        /// \param[in] filename Path to the MAD-X Twiss file to parse
        /// \param[in] ip_name Name of the interaction point
        /// \param[in] min_s Minimal s-coordinate from which the Twiss file must be parsed
        /// \param[in] max_s Maximal s-coordinate at which the Twiss file must be parsed
        MADX( const char* filename, const char* ip_name, int direction, float max_s=-1., float min_s = 0. );
        MADX( const MADX& );
        MADX( MADX& );
        ~MADX() {}

        /// Retrieve the sequenced beamline parsed from the MAD-X Twiss file
        Beamline* beamline() const;
        /// Retrieve the raw beamline parsed from the MAD-X Twiss file
        Beamline* rawBeamline() const { return raw_beamline_.get(); }

        /// Get a Hector element type from a Twiss element name string
        static Element::Type findElementTypeByName( std::string name );
        /// Get a Hector element type from a Twiss element keyword string
        static Element::Type findElementTypeByKeyword( std::string keyword );
        /// Get a Hector element aperture type from a Twiss element apertype string
        static Aperture::Type findApertureTypeByApertype( std::string apertype );

        typedef enum { allPots, horizontalPots, verticalPots } RPType;
        Elements romanPots( const RPType& type = allPots ) const;

        /// Print all useful information parsed from the MAD-X Twiss file
        void printInfo() const;
        std::map<std::string,std::string> headerStrings() const;
        std::map<std::string,float> headerFloats() const;

      private:
        /// A collection of values to be propagated through this parser
        typedef std::vector<std::string> ValuesCollection;
        /// Type of content stored in the parameters map
        enum ValueType : short { Unknown = -1, String, Float, Integer };
        /// Human-readable printout of a value type
        friend std::ostream& operator<<( std::ostream&, const ValueType& );

        void parseHeader();
        void parseElementsFields();
        void parseElements();
        void findInteractionPoint();
        std::shared_ptr<Element::ElementBase> parseElement( const ValuesCollection& );

        ParametersMap::Ordered<std::string> header_str_;
        ParametersMap::Ordered<float> header_float_;

        ParametersMap::Unordered<ValueType> elements_fields_;

        std::ifstream in_file_;
        std::streampos in_file_lastline_;

        std::unique_ptr<Beamline> beamline_;
        std::unique_ptr<Beamline> raw_beamline_;
        std::shared_ptr<Element::ElementBase> interaction_point_;

        int dir_;
        std::string ip_name_;
        float min_s_;
        // quantities needed whenever direction == 1 (FIXME)
        TwoVector previous_relpos_, previous_disp_, previous_beta_;

        static std::regex rgx_typ_, rgx_hdr_, rgx_elm_hdr_;
        static std::regex rgx_drift_name_, rgx_ip_name_, rgx_monitor_name_;
        static std::regex rgx_quadrup_name_;
        static std::regex rgx_sect_dipole_name_, rgx_rect_dipole_name_;
        static std::regex rgx_rect_coll_name_;
    };
  }
}

#endif
