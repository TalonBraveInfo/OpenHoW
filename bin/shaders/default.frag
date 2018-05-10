
uniform sampler2D diffuse;

void main() {
	gl_FragColor = texture2D(diffuse, gl_TexCoord[0].st);
}
