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
/// \file DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"


#include "G4AutoDelete.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4SDManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"


namespace B1
{

G4VPhysicalVolume* DetectorConstruction::Construct()
{

  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  G4bool checkOverlaps = true;

  // World
  G4double world_sizeXY = 8 * m;
  G4double world_sizeZ = 10 * m;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

  auto solidWorld =
    new G4Box("World",  // its name
              0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);  // its size

  auto logicWorld = new G4LogicalVolume(solidWorld,  // its solid
                                        world_mat,  // its material
                                        "World");  // its name

  auto physWorld = new G4PVPlacement(nullptr,  // no rotation
                                     G4ThreeVector(),  // at (0,0,0)
                                     logicWorld,  // its logical volume
                                     "World",  // its name
                                     nullptr,  // its mother  volume
                                     false,  // no boolean operation
                                     0,  // copy number
                                     checkOverlaps);  // overlaps checking

  // Outer HDPE shield
  G4Material* hdpe_mat = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
  G4double hdpeOuter_Z = 2.1 * m;
  G4double hdpeOuter_XY = 1.9* m;
  auto HdpeBox = new G4Box("HDPE_box", 0.5 * hdpeOuter_XY, 0.5 * hdpeOuter_XY, 0.5 * hdpeOuter_Z);
  logicHdpeBox = new G4LogicalVolume(HdpeBox,hdpe_mat,"HDPE_box");
  new G4PVPlacement(nullptr,
                    G4ThreeVector(),
                    logicHdpeBox,
                    "HDPE_box",
                    logicWorld,
                    false,
                    0,
                    checkOverlaps);



  // Lead shield
  G4Material* lead_mat = nist -> FindOrBuildMaterial("G4_Pb");
  G4double leadOuter_Z = 1.7 * m;
  G4double leadOuter_XY = 1.5 * m;
  auto LeadBox = new G4Box("LEAD_box", 0.5 * leadOuter_XY, 0.5 * leadOuter_XY, 0.5 * leadOuter_Z);
  logicLeadBox = new G4LogicalVolume(LeadBox,lead_mat,"LEAD_box");
  new G4PVPlacement(nullptr,
                    G4ThreeVector(),
                    logicLeadBox,
                    "LEAD_box",
                    logicHdpeBox,
                    false,
                    0,
                    checkOverlaps);


  //Copper Shield
  G4Material* copper_mat = nist ->FindOrBuildMaterial("G4_Cu");
  G4double copperOuter_XY = 0.9 * m;
  G4double copperOuter_Z = 1.1 * m;
  auto CopperBox = new G4Box("COPPER_box", 0.5 * copperOuter_XY, 0.5 * copperOuter_XY, 0.5 * copperOuter_Z );
  logicCopperBox = new G4LogicalVolume(CopperBox,copper_mat,"COPPER_box");
  new G4PVPlacement(nullptr,
                    G4ThreeVector(),
                    logicCopperBox,
                    "COPPER_box",
                    logicLeadBox,
                    false,
                    0,
                    checkOverlaps);


  // Internal polyethylene shield
  G4Material* poly_mat = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
  G4double polyOuter_Z = 0.8 * m;
  G4double polyOuter_XY = 0.6 * m;
  auto PolyBox = new G4Box("POLY_box", 0.5 * polyOuter_XY, 0.5 * polyOuter_XY, 0.5 * polyOuter_Z);
  logicPolyBox = new G4LogicalVolume(PolyBox,poly_mat,"POLY_box");
  new G4PVPlacement(nullptr,
                    G4ThreeVector(),
                    logicPolyBox,
                    "POLY_box",
                    logicCopperBox,
                    false,
                    0,
                    checkOverlaps);


  //Inner dimension of box
  G4Material* Inner_mat = nist -> FindOrBuildMaterial("G4_AIR");
  G4double inner_XY = 0.52 * m;
  G4double inner_Z = 0.72 * m;
  auto InnerBox = new G4Box("Inner_box", 0.5 * inner_XY, 0.5 * inner_XY, 0.5 * inner_Z);
  auto logicInnerBox = new G4LogicalVolume(InnerBox,Inner_mat,"Inner_box");
  new G4PVPlacement(nullptr,
                    G4ThreeVector(),
                    logicInnerBox,
                    "Inner_box",
                    logicPolyBox,
                    false,
                    0,
                    checkOverlaps);

  //Detector

  G4double rmin = 0.0 * m;
  G4double rmax = 0.25 * m;
  G4double phimin = 0.0 * deg;
  G4double phimax = 360 * deg;
  G4double thetamin = 0.0 * deg;
  G4double thetamax = 180 * deg;

  G4Material* detector_mat = nist -> FindOrBuildMaterial("G4_CESIUM_IODIDE");

  auto detector = new G4Sphere("Detector", rmin, rmax, phimin, phimax, thetamin, thetamax);
  auto logicDetector = new G4LogicalVolume(detector, detector_mat, "Detector");

  new G4PVPlacement(nullptr,
                    G4ThreeVector(),
                    logicDetector,
                    "Detector",
                    logicInnerBox,
                    false,
                    0,
                    checkOverlaps);

  fScoringVolume=logicDetector;




  return physWorld;
}



}  // namespace B1
