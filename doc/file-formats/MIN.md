# MIN - Model data

**!!This page is still WIP!!<br> 
Most of this has been deduced by observation thus far as it's not a priority right now.**

```c
int8_t      u0[16];
```

Then followed by face data.

```c
uint32_t    num_faces;
struct {
    char u0[24];
} faces[num_faces];

int32_t     u1;
```

32-bit integer seems to follow after faces, unsure why.

This is then followed by another block of unknown data, could
be vertices (however number of indices doesn't match for faces).

```c
uint32_t    num_u2;
struct {
    char unknown[16];
} u2[num_u2];
```
