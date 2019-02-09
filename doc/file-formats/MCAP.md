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