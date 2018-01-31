//
//  integration_example.cpp
//  gsl-modules
//
//  Created by Francisco Meirinhos on 09/11/16.
//

#include "n_integrator.hpp"

#include <iostream>

int main() {

  // 3D integrator
  gsl_modules::Integrator<3> integrator;
  using boundary_t = decltype(integrator)::boundary_t;

  boundary_t boundaries = {{0, 1}, {0, M_PI}, {0, 2 * M_PI}};

  // Jacobian determinant
  auto jacdet = [](double r, double theta, double phi) {
    (void)phi; // avoid unused parameter errors
    return r * r * sin(theta);
  };

  // The volume of a unit sphere
  const double volume_sphere = 4. / 3 * M_PI;
  const double integration = integrator.integrate(jacdet, boundaries);

  std::cout << "The volume of a sphere is: " << volume_sphere << std::endl;
  std::cout << "Integration result: " << integration << std::endl;
  std::cout << "Error: " << fabs(volume_sphere - integration) / integration
            << std::endl;

  return 0;
}
