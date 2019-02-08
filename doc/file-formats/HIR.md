# HIR - Model skeleton

Skeleton used for pigs

```c
foreach(bone) //sizeof = 20
{
    4   int     bone.ParentIndex
    2   short   bone.XOffset
    2   short   bone.YOffset
    2   short   bone.ZOffset
    10  char[]  bone.reserved   //0 -> rotation?
}
// EOF
```

| Index | Bone 
| ----- | ----- 
| 0     | Hip
| 1     | Spine
| 2     | Head
| 3     | UpperArm.L
| 4     | LowerArm.L
| 5     | Hand.L
| 6     | UpperArm.R
| 7     | LowerArm.R
| 8     | Hand.R
| 9     | UpperLeg.L
| 10    | LowerLeg.L
| 11    | Foot.L
| 12    | UpperLeg.R
| 13    | LowerLeg.R
| 14    | Foot.R

![Pig Skeleton](images/pig_skeleton.png)
