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
    void floodFill(class Layer* layer, float color[]);
    FloodFillActionFactory();
public:
    static FloodFillActionFactory* Instance();

    virtual void MouseMove(Image* image, int x, int y);
    virtual void PrimaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void PrimaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void SecondaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void SecondaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super);

    virtual GLuint ToolHelperImage();
};

#endif // FLOODFILLACTION_H
