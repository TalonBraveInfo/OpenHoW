# TAB - Text art block

Describes a section within a font tim used for a letter.

```c
foreach(block) //sizeof = 8
{
    2   ushort  block.XOffset
    2   ushort  block.YOffset
    2   ushort  block.Width
    2   ushort  block.Height    
}
```