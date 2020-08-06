/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

out vec3 interp_normal;
out vec2 interp_UV;
out vec4 interp_colour;

out vec3 frag_pos;

uniform vec4 sun_colour = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec3 sun_position = vec3(0.0, 0.0, 0.0);
uniform vec4 ambient_colour = vec4(1.0, 1.0, 1.0, 1.0);

void main() {
	gl_Position = pl_proj * pl_view * pl_model * vec4(pl_vposition, 1.0f);

	interp_normal = mat3(transpose(inverse(pl_model))) * pl_vnormal;
	interp_UV = pl_vuv;

	vec3 l = normalize( -sun_position );
	vec3 v = normalize( -gl_Position.xyz );
	vec3 n = normalize( interp_normal );

	vec3 diffuse = max( dot( n, l ), 0.0 ) * sun_colour.rgb + ambient_colour.rgb;

	interp_colour = pl_vcolour * vec4( diffuse.rgb, 1.0 );

	frag_pos = vec3(pl_model * vec4(pl_vposition, 1.0));
}
