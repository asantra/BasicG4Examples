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
#include "PrimaryGeneratorAction.hh"
#include "G4RunManager.hh"
#include "EventInformation.hh"

#include "RunAction.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* runAction) : fRunAction(runAction) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



void EventAction::BeginOfEventAction(const G4Event*)
{
  fEdep = 0.;

  fGammaReachedDetector=false; //reset

  //fGammaInteractionCount=0;
  fTrackEdepMap.clear();
  fPDGEdep.clear();

  fTouchedTracks.clear();
  fTrackPDGMap.clear();

  fTrackParentMap.clear();
}


void EventAction::AddTrackEdep(G4int trackID, G4double edep)
{
  fTrackEdepMap[trackID] += edep;
}

void EventAction::AddPDGEdep(G4int pdgID, G4double edep)
{
  fPDGEdep[pdgID]+=edep;
}

const std::map<G4int,G4double>& EventAction::GetPDGEdep() const
{
    return fPDGEdep;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
  // accumulate statistics in run action
  fRunAction->AddEdep(fEdep);
/*
  if (fGammaReachedDetector)
  {
    fRunAction->CountInteractingEvent();
  }
*/
  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();
/*
  // fill histograms
  analysisManager->FillH1(0, fEdep);

  G4int eventID = event->GetEventID();

  for(const auto& p : fPDGEdep)
  {
    analysisManager->FillNtupleIColumn(1, 0, eventID);
    analysisManager->FillNtupleIColumn(1, 1, p.first);
    analysisManager->FillNtupleDColumn(1, 2, p.second);

    analysisManager->AddNtupleRow(1);

  }
*/
  
  auto* generator =
    static_cast<const B1::PrimaryGeneratorAction*>(
        G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());

  auto* info =
    static_cast<const EventInformation*>(
        event->GetUserInformation());

   long long rootEntry = info->GetRootEntry();

  G4int eventID = event->GetEventID();
  G4cout << "From EventAction::EndOfEventAction ---> Geant4 Event ID: " << eventID << ", Root Entry: " << rootEntry << G4endl;

  

  for (const auto& it : fPDGEdep)
  {
    analysisManager->FillNtupleDColumn(1, 0, rootEntry);
    analysisManager->FillNtupleIColumn(1, 1, it.first);   // pdgID
    analysisManager->FillNtupleDColumn(1, 2, it.second);   // total edep
    analysisManager->AddNtupleRow(1);

  }

  for(const auto& it : fTrackEdepMap)
  {
    G4int trackID = it.first;
    G4double totalEdep = it.second;

    //auto itPDG = fTrackPDGMap.find(trackID);
    //G4int pdgID =
        //(itPDG != fTrackPDGMap.end()) ? itPDG->second : -1;
    G4int pdgID = fTrackPDGMap[trackID];

    //auto itParent = fTrackParentMap.find(trackID);
    //G4int parentID =
        //(itParent != fTrackParentMap.end()) ? itParent->second : -1;
    G4int parentID = fTrackParentMap[trackID];

    analysisManager->FillNtupleDColumn(2,0,rootEntry);
    analysisManager->FillNtupleIColumn(2,1,trackID);
    analysisManager->FillNtupleDColumn(2,2,totalEdep);
    analysisManager->FillNtupleIColumn(2,3,pdgID);
    analysisManager->FillNtupleIColumn(2,4,parentID);

    analysisManager->AddNtupleRow(2);

  }
/*
  for (const auto& it : fTrackEdepMap)
  {
    G4int trackID = it.first;
    G4double totalEdep = it.second;

    //G4int pdgID = fTrackPDGMap[trackID];
    auto itPDG = fTrackPDGMap.find(trackID);
    G4int pdgID = (itPDG != fTrackPDGMap.end()) ? itPDG->second : -1;

    analysisManager->FillNtupleDColumn(1,0, rootEntry);
    analysisManager->FillNtupleIColumn(1,1, trackID);
    analysisManager->FillNtupleDColumn(1,2, totalEdep);
    analysisManager->FillNtupleIColumn(1,3,pdgID);

    analysisManager->AddNtupleRow(1);
  }
*/
  // fill ntuple

 // analysisManager->FillNtupleDColumn(0, fEdep);
 // analysisManager->AddNtupleRow();
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B1
