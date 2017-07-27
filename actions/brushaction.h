#ifndef BRUSHACTION_H
#define BRUSHACTION_H

#include "baseaction.h"

class BrushAction : public BaseAction
{
public:
    BrushAction();
};

class BrushActionFactory : public BaseActionFactory
{
    BrushActionFactory();
public:
    static BrushActionFactory* Instance();

    virtual void PrimaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void PrimaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void SecondaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void SecondaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super);
};

#endif // BRUSHACTION_H
