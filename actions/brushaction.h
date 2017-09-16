#ifndef BRUSHACTION_H
#define BRUSHACTION_H

#include "baseaction.h"
#include <glm/glm.hpp>
#include "../layer.h"

class BrushAction : public BaseAction
{
public:
    BrushAction();
};

class BrushActionFactory : public BaseActionFactory
{
    void paint(float color[], const glm::vec2& from, const glm::vec2& to);
    float _size;
    int _lastPosition[2];
    bool _isPainting;
    Layer _tmpLayer;
    BrushActionFactory();
public:
    static BrushActionFactory* Instance();

    virtual void MouseMove(Image* image, int x, int y);
    virtual void PrimaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void PrimaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void SecondaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super);
    virtual void SecondaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super);

    virtual GLuint ToolHelperImage();
};

#endif // BRUSHACTION_H
