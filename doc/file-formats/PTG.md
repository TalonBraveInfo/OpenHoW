# PTG

Contains the terrain tile textures (.tim). 
All files have to be the same size in bytes.

```c
4   int    textureCount
foreach(texture) //sizeof = (ptgFileSize - 4) / textureCount
{
    *   char[]  textureBuffer
}
```