/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
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

namespace ohw {

enum class PhysicsPrimitiveType {
	SPHERE,
	BOX,
	CAPSULE,
	CYLINDER,
	CONE,
	CHAMFER_CYLINDER,
	RANDOM_CONVEX_HULL,
	REGULAR_CONVEX_HULL,
	COMPOUND_CONVEX_CRUZ,
};

class PhysicsBody {
public:
protected:
	PhysicsBody() = default;
	virtual ~PhysicsBody() = default;

private:
};

class PhysicsInterface {
public:
	PhysicsInterface() = default;
	virtual ~PhysicsInterface() = default;

	virtual void Tick();

	virtual PhysicsBody *CreatePhysicsBody();
	virtual void DestroyPhysicsBody( PhysicsBody *body );

	virtual void GenerateTerrainCollision( std::vector<float> vertices );
	virtual void DestroyTerrainCollision();

protected:
private:
};

}
