#include "brushaction.h"

BrushAction::BrushAction() { }

BrushActionFactory::BrushActionFactory() { }

BrushActionFactory* BrushActionFactory::Instance()
{
    static BrushActionFactory instance;

    return &instance;
}

void BrushActionFactory::PrimaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }

void BrushActionFactory::PrimaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }

void BrushActionFactory::SecondaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }

void BrushActionFactory::SecondaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }
