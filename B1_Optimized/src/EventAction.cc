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
/// \file EventAction.cc
/// \brief Implementation of the B1::EventAction class

#include "EventAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "RunAction.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* runAction) : fRunAction(runAction)
{
  // OPTIMIZED: Pre-allocate bucket arrays for expected track multiplicity.
  // These buckets persist across clear() calls — no heap churn between events.
  // Tune these numbers to your typical tracks-per-event count.
  constexpr G4int kExpectedTracks = 256;
  constexpr G4int kExpectedPDGs   = 16;   // particle species in one event

  fTrackEdepMap.reserve(kExpectedTracks);
  fTrackPDGMap .reserve(kExpectedTracks);
  fTrackParentMap.reserve(kExpectedTracks);
  fTouchedTracks .reserve(kExpectedTracks);
  fPDGEdep       .reserve(kExpectedPDGs);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event*)
{
  fEdep = 0.;
  fGammaReachedDetector = false;

  // OPTIMIZED: clear() on unordered_map/set destroys stored elements but
  // KEEPS the bucket array alive (C++ standard guarantee). The next event
  // refills without any heap allocation until the reserved capacity is
  // exceeded — which avoids repeated new/delete overhead per event.
  fTrackEdepMap  .clear();
  fTrackPDGMap   .clear();
  fTrackParentMap.clear();
  fTouchedTracks .clear();
  fPDGEdep       .clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::AddPDGEdep(G4int pdgID, G4double edep)
{
  fPDGEdep[pdgID] += edep;
}

const std::unordered_map<G4int,G4double>& EventAction::GetPDGEdep() const
{
  return fPDGEdep;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
  // accumulate statistics in run action
  fRunAction->AddEdep(fEdep);

  auto analysisManager = G4AnalysisManager::Instance();
  G4int eventID = event->GetEventID();

  // Fill PDG-level energy deposit ntuple
  for (const auto& [pdgID, totalEdep] : fPDGEdep)
  {
    analysisManager->FillNtupleIColumn(1, 0, eventID);
    analysisManager->FillNtupleIColumn(1, 1, pdgID);
    analysisManager->FillNtupleDColumn(1, 2, totalEdep);
    analysisManager->AddNtupleRow(1);
  }

  // Fill track-level energy deposit ntuple
  // OPTIMIZED: use find() instead of operator[] to avoid accidental insertion
  // into fTrackPDGMap/fTrackParentMap during the read loop.
  for (const auto& [trackID, totalEdep] : fTrackEdepMap)
  {
    auto itPDG    = fTrackPDGMap   .find(trackID);
    auto itParent = fTrackParentMap.find(trackID);

    G4int pdgID   = (itPDG    != fTrackPDGMap   .end()) ? itPDG->second    : -1;
    G4int parentID= (itParent != fTrackParentMap .end()) ? itParent->second : -1;

    analysisManager->FillNtupleIColumn(2, 0, eventID);
    analysisManager->FillNtupleIColumn(2, 1, trackID);
    analysisManager->FillNtupleDColumn(2, 2, totalEdep);
    analysisManager->FillNtupleIColumn(2, 3, pdgID);
    analysisManager->FillNtupleIColumn(2, 4, parentID);
    analysisManager->AddNtupleRow(2);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B1
