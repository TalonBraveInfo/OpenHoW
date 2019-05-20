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

#include "../engine.h"
#include "../model.h"

#include "ModelActor.h"

ModelActor::ModelActor(const std::string& name) : Actor(name) {}
ModelActor::~ModelActor() {
    plDestroyModel(model_);
}

void ModelActor::Draw() {
    if(model_ != nullptr) {
        model_->model_matrix = plTranslateMatrix(position_);
        plDrawModel(model_);
    }

    Actor::Draw();
}

void ModelActor::SetModel(const std::string &path) {
    model_ = Model_LoadFile(std::string("/chars" + path).c_str(), false);
}
