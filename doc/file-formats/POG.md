# POG - Map object data

```c

2   ushort  objectCount
for(int iObject = 0; iObject < objectCount; iObject++) // sizeof = 94
{
    16  char[]  obj.Name                //Model name
    16  char[]  obj.NULL                //"NULL"
    2   short   obj.XOffset
    2   short   obj.YOffset
    2   short   obj.ZOffset
    
    2   short   obj.ID
    2   short   obj.XRotation           // 4096 = 360°
    2   short   obj.YRotation           // 4096 = 360°
    2   short   obj.ZRotation           // 4096 = 360°
    2   short   obj.TypeID              // Model type id (instancing)
    
    2   short   obj.BoundingBoxWidth
    2   short   obj.BoundingBoxHeight
    2   short   obj.BoundingBoxLength

    // BoundingBoxType:
    // - Box = 0
    // - Prism = 1
    2   short   obj.BoundingBoxType
        
    1   short   obj.Short0
    1   char    obj.Byte0

    // PigTeam:
    // - Team01 = 0x01
    // - Team02 = 0x02
    // - Team03 = 0x04
    // - Team04 = 0x08
    // - Team05 = 0x10
    // - Team06 = 0x20
    // - Team07 = 0x40
    // - Team08 = 0x80
    1   char    obj.Team
    
    // ScriptType:
    // - DESTROY_ITEM           = 01
    // - DESTROY_PROPOINT       = 02
    // - PROTECT_ITEM           = 03
    // - PROTECT_PROPOINT       = 04
    // - DROPZONE_ITEM          = 07
    // - DROPZONE_PROPOINT      = 08
    // - DESTROY_GROUP_ITEM     = 13
    // - DESTROY_GROUP_PROPOINT = 14
    // - PICKUP_ITEM            = 19
    // - TUTORIAL_DESTROY       = 20
    // - TUTORIAL_END           = 21
    // - TUTORIAL_BLAST         = 22
    // - TUTORIAL_DESTROY_GROUP = 23
    2   short   obj.ScriptType

    1   char    obj.ScriptGroup    
    19  char[]  obj.ScriptParamters
    2   short   obj.ScriptXOffset
    2   short   obj.ScriptYOffset
    2   short   obj.ScriptZOffset
    
    // ObjectFlag:
    // - None      = 0
    // - Player    = 1 << 0,
    // - Bit1      = 1 << 1,
    // - Bit2      = 1 << 2,
    // - Bit3      = 1 << 3,
    // - ScriptObj = 1 << 4,
    // - Inside    = 1 << 5
    // - Delayed   = 1 << 6
    // - Bit7      = 1 << 7
    2   short   obj.Flag

    2   short   obj.Short1
    2   short   obj.Short2
}
// EOF
```