#ifndef VULKANTUT_INCLUDE_INTERACTIVESTATE_H_
#define VULKANTUT_INCLUDE_INTERACTIVESTATE_H_

#endif //VULKANTUT_INCLUDE_INTERACTIVESTATE_H_

namespace interactivestate {

class InteractiveState {
  static float rotation;
  static float distance;

 public:
  static bool rotate;
  static bool light;

  static float get_rotation(float to_add);

  static float get_distance();

  static void set_distance(float to_add);
};

bool InteractiveState::light = false;
bool InteractiveState::rotate = false;
float InteractiveState::rotation = 0.0f;
float InteractiveState::distance = 40.0f;

float InteractiveState::get_rotation(float to_add = 0.0f) {
  InteractiveState::rotation += to_add;
  return glm::radians(InteractiveState::rotation);
}

float InteractiveState::get_distance() {
  return glm::radians(InteractiveState::distance);
}

void InteractiveState::set_distance(float to_add) {
  InteractiveState::distance += to_add;
}

}