#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <cstdio>

int main() {
    glfwSetErrorCallback([](int e, const char* d) { std::fprintf(stderr, "GLFW error %d: %s\n", e, d); });
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);      // 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);      // OpenGL 3.3 core
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 800, "2D Transformations", nullptr, nullptr);
    if (!window) { 
        glfwTerminate(); 
        return 1; 
    }
    glfwMakeContextCurrent(window);                      // make the GL context current
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);  // then load GL entry points
    
    // init ImGui: context → style → two backends (order matters, reverse on cleanup)
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);            // dark background
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();        

        // ------- describe this frame's UI -------
            ImGui::SetNextWindowPos(ImVec2(16, 16), ImGuiCond_FirstUseEver);
        ImGui::Begin("2D Transformations");
        ImGui::Text("Hello before we wire transforms!");
        static float angleDeg = 0.0f;
        ImGui::SliderFloat("Angle (deg)", &angleDeg, 0.0f, 360.0f, "%.0f");
        ImGui::Text("Value now: %.1f", angleDeg);
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