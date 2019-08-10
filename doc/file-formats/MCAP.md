# MCAP - Motion capture

.mcap are extracted from mcap.mad

```c
foreach(keyframe) //sizeof = 272
{
    // World space transform
    // Transform32    
    4   int rootTransform.X
    4   int rootTransform.Y
    4   int rootTransform.Z
    4   int rootTransform.W   // unused 
    
    // Object space transform
    // Transform32
    4   int objectTransform.X
    4   int objectTransform.Y
    4   int objectTransform.Z
    4   int objectTransform.W   // unused
    
    foreach(boneRotation) //sizeof = 15
    {        
        //Rotation32 (Quaternion)
        4   float   rotation.X
        4   float   rotation.Y
        4   float   rotation.Z
        4   float   rotation.W
    }
}
// EOF
```

----

```c++
struct CaptureRotation {
#if 1
    uint16_t x, y, z, w;
#else
    uint8_t x, y, z, w;
#endif
};

struct CaptureFrame {
    uint16_t        Root[4];    // xyz(pad)
    uint16_t        Object[4];  // xyz(pad)
	CaptureRotation pt[15];
};
```
