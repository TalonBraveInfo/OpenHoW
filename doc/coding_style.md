# C/C++ Style Guide

```c++

struct ThisThing {
	int someVar, someOtherVar;
};

class ThatThing {
public:

	unsigned int GetSomeVar() const { return someVar; }
	void SetSomeVar( unsigned int inSomeVar ) {
		someVar = inSomeVar;
	}

protected:
private:

	// Always use the initialisers
	unsigned int myVar { 0 };
};

// (OpenHoW) Actor derived class prefixed with 'A'
class ATank : public Actor {}

// Interface class prefixed with 'I'
class IPhysicsBody {
public:
	virtual void SomeMethod() = 0;
	virtual void SomeOtherMethod() = 0;
}

// Template class prefixed with 'T'
template<T> class TVector {}

void Function( void ) {
	// Tabs are used ( tab size = 4 )
	
	printf( "Hello World!\n" );
	
	// Variables use mixed case
	int castMe = 0;
	bool castYou = static_cast< bool >( castMe );
	
	bool fooVar = false;
	if ( !fooVar ) {
		printf( "False Var\n" );
	}
	
	void *fooPointer = NULL;
	if ( fooPointer == NULL ) {
		printf( "Pointer is null\n" );
	}
	
	struct {
		// Variables are aligned by spaces
		int   someVar;
		float someVar2;
	} myStruct;
}

```

## Platform Library

```c

// The platform library is primarily C

#define PL_THING_DEFAULT_SOMETHING	2

enum {
    PL_THING_TYPE_AWESOME,
    PL_THING_TYPE_BAD,
    PL_THING_TYPE_GOOD,
};

// Struct types are prefixed with 'PL'
typedef struct PLThing {
	int   someThing;
	float someThing2;
} PLThing;

PLThing *plCreateThing( int someThing, float someThing ) {
	PLThing *thing = pl_malloc( sizeof( PLThing ) );
	thing->someThing = someThing;
	thing->someThing2 = someThing;
	return thing;
}

bool plIsValidThing( PLThing *thing ) {
	return ( bool )( thing != NULL );
}

```
