
#include "model.h"

// Skeletal Animation

/*  HIR Format Specification    */
/* Used to store our piggy bones.
 */

// P        X   Y    Z   ?    ?   ?    ?   ?

// 00000000 0000EBFF 01000000 00000000 00000000
// 01000000 06004FFF 02000000 00000000 00000000
// 01000000 FDFF9BFF 58000000 00000000 00000000
// 03000000 03000000 6E000000 00000000 00000000
// 04000000 FDFF0000 6F000000 00000000 00000000
// 01000000 FEFF9BFF A9FF0000 00000000 00000000

typedef struct __attribute__((packed)) HIRBone {
    uint32_t parent; // not 100% on this, links in with
    // animations from the looks of it, urgh...

    int16_t coords[3];
    int16_t padding;

    int32_t unknown0;
    int32_t unknown1;
} HIRBone;
