#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include <string>

namespace B1
{

PrimaryGeneratorMessenger::
PrimaryGeneratorMessenger(PrimaryGeneratorAction* gun)
: fPrimary(gun)
{
    fDirectory = new G4UIdirectory("/myapp/");
    fDirectory->SetGuidance("Primary Generator Control");

    fStartEventCmd = new G4UIcmdWithAString("/myapp/startEventID", this);
    fStartEventCmd->SetGuidance("Set first ROOT event (64-bit).");
    fStartEventCmd->SetParameterName("eventID", false);
    fStartEventCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
    delete fStartEventCmd;
    delete fDirectory;
}

void PrimaryGeneratorMessenger::SetNewValue(
        G4UIcommand* command,
        G4String value)
{
    if(command == fStartEventCmd)
    {
        fPrimary->SetStartEventID(
            std::stoll(value));
    }
}

}