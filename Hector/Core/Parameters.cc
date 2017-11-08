#include "Parameters.h"

namespace Hector
{
  Parameters::Parameters() :
    beam_energy_( 6500. ), // in GeV
    beam_particles_mass_( 0.938272046 ), // in GeV
    crossing_angle_x_( 370.0e-6 ), // in rad
    crossing_angle_y_( 0. ), // in rad
    beam_particles_charge_( +1 ),
    logging_threshold_( Info ),
    use_relative_energy_( false ),
    correct_beamline_overlaps_( false ),
    compute_aperture_acceptance_( true ),
    enable_kickers_( false ),
    enable_dipoles_( true )
  {}
}