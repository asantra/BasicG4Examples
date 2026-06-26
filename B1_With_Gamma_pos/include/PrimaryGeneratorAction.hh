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
/// \file PrimaryGeneratorAction.hh
/// \brief Definition of the B1::PrimaryGeneratorAction class

#ifndef B1PrimaryGeneratorAction_h
#define B1PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
//#include "TH1D.h"
//#include "TFile.h"
#include "globals.hh"
#include <vector>

class G4ParticleGun;
class G4Event;
class G4Sphere;

namespace B1
{

class PrimaryGeneratorMessenger;

/// The primary generator action class with particle gun.
///
/// The default kinematic is a 6 MeV gamma, randomly distribued
/// in front of the phantom across 80% of the (X,Y) phantom size.

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();

    PrimaryGeneratorAction(const std::vector<double>& E,
                       const std::vector<double>& CDF);
    ~PrimaryGeneratorAction() override;

    // method from the base class
    void GeneratePrimaries(G4Event*) override;

    // method to access particle gun
    //const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

    const G4ParticleGun* GetGammaGun() const { return fGammaGun; }
    //const G4ParticleGun* GetNeutronGun() const { return fNeutronGun; }
    void SetStartEventID(long long id);
    long long GetStartEventID() const { return fStartEventID; }
    

  private:
    //G4ParticleGun* fParticleGun = nullptr;  // pointer a to G4 gun class

    //G4ParticleGun* fGammaGun = nullptr;
    //TFile* fFile = nullptr;
    //TH1D* h = nullptr;
    std::vector<double> E;
    std::vector<double> CDF;


    G4ParticleGun* fGammaGun = nullptr;

    G4Sphere* fEnvelopeBox = nullptr;
    long long fStartEventID = 0;
    PrimaryGeneratorMessenger* fMessenger;
};

}  // namespace B1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
