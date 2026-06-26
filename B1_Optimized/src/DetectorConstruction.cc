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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{

  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Envelope parameters
  //
  //G4double env_sizeXY = 4 * m, env_sizeZ = 4 * m;
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_AIR");

  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //
  // World
  //
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

  G4double sphere_rmin = 0 * m , sphere_rmax = 1.5*m;
  G4double sphere_theta_min = 0* deg, sphere_theta_max = 180 * deg;
  G4double sphere_phi_min = 0* deg, sphere_phi_max = 360 * deg;

  auto solidEnv = new G4Sphere("Envelope",
                               sphere_rmin, sphere_rmax, sphere_phi_min, sphere_phi_max, sphere_theta_min, sphere_theta_max);

  //
  // Envelope
  //
  /*
  auto solidEnv = new G4Box("Envelope",  // its name
                            0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5 * env_sizeZ);  // its size
  */

  auto logicEnv = new G4LogicalVolume(solidEnv,  // its solid
                                      env_mat,  // its material
                                      "Envelope");  // its name

  new G4PVPlacement(nullptr,  // no rotation
                    G4ThreeVector(),  // at (0,0,0)
                    logicEnv,  // its logical volume
                    "Envelope",  // its name
                    logicWorld,  // its mother  volume
                    false,  // no boolean operation
                    0,  // copy number
                    checkOverlaps);  // overlaps checking

/*
  G4Material* Outer_mat = nist -> FindOrBuildMaterial("G4_Pb");

  G4double outer_XYZ = 30.1 * cm;
  auto OuterBox = new G4Box("Outer_box", 0.5 * outer_XYZ, 0.5 * outer_XYZ, 0.5 * outer_XYZ);
  auto logicOuterBox = new G4LogicalVolume(OuterBox,Outer_mat,"Outer_box");

  new G4PVPlacement(nullptr,
                    G4ThreeVector(),
                    logicOuterBox,
                    "Outer_box",
                    logicEnv,
                    false,
                    0,
                    checkOverlaps);

  G4Material* Inner_mat = nist -> FindOrBuildMaterial("G4_AIR");

  G4double inner_XYZ = 20.1 * cm;
  auto InnerBox = new G4Box("Inner_box", 0.5 * inner_XYZ, 0.5 * inner_XYZ, 0.5 * inner_XYZ);
  auto logicInnerBox = new G4LogicalVolume(InnerBox,Inner_mat,"Inner_box");

  new G4PVPlacement(nullptr,
                    G4ThreeVector(),
                    logicInnerBox,
                    "Inner_box",
                    logicOuterBox,
                    false,
                    0,
                    checkOverlaps);

*/

  G4double rmin = 0 * cm , rmax = 10 * cm;
  G4double cyl_phimin = 0. * deg, cyl_phimax = 360. * deg;
  G4double thickness_Z = 20 * cm;

  G4Material* cyl_mat = nist -> FindOrBuildMaterial("G4_CESIUM_IODIDE");

  auto cylinder= new G4Tubs("Cylinder",rmin, rmax, 0.5 * thickness_Z, cyl_phimin, cyl_phimax);
  auto logicCylinder = new  G4LogicalVolume(cylinder, cyl_mat,"Cylinder");

  new G4PVPlacement(nullptr,
                    G4ThreeVector(),
                    logicCylinder,
                    "Cylinder",
                    logicEnv,
                    false,
                    0,
                    checkOverlaps);

  fScoringVolume=logicCylinder;




  return physWorld;
}




//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B1
