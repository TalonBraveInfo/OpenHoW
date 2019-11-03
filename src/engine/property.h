/* OpenHoW
 * Copyright (C) 2017-2019 Daniel Collins <solemnwarning@solemnwarning.net>
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

#include <map>
#include <sstream>
#include <string>
#include <string.h>

/**
 * @defgroup PropertyFlags Property flags
 */
/*@{*/

#define PROP_PUSH       (1U << 0)  /**< Property changes should be pushed from client to server.
                                        Implies PROP_IMMEDIATE. */

#define PROP_IMMEDIATE  (1U << 1)  /**< Property changes should be propogated from server
                                        immediately */

#define PROP_DISCARD    (1U << 2)  /**< Property value can be discarded when saving/loading game */

#define PROP_LOCAL      (1U << 3)  /**< Property value is not sync'd between peers */

/*@}*/

/**
 * @brief Helper macro for initialising properties.
 *
 * @param name   Name of member (bareword)
 * @param flags  PROP_XXX flags
 * @param ...    Extra parameters to property constructor
*/
#define INIT_PROPERTY(name, flags, ...) name(*this, #name, flags, ##__VA_ARGS__)

class PropertyOwner;

/**
 * @brief Base class for all properties. Pure virtual.
*/
class Property
{
	public:
		const std::string name;
		const unsigned flags;
		
		/* No copy/assignment c'tors. */
		Property(const Property&) = delete;
		Property& operator=(const Property&) = delete;
		
		/**
		 * @brief Returns the serialised form of the property's value.
		 *
		 * Serialises the property's value using std::string as a container. The value may
		 * or may not be a printable string; treat it as a byte array.
		*/
		virtual std::string Serialise() const = 0;
		
		/**
		 * @brief Set the property to the given serialised form.
		 *
		 * Sets the value to the given serialised one, which must have been returned from
		 * a call to Serialise(). Marks the property as dirty.
		 *
		 * Throws XXX and makes no change to the property if the serialised form isn't
		 * valid.
		*/
		virtual void Deserialise(const std::string &serialised) = 0;
		
		/**
		 * @brief Mark the property as clean and save the current value.
		*/
		void MarkClean();
		
		/**
		 * @brief Mark the property as dirty.
		 *
		 * If the property is clean, records the current tick count and marks it as dirty,
		 * otherwise does nothing.
		*/
		void MarkDirty();
		
		/**
		 * @brief Restore the last clean value and mark the property clean.
		 *
		 * Restores the value saved in the last call to MarkClean() and marks the property
		 * as clean again.
		*/
		void ResetToClean();
		
		/**
		 * @brief Returns the number of ticks the property has been dirty for.
		*/
		unsigned int DirtyTicks() const;
		
	protected:
		PropertyOwner &po_;
		
		Property(PropertyOwner &po, const std::string &name, unsigned flags);
		virtual ~Property();
		
	private:
		bool is_dirty_;
		unsigned int dirty_since_;
		
		std::string clean_serialised_;
};

typedef std::map<std::string, Property*> PropertyMap;

/**
 * @brief Base class for actors, game modes, etc to allow them to have properties.
*/
class PropertyOwner
{
	friend Property;
	
	public:
		/* No copy/assignment c'tors. */
		PropertyOwner(const PropertyOwner&) = delete;
		PropertyOwner& operator=(const PropertyOwner&) = delete;

		const PropertyMap& GetProperties() { return properties_; }
	
	protected:
		PropertyOwner();
		virtual ~PropertyOwner();
		
	private:
		/** Properties registered under this object */
        PropertyMap properties_;
};

/**
 * @brief Template class for numeric (integer or float) properties.
*/
template<typename T> class NumericProperty: public Property
{
	private:
		T value_;
		
	public:
		NumericProperty(PropertyOwner &po, const std::string &name, unsigned flags, T value = 0):
			Property(po, name, flags), value_(value) {}
		
		/* Implicit conversion for using as a (const) T */
		operator const T&() const
		{
			return value_;
		}
		
		/**
		 * @brief Assigns value, marks property dirty.
		 *
		 * @return Reference to provided value
		*/
		const T& operator=(const T& value)
		{
			this->value_ = value;
			MarkDirty();
			
			return value;
		}
		
		std::string Serialise() const override
		{
			return std::string((const char*)(&value_), sizeof(value_));
		}
		
		void Deserialise(const std::string &serialised) override
		{
			u_assert(serialised.length() == sizeof(value_));
			memcpy(&value_, serialised.data(), sizeof(value_));
			MarkDirty();
		}
};

/**
 * @brief Property class for boolean properties.
*/
class BooleanProperty: public Property
{
	private:
		bool value_;
		
	public:
		BooleanProperty(PropertyOwner &po, const std::string &name, unsigned flags, bool value = false):
			Property(po, name, flags), value_(value) {}
		
		/* Implicit conversion for using as a (const) bool */
		operator const bool&() const
		{
			return value_;
		}
		
		/**
		 * @brief Assigns value, marks property dirty.
		 *
		 * @return Reference to provided value
		*/
		const bool& operator=(const bool& value)
		{
			value_ = value;
			MarkDirty();
			
			return value;
		}
		
		std::string Serialise() const override
		{
			return value_
				? "true"
				: "false";
		}
		
		void Deserialise(const std::string &serialised) override
		{
			if(serialised == "false")
			{
				value_ = false;
				MarkDirty();
			}
			else if(serialised == "true")
			{
				value_ = true;
				MarkDirty();
			}
			else{
				/* TODO: Have a this-isn't-valid exception */
				abort();
			}
		}
};
