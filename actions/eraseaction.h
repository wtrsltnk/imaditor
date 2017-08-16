#ifndef ERASEACTION_H
#define ERASEACTION_H

#include "baseaction.h"
#include <glm/glm.hpp>

class EraseAction
{
public:
    EraseAction();
};

class EraseActionFactory : public BaseActionFactory
{
    void erase(Image* image, float color[], const glm::vec2& p);
    float _size;
    int _lastPosition[2];
    bool _isErasing;
    EraseActionFactory();
public:
    static EraseActionFactory* Instance();

    virtual void MouseMove(Image* image, int x, int y);
    virtual void PrimaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void PrimaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void SecondaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void SecondaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super);

    virtual GLuint ToolHelperImage();
};

#endif // ERASEACTION_H
