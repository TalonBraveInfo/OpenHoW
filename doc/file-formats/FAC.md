# FAC - Model faces

```c
16  char[]  reserved                    // reserved for name?
4   int     triangleCount
foreach(triangle) // sizeof = 32
{
    1   char   triangle.U_A             // U component for Vertex_A
    1   char   triangle.V_A             // U component for Vertex_A

    1   char   triangle.U_B             // U component for Vertex_B
    1   char   triangle.V_B             // V component for Vertex_B

    1   char   triangle.U_C             // U component for Vertex_C	
    1   char   triangle.V_C             // V component for Vertex_C

    2   short  triangle.Vertex_A        // index from .VTX
    2   short  triangle.Vertex_B        // index from .VTX
    2   short  triangle.Vertex_C        // index from .VTX

    2   short  triangle.Normal_A        // index from .NO2
    2   short  triangle.Normal_B        // index from .NO2
    2   short  triangle.Normal_C        // index from .NO2
    
    2   short  triangle.Short0
    
    4   int    triangle.TextureIndex    // index from .MTD
    
    2   short  triangle.Short1
    2   short  triangle.Short2
    2   short  triangle.Short3
    2   short  triangle.Short4
}

4   int    planeCount
foreach(plane)   //36 bytes per plane
{
    1   char   plane.U_A                // U component for Vertex_A
    1   char   plane.V_A                // U component for Vertex_A

    1   char   plane.U_B                // U component for Vertex_B
    1   char   plane.V_B                // V component for Vertex_B

    1   char   plane.U_C                // U component for Vertex_C	
    1   char   plane.V_C                // V component for Vertex_C

    1   char   plane.U_D                // U component for Vertex_D
    1   char   plane.V_D                // V component for Vertex_D

    2   short  plane.Vertex_A           // index from .VTX
    2   short  plane.Vertex_B           // index from .VTX
    2   short  plane.Vertex_C           // index from .VTX
    2   short  plane.Vertex_D           // index from .VTX

    2   short  plane.Normal_A           // index from .NO2
    2   short  plane.Normal_B           // index from .NO2
    2   short  plane.Normal_C           // index from .NO2
    2   short  plane.Normal_D           // index from .NO2    

    4   int    plane.TextureIndex       // index from .MTD
    
    2   short  plane.Short1
    2   short  plane.Short2
    2   short  plane.Short3
    2   short  plane.Short4
}
// EOF
```