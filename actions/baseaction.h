#ifndef BASEACTION_H
#define BASEACTION_H

#include <GL/glextl.h>

class Image;

class BaseAction
{
public:
    BaseAction();
};

class BaseActionFactory
{
protected:
    BaseActionFactory();

public:
    virtual void MouseMove(Image* image, int x, int y) = 0;
    virtual void PrimaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super) = 0;
    virtual void PrimaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super) = 0;
    virtual void SecondaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super) = 0;
    virtual void SecondaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super) = 0;

    virtual GLuint ToolHelperImage() = 0;
};

#endif // BASEACTION_H
