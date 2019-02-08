# MAD & MTD 

### Most MAD & MTD packages follow the "named structure"
```c
foreach(namedFileInfo) //sizeof = 24
{
    16  char[]  entry.Name
    4   int     entry.DataOffset  // within this file.
    4   int     entry.DataSize
}
// DATA
// EOF
```

### "mcap.mad" follows the "raw structure"
```c
foreach(rawFileInfo) //sizeof = 8
{
    4   int     entry.DataOffset  //within this file.
    4   int     entry.DataSize
}
// DATA
// EOF
```