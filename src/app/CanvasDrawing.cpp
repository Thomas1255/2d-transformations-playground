#include "app/CanvasDrawing.h"
#include <cmath>

namespace canvas {
    ImVec2 worldToScreen(const math2d::Vec2& p, const ImVec2& origin, float scale) {
        return ImVec2(origin.x + p.x * scale, origin.y - p.y * scale);
    }

    void drawPolyline(ImDrawList* dl,
        const std::vector<math2d::Vec2>& pts,
        const ImVec2& origin,
        float scale,
        ImU32 color,
        bool closed,
        float thickness) {
        if (pts.size() < 2) return;

        for (size_t i = 0; i + 1 < pts.size(); ++i) {
            dl->AddLine(worldToScreen(pts[i], origin, scale),
                worldToScreen(pts[i + 1], origin, scale),
                color, thickness);
        }

        if (closed && pts.size() >= 3) {
            dl->AddLine(worldToScreen(pts.back(), origin, scale),
                worldToScreen(pts.front(), origin, scale),
                color, thickness);
        }
    }

    void drawArrow(ImDrawList* dl,
        const math2d::Vec2& a,
        const math2d::Vec2& b,
        const ImVec2& origin,
        float scale,
        ImU32 color,
        float thickness) {
        ImVec2 p0 = worldToScreen(a, origin, scale);
        ImVec2 p1 = worldToScreen(b, origin, scale);

        dl->AddLine(p0, p1, color, thickness);

        const float dx = p1.x - p0.x;
        const float dy = p1.y - p0.y;
        const float len = std::sqrt(dx * dx + dy * dy);
        if (len < 1e-6f) return;

        const float ux = dx / len;
        const float uy = dy / len;
        const float head = 10.0f;

        ImVec2 left(p1.x - head * (ux + 0.4f * uy), p1.y - head * (uy - 0.4f * ux));
        ImVec2 right(p1.x - head * (ux - 0.4f * uy), p1.y - head * (uy + 0.4f * ux));

        dl->AddLine(p1, left, color, thickness);
        dl->AddLine(p1, right, color, thickness);
    }

    void drawGrid(ImDrawList* dl, const ImVec2& pmin, const ImVec2& pmax, const ImVec2& origin, float stepPx) {
        const ImU32 gridColor = IM_COL32(70, 70, 76, 255);
        const ImU32 axisColor = IM_COL32(180, 180, 190, 255);

        for (float x = origin.x; x <= pmax.x; x += stepPx)
            dl->AddLine(ImVec2(x, pmin.y), ImVec2(x, pmax.y), gridColor, 1.0f);
        for (float x = origin.x; x >= pmin.x; x -= stepPx)
            dl->AddLine(ImVec2(x, pmin.y), ImVec2(x, pmax.y), gridColor, 1.0f);

        for (float y = origin.y; y <= pmax.y; y += stepPx)
            dl->AddLine(ImVec2(pmin.x, y), ImVec2(pmax.x, y), gridColor, 1.0f);
        for (float y = origin.y; y >= pmin.y; y -= stepPx)
            dl->AddLine(ImVec2(pmin.x, y), ImVec2(pmax.x, y), gridColor, 1.0f);

        dl->AddLine(ImVec2(pmin.x, origin.y), ImVec2(pmax.x, origin.y), axisColor, 2.0f);
        dl->AddLine(ImVec2(origin.x, pmin.y), ImVec2(origin.x, pmax.y), axisColor, 2.0f);
    }

    void drawPivot(ImDrawList* dl, float cx, float cy, const ImVec2& origin, float scale) {
        const ImVec2 p = worldToScreen(math2d::Vec2{ cx, cy }, origin, scale);
        const ImU32 yellow = IM_COL32(240, 220, 70, 255);

        dl->AddCircleFilled(p, 5.0f, yellow);
        dl->AddLine(ImVec2(p.x - 8, p.y), ImVec2(p.x + 8, p.y), yellow, 2.0f);
        dl->AddLine(ImVec2(p.x, p.y - 8), ImVec2(p.x, p.y + 8), yellow, 2.0f);
    }

    math2d::Vec2 screenToWorld(const ImVec2& p, const ImVec2& origin, float scale) {
        return math2d::Vec2{ (p.x - origin.x) / scale, (origin.y - p.y) / scale };
    }


    void drawPoint(ImDrawList* dl, const math2d::Vec2& p, const ImVec2& origin, float scale, ImU32 color, float radius) {
        dl->AddCircleFilled(worldToScreen(p, origin, scale), radius, color);
    }

    void drawOpenPolyline(ImDrawList* dl,
        const std::vector<math2d::Vec2>& pts,
        const ImVec2& origin,
        float scale,
        ImU32 color,
        float thickness) {
        if (pts.size() < 2) return;
        for (size_t i = 0; i + 1 < pts.size(); ++i) {
            dl->AddLine(worldToScreen(pts[i], origin, scale),
                        worldToScreen(pts[i + 1], origin, scale),
                        color, thickness);
        }
    }


}
