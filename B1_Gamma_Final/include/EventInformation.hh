#ifndef B1EventInformation_h
#define B1EventInformation_h 1

#include "G4VUserEventInformation.hh"
#include "globals.hh"

namespace B1
{

class EventInformation : public G4VUserEventInformation
{
public:

    EventInformation() = default;
    ~EventInformation() override = default;

    void SetRootEntry(long long id)
    {
        fRootEntry = id;
    }

    long long GetRootEntry() const
    {
        return fRootEntry;
    }

    void Print() const override {}

private:

    long long fRootEntry = -1;
};

}

#endif