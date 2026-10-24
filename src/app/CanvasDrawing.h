#pragma once
#include "math/Transform2D.h"
#include <vector>
#include <imgui.h>

namespace canvas {
ImVec2 worldToScreen(const math2d::Vec2& p, const ImVec2& origin, float scale);
void drawPolyline(ImDrawList* dl,
        const std::vector<math2d::Vec2>& pts,
        const ImVec2& origin,
        float scale,
        ImU32 color,
        bool closed = true,
        float thickness = 2.0f);
void drawArrow(ImDrawList* dl,
        const math2d::Vec2& a,
        const math2d::Vec2& b,
        const ImVec2& origin,
        float scale,
        ImU32 color,
        float thickness = 2.0f);
void drawGrid(ImDrawList* dl, const ImVec2& pmin, const ImVec2& pmax, const ImVec2& origin, float stepPx);
void drawPivot(ImDrawList* dl, float cx, float cy, const ImVec2& origin, float scale);
math2d::Vec2 screenToWorld(const ImVec2& p, const ImVec2& origin, float scale);
void drawPoint(ImDrawList* dl, const math2d::Vec2& p, const ImVec2& origin, float scale, ImU32 color, float radius = 5.0f);
void drawOpenPolyline(ImDrawList* dl,
        const std::vector<math2d::Vec2>& pts,
        const ImVec2& origin,
        float scale,
        ImU32 color,
        float thickness = 2.0f);
}
