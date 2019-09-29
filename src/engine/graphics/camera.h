
#pragma once

#include <PL/platform_graphics_camera.h>

class Camera {
 public:
  Camera(const PLVector3& pos, const PLVector3& angles);
  ~Camera();

  void SetPosition(const PLVector3& pos);
  void SetAngles(const PLVector3& angles);

  PLVector3 GetPosition() { return camera_->position; }
  PLVector3 GetAngles() { return camera_->angles; }
  PLVector3 GetForward() { return camera_->forward; }

  void SetViewport(const std::array<int, 2>& xy, const std::array<int, 2>& wh);

  int GetViewportWidth() { return camera_->viewport.w; }
  int GetViewportHeight() { return camera_->viewport.h; }

  void MakeActive();

 protected:
 private:
  PLCamera* camera_{nullptr};
};
