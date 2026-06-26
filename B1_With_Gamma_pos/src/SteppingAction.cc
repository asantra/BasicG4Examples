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
/// \file SteppingAction.cc
/// \brief Implementation of the B1::SteppingAction class

#include "SteppingAction.hh"
#include "EventInformation.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "G4Gamma.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include <map>

#include "G4Event.hh"
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"



namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(EventAction* eventAction) : fEventAction(eventAction) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  if (!fScoringVolume) {
    const auto detConstruction = static_cast<const DetectorConstruction*>(
      G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume = detConstruction->GetScoringVolume();
  }


  auto track = step->GetTrack();


  auto preVol=
  step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  auto postVol =
  step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();


    auto analysisManager =
        G4AnalysisManager::Instance();
    auto eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

    G4int trackID = track->GetTrackID();
    G4int pdgID =track->GetDefinition()->GetPDGEncoding();
    G4int parentID = track->GetParentID();

    auto* info =
    static_cast<const EventInformation*>(
        G4RunManager::GetRunManager()->GetCurrentEvent()->GetUserInformation());

    long long rootEntry = info->GetRootEntry();

    // G4cout << "From SteppingAction::UserSteppingAction ---> Geant4 Event ID: " << eventID << ", Root Entry: " << rootEntry << G4endl;


    if(preVol && postVol)
    {
      if(preVol->GetLogicalVolume() != fScoringVolume &&
        postVol->GetLogicalVolume() == fScoringVolume)
      {
        if(!fEventAction->HasTouchedDetector(trackID))
        {
            fEventAction->MarkTouchedDetector(trackID);
            G4double boundaryEnergy = track->GetKineticEnergy();
            G4ThreeVector pos = track->GetPosition();
            G4ThreeVector dir = track->GetMomentumDirection();


            analysisManager->FillNtupleDColumn(0,0,rootEntry);
            analysisManager->FillNtupleIColumn(0,1,trackID);
            analysisManager->FillNtupleIColumn(0,2,pdgID);
            analysisManager->FillNtupleIColumn(0,3,parentID);
            analysisManager->FillNtupleDColumn(0,4,boundaryEnergy);
            analysisManager->FillNtupleDColumn(0,5,pos.x()/m);
            analysisManager->FillNtupleDColumn(0,6,pos.y()/m);
            analysisManager->FillNtupleDColumn(0,7,pos.z()/m);
            analysisManager->FillNtupleDColumn(0,8,dir.x());
            analysisManager->FillNtupleDColumn(0,9,dir.y());
            analysisManager->FillNtupleDColumn(0,10,dir.z());

            analysisManager->AddNtupleRow(0);
        }

      }

    }





    if (preVol && preVol->GetLogicalVolume()==fScoringVolume)
    {
      G4double edepStep = step->GetTotalEnergyDeposit();
      if(edepStep>0.)
      {
        fEventAction->AddTrackEdep(trackID, edepStep);
        fEventAction->AddTrackPDG(trackID, pdgID);
        fEventAction->AddTrackParent(trackID, parentID);
        fEventAction->AddPDGEdep(pdgID, edepStep);



      }





    }










  }



}
/*
void SteppingAction::Reset()
    {
      fTrackEdep.clear();

    }
    const std::map<G4int, G4double>& SteppingAction::GetTrackEdep() const
    {
      return fTrackEdep;

    }
*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

 // namespace B1
