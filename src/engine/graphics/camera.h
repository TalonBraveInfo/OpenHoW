/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <PL/platform_graphics_camera.h>

class Camera {
 public:
  Camera(const PLVector3& pos, const PLVector3& angles);
  ~Camera();

  void SetPosition(const PLVector3& pos);
  void SetAngles(const PLVector3& angles);
  void SetFieldOfView(float fov);

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
