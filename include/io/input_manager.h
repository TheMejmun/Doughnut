//
// Created by Sam on 2023-04-08.
//

#ifndef DOUGHNUT_INPUT_MANAGER_H
#define DOUGHNUT_INPUT_MANAGER_H

#include "preprocessor.h"
#include "typedefs.h"
#include "ecs/entities/input_state_entity.h"

#include <GLFW/glfw3.h>
#include <memory>
#include <vector>

class InputController {
public:
    explicit InputController(GLFWwindow *window);

    ~InputController();

    void update(double delta, EntityManagerSpec &entityManager) ;

private:
    static void _callback(GLFWwindow *window, int key, int scancode, int action, int mods);

    void processInput(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void handleKey(KeyState *key, const int &actionCode);

    KeyState *keySwitch(const KeyCode &key);

    GLFWwindow *window = nullptr;

    KeyState closeWindow = IM_RELEASED;
    KeyState toggleFullscreen = IM_RELEASED;
    KeyState moveForward = IM_RELEASED;
    KeyState moveBackward = IM_RELEASED;
    KeyState toggleRotation = IM_RELEASED;
};

#endif //DOUGHNUT_INPUT_MANAGER_H
