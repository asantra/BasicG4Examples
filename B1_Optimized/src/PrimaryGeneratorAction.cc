//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the B1::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <cmath>

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle = particleTable->FindParticle(particleName = "gamma");
  fParticleGun->SetParticleDefinition(particle);
  //fParticleGun->SetParticleEnergy(20. * MeV);
}



PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}



void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  // this function is called at the begining of ecah event
  //

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get Envelope volume
  // from G4LogicalVolumeStore.


  G4double costheta1 = 2.0 * G4UniformRand() - 1.0;
  G4double theta1 = std::acos(costheta1);
  G4double sintheta1 = std::sin(theta1);
  G4double phi1 = 2* CLHEP::pi* G4UniformRand();
  G4double x1 = sintheta1 * std::cos(phi1);
  G4double y1 = sintheta1 * std::sin(phi1);
  G4double z1 = costheta1;


  fParticleGun->SetParticlePosition(G4ThreeVector(1*x1*m,1*y1*m,1*z1*m));


  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(-1*x1, -1*y1, -1*z1));



/*
  G4double costheta = 2.0 * G4UniformRand() - 1.0;

  G4double theta2 = CLHEP::pi+std::acos(costheta);
  G4double costheta2 = std::cos(theta2);
  G4double sintheta2 = std::sin(theta2);
  G4double phi2 = 2* CLHEP::pi* G4UniformRand();
  G4double x2 = sintheta2 * std::cos(phi2);
  G4double y2 = sintheta2 * std::sin(phi2);
  G4double z2 = costheta2;
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(x2, y2, z2));
*/

  G4double Emin = 3.01*MeV;
  G4double Emax = 4.6*MeV;

  G4double energy = Emin + (Emax - Emin)*G4UniformRand();


/*
  G4double mean = (Emin + Emax)/2.0;
  G4double sigma = (Emax-Emin)/6.0;
  G4double energy ;
  do {
    energy = G4RandGauss::shoot(mean,sigma);
  } while (energy<Emin || energy > Emax);

*/
  fParticleGun->SetParticleEnergy(energy);


  fParticleGun->GeneratePrimaryVertex(event);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B1
