/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

  if(model_ == nullptr) {
    return;
  }

  if (strstr(model_->name, "dummy") != nullptr) {
    angles_.y += TICKS_PER_SECOND / 1000.f;
  }

  PLMatrix4 mrot = plRotateMatrix4(angles_.x, PLVector3(1, 0, 0));
  mrot = mrot * plRotateMatrix4(angles_.y, PLVector3(0, 1, 0));
  mrot = mrot * plRotateMatrix4(angles_.z, PLVector3(0, 0, 1));
  model_->model_matrix = mrot * plTranslateMatrix4(position_);

  plDrawModel(model_);
}

void AModel::SetModel(const std::string &path) {
  model_ = ModelManager::GetInstance()->LoadCachedModel("chars/" + path, false);
}
