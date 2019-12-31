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

#include "../../engine.h"
#include "../../model.h"
#include "actor_model.h"

using namespace openhow;

AModel::AModel() : SuperClass() {}
AModel::~AModel() = default;

void AModel::Draw() {
  SuperClass::Draw();

  if(!show_model_ || model_ == nullptr) {
    return;
  }

  if (strstr(model_->name, "dummy") != nullptr) {
    angles_.y += TICKS_PER_SECOND / 10.f;
  }

  PLVector3 angles(
      plDegreesToRadians(angles_.x),
      plDegreesToRadians(angles_.y),
      plDegreesToRadians(angles_.z));

  PLMatrix4 translation =
      (
        plRotateMatrix4(angles.z, { 1, 0, 0 }) *
        plRotateMatrix4(-angles.y, { 0, 1, 0 }) *
        plRotateMatrix4(angles.x, { 0, 0, 1 })
      ) * plTranslateMatrix4(position_);

  Model_Draw(model_, translation);
}

void AModel::SetModel(const std::string &path) {
  model_ = Engine::Resource()->LoadModel("chars/" + path, false);
  u_assert(model_ != nullptr);
}

void AModel::ShowModel(bool show) {
  show_model_ = show;
}
