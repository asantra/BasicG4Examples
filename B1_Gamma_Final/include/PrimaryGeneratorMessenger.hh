#ifndef PrimaryGeneratorMessenger_h
#define PrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"

class G4UIcmdWithAString;
class G4UIdirectory;

namespace B1
{

class PrimaryGeneratorAction;

class PrimaryGeneratorMessenger : public G4UImessenger
{
public:

    PrimaryGeneratorMessenger(PrimaryGeneratorAction*);
    ~PrimaryGeneratorMessenger() override;

    void SetNewValue(G4UIcommand*, G4String) override;

private:

    PrimaryGeneratorAction* fPrimary = nullptr;

    G4UIdirectory* fDirectory = nullptr;

    G4UIcmdWithAString* fStartEventCmd = nullptr;
};

}

#endif