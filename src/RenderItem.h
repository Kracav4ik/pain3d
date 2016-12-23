#pragma once

class QMatrix4x4;

struct RenderItem{
    virtual void render(const QMatrix4x4& mvp) = 0;
    virtual void init_gl() = 0;
    virtual ~RenderItem(){}
};
