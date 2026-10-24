#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <cstdio>
#include "app/CanvasDrawing.h"

int main()
{
    glfwSetErrorCallback([](int e, const char *d)
                         { std::fprintf(stderr, "GLFW error %d: %s\n", e, d); });
    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL 3.3 core
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 800, "2D Transformations", nullptr, nullptr);
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

        // init transformation values, pivot, and display option
        static bool showOriginal = true;
        static bool pivotRotation = false;
        static bool pivotScale = false;
        static float pivot[2] = {0.0f, 0.0f};
        static float translation[2] = {0.0f, 0.0f};
        static float rotationDeg = 0.0f;
        static float scale[2] = {1.0f, 1.0f};
        static float shear[2] = {0.0f, 0.0f};

        ImGui::SliderFloat2("Translation (tx, ty)", translation, -5.0f, 5.0f, "%.1f");
        ImGui::SliderFloat("Rotation (deg)", &rotationDeg, 0.0f, 360.0f, "%.0f");
        ImGui::SliderFloat2("Scale (sx, sy)", scale, -3.0f, 3.0f, "%.1f");
        ImGui::SliderFloat2("Shear (shearX, shearY)", shear, -1.0f, 1.0f, "%.2f");
        ImGui::Checkbox("Rotate about pivot", &pivotRotation);
        ImGui::Checkbox("Scale about pivot", &pivotScale);
        ImGui::SliderFloat2("Pivot (x, y)", pivot, -4.0f, 4.0f, "%.1f");
        ImGui::Checkbox("Show original shape", &showOriginal);

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

            const auto square = math2d::makeSquare();

            if (showOriginal)
            {
                canvas::drawPolyline(dl, square, origin, viewScale,
                                     IM_COL32(90, 170, 255, 255));
            }

            if (pivotRotation || pivotScale)
            {
                canvas::drawPivot(dl, pivot[0], pivot[1], origin, viewScale);
            }

            const math2d::Mat3 M = math2d::buildStandardTransform(
                translation[0], translation[1],
                rotationDeg,
                scale[0], scale[1],
                shear[0], shear[1],
                pivotRotation, pivotScale,
                pivot[0], pivot[1]);

            // rotated/sheared basis arrows: x is red, y is green
            const auto bx = math2d::transformedBasisX(M);
            canvas::drawArrow(dl, bx[0], bx[1], origin, viewScale,
                              IM_COL32(230, 90, 90, 255));

            const auto by = math2d::transformedBasisY(M);
            canvas::drawArrow(dl, by[0], by[1], origin, viewScale,
                              IM_COL32(90, 220, 110, 255));

            const auto transformed = math2d::transformShape(M, square);
            canvas::drawPolyline(dl, transformed, origin, viewScale,
                                 IM_COL32(255, 140, 60, 255));
        }
        ImGui::EndChild();
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