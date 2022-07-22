#include "GLFW/glfw3.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "sip/sip-agent.h"
#include "utils/command.h"
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
int count = 0;
int main(int argc, char *args[]) {
  if (argc != 2) {
    std::cout << "Need the local binding sip uri." << std::endl;
    return 0;
  }
  RequestURI uri(args[1]);
  SipAgent sip_agent(uri.host(), uri.port(), 10000, 20000);
  sip_agent.setFromURI(args[1]);
  std::thread sip_agent_thr([&sip_agent]() {
    sip_agent.addRequestHandler("INVITE", [&sip_agent](const std::shared_ptr<SIPMessage> &msg) {
      sip_agent.doLua("../scripts/invite.lua", msg);
    });
    sip_agent.addRequestHandler("CANCEL", [&sip_agent](const std::shared_ptr<SIPMessage> &msg) {
      sip_agent.doLua("../scripts/cancel.lua", msg);
    });
    sip_agent.addRequestHandler("ACK", [&sip_agent](const std::shared_ptr<SIPMessage> &msg) {});
    sip_agent.addRequestHandler("BYE", [&sip_agent](const std::shared_ptr<SIPMessage> &msg) {
      sip_agent.doLua("../scripts/bye.lua", msg);
      sip_agent.endDialog(msg);
    });
    sip_agent.Run(&count);
  });
  std::thread t([] {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow *window = glfwCreateWindow(1280, 720, "MY SIP", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
      {
        ImGui::Begin("control");
        ImGui::Text("hello");
        ImGui::End();
      }
      ImGui::Render();
      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                   clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT);
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      glfwSwapBuffers(window);
    }
  });
  char input_char[100] = {0};
  while (true) {
    Command command(input_char);
    if (command.command() == "invite") {
      if (command.params().empty()) {
        std::cout << "Need a to uri like sip:1000@127.0.0.1:5060" << std::endl;
      } else {
        sip_agent.invite(command.params());
      }
    } else if (command.command() == "quit") {
      break;
    } else {
      std::cout << "q to quit." << std::endl;
    }

    std::cin.getline(input_char, 100);
  }

  sip_agent.stop();
  sip_agent_thr.join();
  std::cout << "packet recv: " << count << std::endl;

  t.join();
  return 0;
}