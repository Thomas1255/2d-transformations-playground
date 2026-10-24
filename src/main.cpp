#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <cstdio>
#include "app/CanvasDrawing.h"

int main()
{
    glfwSetErrorCallback([](int e, const char *d) { std::fprintf(stderr, "GLFW error %d: %s\n", e, d); });
    if (!glfwInit()) {return 1;}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL 3.3 core
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1920, 1080, "2D Transformations", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);                     // make the GL context current
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); // then load GL entry points

    // init ImGui: context → style → two backends (order matters, reverse on cleanup)
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // dark background
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ------- this frame's UI -------
        ImGui::SetNextWindowPos(ImVec2(16, 16), ImGuiCond_FirstUseEver);
        ImGui::Begin("2D Transformations");

        constexpr float kWidgetX = 160.0f; // x where the slider column starts

        // init transformation values, pivot, and display option
        static bool showOriginal = true;
        static bool pivotRotation = false;
        static bool pivotScale = false;
        static float pivot[2] = {0.0f, 0.0f};
        static float translation[2] = {0.0f, 0.0f};
        static float rotationDeg = 0.0f;
        static float scale[2] = {1.0f, 1.0f};
        static float shear[2] = {0.0f, 0.0f};

        static int shapeIndex = 1;
        static const char *shapeNames[] = {"Triangle", "Square", "House", "Car"};

        // lambda function so every slider below uses the same pattern
        auto labeled = [&](const char* label)
        {
            ImGui::Text("%s", label);
            ImGui::SameLine(kWidgetX);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        };

        labeled("Shape");
        ImGui::Combo("##shape", &shapeIndex, shapeNames, 4);

        labeled("Translation (tx, ty)");
        ImGui::SliderFloat2("##txty", translation, -5.0f, 5.0f, "%.1f");

        labeled("Rotation (deg)");
        ImGui::SliderFloat("##rot", &rotationDeg, 0.0f, 360.0f, "%.0f");

        labeled("Scale (sx, sy)");
        ImGui::SliderFloat2("##sxy", scale, -3.0f, 3.0f, "%.1f");

        labeled("Shear (shearX, shearY)");
        ImGui::SliderFloat2("##shear", shear, -1.0f, 1.0f, "%.2f");

        ImGui::Checkbox("Rotate about pivot", &pivotRotation);
        ImGui::Checkbox("Scale about pivot", &pivotScale);
        labeled("Pivot (x, y)");
        ImGui::SliderFloat2("##pivot", pivot, -4.0f, 4.0f, "%.1f");

        ImGui::Checkbox("Show original shape", &showOriginal);

        const math2d::Mat3 M = math2d::buildStandardTransform(
            translation[0], translation[1],
            rotationDeg,
            scale[0], scale[1],
            shear[0], shear[1],
            pivotRotation, pivotScale,
            pivot[0], pivot[1]);

        // ------- canvas region -------
        ImGui::BeginChild("canvas", ImVec2(0, 420), ImGuiChildFlags_Borders);
        {
            const float viewScale = 60.0f; // pixels per world unit
            const ImVec2 wpos = ImGui::GetWindowPos();
            const ImVec2 wsiz = ImGui::GetWindowSize();
            const ImVec2 pmin = wpos;
            const ImVec2 pmax(wpos.x + wsiz.x, wpos.y + wsiz.y);
            const ImVec2 origin((pmin.x + pmax.x) * 0.5f,
                                (pmin.y + pmax.y) * 0.5f);

            ImDrawList *dl = ImGui::GetWindowDrawList();

            canvas::drawGrid(dl, pmin, pmax, origin, 40.0f);

            std::vector<math2d::Vec2> shape;
            switch (shapeIndex)
            {
            case 0:
                shape = math2d::makeTriangle();
                break;
            case 1:
                shape = math2d::makeSquare();
                break;
            case 2:
                shape = math2d::makeHouse();
                break;
            case 3:
                shape = math2d::makeCar();
                break;
            }

            if (showOriginal)
            {
                canvas::drawPolyline(dl, shape, origin, viewScale,
                                     IM_COL32(90, 170, 255, 255));
            }

            if (pivotRotation || pivotScale)
            {
                canvas::drawPivot(dl, pivot[0], pivot[1], origin, viewScale);
            }

            // rotated/sheared basis arrows: x is red, y is green
            const auto bx = math2d::transformedBasisX(M);
            canvas::drawArrow(dl, bx[0], bx[1], origin, viewScale,
                              IM_COL32(230, 90, 90, 255));

            const auto by = math2d::transformedBasisY(M);
            canvas::drawArrow(dl, by[0], by[1], origin, viewScale,
                              IM_COL32(90, 220, 110, 255));

            const auto transformed = math2d::transformShape(M, shape);
            canvas::drawPolyline(dl, transformed, origin, viewScale,
                                 IM_COL32(255, 140, 60, 255));
        }
        ImGui::EndChild();

        ImGui::Separator();
        ImGui::Text("Composed matrix M = T · R · ShY · ShX · S (row-major):");
        ImGui::Text("%8.3f %8.3f %8.3f", M.m[0][0], M.m[0][1], M.m[0][2]);
        ImGui::Text("%8.3f %8.3f %8.3f", M.m[1][0], M.m[1][1], M.m[1][2]);
        ImGui::Text("%8.3f %8.3f %8.3f", M.m[2][0], M.m[2][1], M.m[2][2]);

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // Destroy imgui (init in reverse)
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}