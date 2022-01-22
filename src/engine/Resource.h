// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#define IMPLEMENT_RESOURCE_CLASS( A ) \
inline size_t GetMemoryUsage() const override { return sizeof( A ); }

namespace ohw {
	template< typename T >
	class SharedResourcePointer {
	private:
		T *ptr;

	public:
		SharedResourcePointer( T *p ) : ptr( p ) {
			if ( ptr != nullptr ) {
				ptr->AddReference();
			}
		}

		SharedResourcePointer( const SharedResourcePointer< T > &src ) : ptr( src.ptr ) {
			if ( ptr != nullptr ) {
				ptr->AddReference();
			}
		}

		~SharedResourcePointer() {
			if ( ptr != nullptr ) {
				ptr->Release();
			}
		}

		SharedResourcePointer &operator=( const SharedResourcePointer< T > &rhs ) {
			if ( rhs.ptr != nullptr ) {
				rhs.ptr->AddReference();
			}

			if ( ptr != nullptr ) {
				ptr->Release();
			}

			ptr = rhs.ptr;

			return *this;
		}

		operator T *() const {
			return ptr;
		}

		T *operator->() const {
			return ptr;
		}
	};

	class Resource {
	public:
		explicit Resource( const std::string &path, bool persist = false );
		virtual ~Resource();

		inline unsigned int AddReference() { return ( ++numReferences ); }
		inline unsigned int GetReferenceCount() const { return numReferences; }
		void Release();

		bool CanDestroy() const;

		virtual size_t GetMemoryUsage() const = 0;

		// TODO: GetAbsolutePath() ...
		inline const std::string &GetPath() const { return referencePath; }

	private:
		std::string referencePath;

		unsigned int numReferences{ 0 };
		bool persist{ false };
	};
}
