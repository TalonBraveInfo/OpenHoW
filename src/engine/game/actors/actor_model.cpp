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

AModel::AModel(const ActorSpawn& spawn) : Actor(spawn) {}
AModel::AModel() : Actor() {}
AModel::~AModel() = default;

void AModel::Draw() {
  if (class_name_ == "dummy") {
    angles_.y += TICKS_PER_SECOND / 1000.f;
  }

  if (model_ != nullptr) {
    // todo: ++'ify... and should actors use degrees by defacto?

    PLMatrix4 mrot = plRotateMatrix4(angles_.x, PLVector3(1, 0, 0));
    mrot = plMultiplyMatrix4(mrot, plRotateMatrix4(angles_.y, PLVector3(0, 1, 0)));
    mrot = plMultiplyMatrix4(mrot, plRotateMatrix4(angles_.z, PLVector3(0, 0, 1)));

    model_->model_matrix = plMultiplyMatrix4(mrot, plTranslateMatrix4(position_));

    plDrawModel(model_);
  }

  Actor::Draw();
}

void AModel::SetModel(const std::string &path) {
  model_ = ModelManager::GetInstance()->LoadCachedModel("chars/" + path, false);
}
