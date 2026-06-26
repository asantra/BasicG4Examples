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
#include "PrimaryGeneratorMessenger.hh"
#include "EventInformation.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Event.hh"
#include <cmath>
#include <algorithm>

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  PrimaryGeneratorAction::PrimaryGeneratorAction(const std::vector<double>& E,
                                               const std::vector<double>& CDF)
  : E(E), CDF(CDF)


//PrimaryGeneratorAction::PrimaryGeneratorAction()
{


    fGammaGun = new G4ParticleGun(1);



    auto particleTable = G4ParticleTable::GetParticleTable();
    fGammaGun->SetParticleDefinition(particleTable->FindParticle("gamma"));

    fStartEventID = 0;

    fMessenger = new PrimaryGeneratorMessenger(this);



}




PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  //delete fParticleGun;
  delete fGammaGun;
  delete fMessenger;
  //delete fFile;

  //delete fNeutronGun;
}


void PrimaryGeneratorAction::SetStartEventID(long long id)
{
    fStartEventID = id;
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

  G4int geantEventID = event->GetEventID();

  long long rootEntry = fStartEventID + geantEventID;

  //------------------------------------------
  // Attach ROOT entry to this event
  //------------------------------------------

  auto* info = new EventInformation();

  info->SetRootEntry(rootEntry);

  event->SetUserInformation(info);

//------------------------------------------

  //fParticleGun->SetParticlePosition(G4ThreeVector(1.5*x1*m,1.5*y1*m,1.5*z1*m)); //since body diagonal of the outermost box is 2.08m


  //fParticleGun->SetParticleMomentumDirection(G4ThreeVector(-1*x1, -1*y1, -1*z1));

  G4ThreeVector pos1(2.0*x1*m,2.0*y1*m,2.0*z1*m);

  G4ThreeVector dir1(-x1,-y1,-z1);


  double r = G4UniformRand();

  auto it = std::lower_bound(CDF.begin(), CDF.end(), r);

  //size_t idx = std::distance(CDF.begin(), it);
  size_t idx = (it == CDF.end()) ? (CDF.size() - 1)
                               : std::distance(CDF.begin(), it);

  double gammaEnergy = E[idx];



  fGammaGun->SetParticlePosition(pos1);
  fGammaGun->SetParticleMomentumDirection(dir1);
  fGammaGun->SetParticleEnergy(gammaEnergy * keV);

  fGammaGun->GeneratePrimaryVertex(event);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B1


