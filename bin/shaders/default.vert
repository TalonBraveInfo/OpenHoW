#include "./shared.vert"

/* some random comments
 * and some more
 * and some more
 */

// single line comment

void main() {
    gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}