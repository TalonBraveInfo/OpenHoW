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

class Actor;

typedef std::set<Actor *> ActorSet;

class ActorManager {
protected:
	typedef Actor *(*actor_ctor_func)();
	static std::map<std::string, actor_ctor_func> actor_classes_;

public:
	static ActorManager *GetInstance() {
		static ActorManager *instance = nullptr;
		if ( instance == nullptr ) {
			instance = new ActorManager();
		}
		return instance;
	}

	Actor *CreateActor( const std::string &class_name, const ActorSpawn &spawnData = ActorSpawn() );
	void DestroyActor( Actor *actor );

	void TickActors();
	void DrawActors();
	void DestroyActors();

	void ActivateActors();
	void DeactivateActors();

	const ActorSet &GetActors() const { return actors_; }

	class ActorClassRegistration {
	public:
		const std::string name_;

		ActorClassRegistration( const std::string &name, actor_ctor_func ctor_func );
		~ActorClassRegistration();
	};

private:
	static ActorSet actors_;
	static std::vector<Actor *> destructionQueue;
};

#define REGISTER_ACTOR( NAME, CLASS ) \
    static Actor * NAME ## _make() { return new CLASS (); } \
    static ActorManager::ActorClassRegistration __attribute__ ((init_priority(2000))) \
    _reg_actor_ ## NAME ## _name((#NAME), NAME ## _make); // NOLINT(cert-err58-cpp)
#define REGISTER_ACTOR_BASIC( CLASS ) REGISTER_ACTOR( CLASS, CLASS )
