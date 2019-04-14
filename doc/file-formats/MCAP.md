# MCAP - Motion capture

.mcap are extracted from mcap.mad

```c
foreach(keyframe) //sizeof = 272
{
    2   ushort  unkUShort0
    foreach(boneTransform) //per bone branch? (none-leaf-bone) = 10
    {
        //Vector3
        1   char   transform.X
        1   char   transform.Y
        1   char   transform.Z
    }
    
    foreach(boneRotation) //sizeof = 15
    {        
        //Quaternion
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
