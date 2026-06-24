#pragma once
#include "./wtypes/Vec2.hpp"
#include <vector>


class RenderElem {
public:
    virtual ~RenderElem() = default;
    
    void draw(const Vec2& globalPos, const Vec2& globalRot, const Vec2& globalScale) {
    }
};


class Node {
public:
    Vec2 position = Vec2::ZERO;
    Vec2 rotation = Vec2::ZERO; // x/y tracking or angle mapping
    Vec2 scale    = Vec2(1.0f, 1.0f);

    std::vector<RenderElem*> renderElements;

    ~Node() {
        for (auto* elem : renderElements) {
            delete elem;
        }
    }

    void render() {
        // Cascade down to every primitive draw bucket attached to this spatial location
        for (auto* elem : renderElements) {
            elem->draw(position, rotation, scale);
        }
    }
};
