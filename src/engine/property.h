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

#define PROP_WRITE      (1U << 4)  /**< Property value can be modified locally */

/*@}*/

/**
 * @brief Helper macro for initialising properties.
 *
 * @param name   Name of member (bareword)
 * @param flags  PROP_XXX flags
 * @param ...    Extra parameters to property constructor
*/
#define INIT_PROPERTY(name, flags, ...) name(*this, #name, flags, ##__VA_ARGS__)

/**
 * @brief Helper macro for copying-constructing properties.
 *
 * @param name  Name of member (bareword)
 * @param src   Name of source structure (bareword)
*/
#define COPY_PROPERTY(name, src) name(*this, src.name)

class PropertyOwner;

/**
 * @brief Base class for all properties. Pure virtual.
*/
class Property {
public:
	const std::string name;
	const std::string description;
	const unsigned flags;

	/* No copy/assignment c'tors. */
	Property( const Property& ) = delete;
	Property& operator=( const Property& ) = delete;

	/**
	 * @brief Returns the serialised form of the property's value.
	 *
	 * Serialises the property's value using std::string as a container. The value may
	 * or may not be a printable string; treat it as a byte array.
	*/
		virtual std::string Serialise() const = 0;

        /**
         * Returns the property formatted as Json.
         */
        virtual std::string SerialiseAsJson() const = 0;
		
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
		Property(PropertyOwner &po, const Property &src);
		virtual ~Property();
		
	private:
		bool is_dirty_;
		unsigned int dirty_since_{ 0 };
		
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
		/* These are no-ops because copying or assigning a PropertyOwner class should just
		 * assign/copy the properties under it, which will be registered when the property
		 * is copy-constructed, or already registered in the case of assignment.
		*/
		PropertyOwner(const PropertyOwner&) {}
		PropertyOwner& operator=(const PropertyOwner&) { return *this; }

		const PropertyMap& GetProperties() { return properties_; }

		virtual std::string SerializePropertiesAsJson();
	
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
		
		NumericProperty(PropertyOwner &po, const NumericProperty<T> &src):
			Property(po, src), value_(src.value_) {}
		
		/* Implicit conversion for using as a (const) T */
		operator const T&() const
		{
			return value_;
		}

        const T& operator=(const NumericProperty& value)
        {
          this->value_ = value;
          MarkDirty();
          return value;
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

        std::string SerialiseAsJson() const override {
            return std::to_string(value_);
        }
		
		void Deserialise(const std::string &serialised) override
		{
			u_assert(serialised.length() == sizeof(value_));
			memcpy(&value_, serialised.data(), sizeof(value_));
			MarkDirty();
		}
};

class VectorStringProperty : public Property {
 private:
  std::vector<std::string> value_;

 public:
  VectorStringProperty(PropertyOwner& po, const std::string& name, unsigned int flags,
                       const std::vector<std::string>& value = {}) :
      Property(po, name, flags), value_(value) {}

  operator const std::vector<std::string>&() const {
    return value_;
  }

  const std::vector<std::string>& operator=(const std::vector<std::string>& value) {
    value_ = value;
    MarkDirty();
    return value;
  }

  std::string Serialise() const override {
    std::string value;
    for(const auto& i : value_) {
      uint32_t l = i.length();
      value += std::string((const char*)(&l), sizeof(l)) + i;
    }
    return value;
  }

  std::string SerialiseAsJson() const override {
    std::string str = "[";
    for(size_t i = 0; i < value_.size(); ++i) {
      if(i > 0) {
        str += ",";
      }
      str += "\"" + value_[i] + "\"";
    }
    str += "]";
    return str;
  }

  void Deserialise(const std::string& serialised) override {
    value_.clear();

    /* TODO: Assertions should be a bad-serialised-value exception */
    for (size_t i = 0; i < serialised.length();) {
      assert((i + sizeof(uint32_t)) < serialised.length());
      uint32_t l = *(uint32_t*) (serialised.data() + i);
      i += sizeof(uint32_t);

      assert((i + l) < serialised.length());
      value_.emplace_back((serialised.data() + i), l);
      i += l;
    }

    MarkDirty();
  }
};

class StringProperty : public Property {
 private:
  std::string value_;

 public:
  StringProperty(PropertyOwner& po, const std::string& name, unsigned int flags, const std::string& value = "") :
      Property(po, name, flags), value_(value) {}

  StringProperty(PropertyOwner& po, const StringProperty &src):
      Property(po, src), value_(src.value_) {}

  operator const std::string&() const {
    return value_;
  }

  const char *c_str() const {
    return value_.c_str();
  }

  const std::string& operator=(const StringProperty& value) {
    value_ = value;
    MarkDirty();
    return value;
  }

  const std::string& operator=(const std::string& value) {
    value_ = value;
    MarkDirty();
    return value;
  }

  std::string Serialise() const override {
    return value_;
  }

	std::string SerialiseAsJson() const override {
		return "\"" + value_ + "\"";
	}

	void Deserialise( const std::string& serialised ) override {
		value_ = serialised;
		MarkDirty();
	}
};

/**
 * @brief Property class for vector3 properties.
 */
class Vector3Property : public Property {
private:
	PLVector3 value_;

public:
	Vector3Property( PropertyOwner& po, const std::string& name, unsigned flags,
					 PLVector3 value = PLVector3( 0, 0, 0 ) ) :
		Property( po, name, flags ), value_( value ) {}

	operator const PLVector3&() const {
		return value_;
	}

	const PLVector3& GetValue() const {
		return value_;
	}

	const PLVector3& operator=( const PLVector3& value ) {
		value_ = value;
		MarkDirty();
		return value;
	}

	std::string SerialiseAsJson() const override {
		return std::string(
			std::to_string( value_.x ) + " " +
				std::to_string( value_.y ) + " " +
				std::to_string( value_.z ) );
	}

	std::string Serialise() const override { return ""; } // todo
	void Deserialise( const std::string& serialised ) override {} // todo
};

/**
 * @brief Property class for boolean properties.
*/
class BooleanProperty : public Property {
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

		std::string SerialiseAsJson() const override {
		  return Serialise();
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
