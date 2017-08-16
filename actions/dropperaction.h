#ifndef DROPPERACTION_H
#define DROPPERACTION_H

#include "baseaction.h"
#include <glm/glm.hpp>

class DropperAction
{
public:
    DropperAction();
};

class DropperActionFactory : public BaseActionFactory
{
    void getColor(Image* image, float color[], const glm::vec2& p);
    float _size;
    int _lastPosition[2];
    bool _isPainting;
    DropperActionFactory();
public:
    static DropperActionFactory* Instance();

    virtual void MouseMove(Image* image, int x, int y);
    virtual void PrimaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void PrimaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void SecondaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void SecondaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super);

    virtual GLuint ToolHelperImage();
};

#endif // DROPPERACTION_H
