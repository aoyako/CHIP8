#include <GLFW/glfw3.h>

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void ErrorCallback(int, const char* err_str) {
  std::cout << "GLFW Error: " << err_str << std::endl;
}

int main() {
  glfwSetErrorCallback(ErrorCallback);
  if (!glfwInit()) {
    std::cout << "b" << std::endl;
    return 1;
  }

  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  std::cout << "a" << std::endl;
  return 0;
}