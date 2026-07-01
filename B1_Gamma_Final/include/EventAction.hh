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
/// \file EventAction.hh
/// \brief Definition of the B1::EventAction class

#ifndef B1EventAction_h
#define B1EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <map>
#include <set>


class G4Event;

namespace B1
{

class RunAction;

/// Event action class

class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction* runAction);
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    void AddEdep(G4double edep) { fEdep += edep; }

    void AddTrackEdep(G4int trackID, G4double edep);//{fTrackEdepMap[trackID] += edep;}
    //void CountGammaInteraction(){fGammaInteractionCount++;}
    const std::map<G4int,G4double>&GetTrackEdepMap() const {return fTrackEdepMap;}

    void AddTrackPDG(G4int trackID, G4int pdgID)
    {
      fTrackPDGMap[trackID] = pdgID;
    }

    const std::map<G4int,G4int>& GetTrackPDGMap() const
    {
      return fTrackPDGMap;
    }

    void AddPDGEdep(G4int pdgID, G4double edep);
    const std::map<G4int,G4double>& GetPDGEdep() const;

    void SetGammaReachedDetector()
    {
      fGammaReachedDetector = true;
    }

    G4bool GammaReachedDetector() const
    {
      return fGammaReachedDetector;
    }


    bool HasTouchedHdpe(G4int trackID) const
    {
      return fTouchedTracksH.find(trackID) != fTouchedTracksH.end();

    }
    void MarkTouchedHdpe(G4int trackID)
    {
      fTouchedTracksH.insert(trackID);

    }


    bool HasTouchedLead(G4int trackID) const
    {
      return fTouchedTracksL.find(trackID) != fTouchedTracksL.end();

    }
    void MarkTouchedLead(G4int trackID)
    {
      fTouchedTracksL.insert(trackID);

    }



    bool HasTouchedCopper(G4int trackID) const
    {
      return fTouchedTracksC.find(trackID) != fTouchedTracksC.end();

    }
    void MarkTouchedCopper(G4int trackID)
    {
      fTouchedTracksC.insert(trackID);

    }

    bool HasTouchedPoly(G4int trackID) const
    {
      return fTouchedTracksP.find(trackID) != fTouchedTracksP.end();

    }
    void MarkTouchedPoly(G4int trackID)
    {
      fTouchedTracksP.insert(trackID);

    }


    bool HasTouchedDetector(G4int trackID) const
    {
      return fTouchedTracks.find(trackID) != fTouchedTracks.end();

    }
    void MarkTouchedDetector(G4int trackID)
    {
      fTouchedTracks.insert(trackID);

    }



    void AddTrackParent(G4int trackID, G4int parentID)
    {
      fTrackParentMap[trackID] = parentID;

    }

    const std::map<G4int,G4int>& GetTrackParentMap() const
    {
      return fTrackParentMap;

    }

  private:
    RunAction* fRunAction = nullptr;
    G4double fEdep = 0.;

    //G4bool fGammaReachedDetector=false;

    std::map<G4int,G4double>fTrackEdepMap;

    std::map<G4int,G4int>fTrackPDGMap;
    G4bool fGammaReachedDetector=false;


    std::map<G4int, G4double> fPDGEdep;

    std::set<G4int>fTouchedTracksH;

    std::set<G4int>fTouchedTracksL;

    std::set<G4int>fTouchedTracksC;

    std::set<G4int>fTouchedTracksP;

    std::set<G4int>fTouchedTracks;

    std::map<G4int,G4int> fTrackParentMap;




    //G4int fGammaInteractionCount = 0;
};

}  // namespace B1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
