# PMG - Map mesh data

A map is 32768 x 32768 units and consists of 16 x 16 blocks.  
A block is 2048 x 2048 units and consists of 4x4 tiles.  
A tile is 512 x 512 units.

```c
for (int yBlock = 0; yBlock < 16; yBlock++)
{
    for (int xBlock = 0; xBlock < 16; xBlock++)
    {
        2   short  block.xOffset                   // -16384 ~ 16384
        2   short  block.yOffset                   // unreliable?
        2   short  block.zOffset                   // -16384 ~ 16384
        2   short  block.Field0
        
        // HeightMap
        for (int yVertex = 0; yVertex < 5; yVertex++)
        {
            for (int xVertex = 0; xVertex < 5; xVertex++)
            {
                2   short   vertex.height                // height sample
                2   short   vertex.lighting              // <= 255
            }
        }
        
        4   int   block.Field1                  // always 0 -> obsolete tileCount?
        
        // TileMap
        for (int yTile = 0; yTile < 4; yTile++)
        {
            for (int xTile = 0; xTile < 4; xTile++)
            {   
                2   short   tile.Field0          // always 0 -> obsolete xOffset?
                2   short   tile.Field1          // always 0 -> obsolete yOffset?
                2   short   tile.Field2          // always 0 -> obsolete zOffset?

                // MapTileType
                // (MSB)                               (LSB)
                // | 07 | 06 | 05 | 04 | 03 | 02 | 01 | 00 |
                // | F3 | F2 | F1 |           T1           |
                // T1 = Type (0x1F)
                //      Mud   = 00
                //      Grass = 01
                //      Metal = 02
                //      Wood  = 03
                //      Water = 04
                //      Stone = 05
                //      Rock  = 06
                //      Sand  = 07
                //      Ice   = 08
                //      Snow  = 09
                //      Quag  = 10
                //      Lava  = 11
                // F1 = IsWater (0x20)
                // F2 = IsMine (0x40)
                // F3 = IsWall (0x80)
                1   char    tile.Type

                // MapTileSlip (requires "IsWall"?)
                //      Full        = 0
                //      Bottom      = 1
                //      Left        = 2
                //      Top         = 3
                //      Right       = 4
                //      BottomRight = 5
                //      BottomLeft  = 6
                //      TopLeft     = 7
                //      TopRight    = 8
                1   char    tile.Slip

                2   short   tile.Field3             // always 0

                // MapTileRotateFlip
                // FlipX     = 1
                // Rotate90  = 2
                // Rotate180 = 4
                1   char    tile.RotationFlip

                4   int     tile.TextureIndex       // into PTG
                1   char    tile.Field4             // always 0
            }
        }
    }
}
// EOF
```