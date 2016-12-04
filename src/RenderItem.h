#pragma once

struct RenderItem{
    virtual void render() = 0;
    virtual void init_gl() = 0;
    virtual ~RenderItem(){}
};
