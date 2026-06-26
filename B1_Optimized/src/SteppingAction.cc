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

#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "G4Gamma.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
// OPTIMIZED: removed #include <map> — no map is owned by SteppingAction

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

  auto track   = step->GetTrack();
  auto preVol  = step->GetPreStepPoint() ->GetTouchableHandle()->GetVolume();
  auto postVol = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();

  auto analysisManager = G4AnalysisManager::Instance();
  auto eventID  = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

  G4int trackID  = track->GetTrackID();
  G4int pdgID    = track->GetDefinition()->GetPDGEncoding();
  G4int parentID = track->GetParentID();

  // Track first entry into the scoring volume
  if (preVol && postVol)
  {
    if (preVol ->GetLogicalVolume() != fScoringVolume &&
        postVol->GetLogicalVolume() == fScoringVolume)
    {
      if (!fEventAction->HasTouchedDetector(trackID))
      {
        fEventAction->MarkTouchedDetector(trackID);

        analysisManager->FillNtupleIColumn(0, 0, eventID);
        analysisManager->FillNtupleIColumn(0, 1, trackID);
        analysisManager->FillNtupleIColumn(0, 2, pdgID);
        analysisManager->FillNtupleIColumn(0, 3, parentID);
        analysisManager->AddNtupleRow(0);
      }
    }
  }

  // Accumulate energy deposit inside the scoring volume
  if (preVol && preVol->GetLogicalVolume() == fScoringVolume)
  {
    G4double edepStep = step->GetTotalEnergyDeposit();
    if (edepStep > 0.)
    {
      // OPTIMIZED: all accumulation goes into EventAction's unordered_maps;
      // no local map in SteppingAction means zero per-step allocation cost.
      fEventAction->AddTrackEdep  (trackID, edepStep);
      fEventAction->AddTrackPDG   (trackID, pdgID);
      fEventAction->AddTrackParent(trackID, parentID);
      fEventAction->AddPDGEdep    (pdgID,   edepStep);
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B1
