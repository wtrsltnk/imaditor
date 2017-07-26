#ifndef FLOODFILLACTION_H
#define FLOODFILLACTION_H

#include "baseaction.h"

class FloodFillAction : public BaseAction
{
public:
    FloodFillAction();
};

class FloodFillActionFactory : public BaseActionFactory
{
    FloodFillActionFactory();
public:
    static FloodFillActionFactory* Instance();

    virtual void PrimaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void PrimaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void SecondaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void SecondaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super);
};

#endif // FLOODFILLACTION_H
