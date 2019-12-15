/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

uniform sampler2D diffuse;

uniform float fog_far = 0;
uniform float fog_near = 0;
uniform vec4 fog_colour = vec4(1.0, 1.0, 1.0, 1.0);

uniform vec4 sun_colour = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec3 sun_position = vec3(0.0, 0.0, 0.0);

uniform vec4 ambient_colour = vec4(1.0, 1.0, 1.0, 1.0);

in vec3 interp_normal;
in vec2 interp_UV;
in vec4 interp_colour;

in vec3 frag_pos;

void main() {
    vec4 dsample = texture(diffuse, interp_UV);
    if(dsample.a < 0.1) {
        discard;
    }

    vec3 normal = normalize(interp_normal);
    vec3 light_direction = normalize(-sun_position);
    vec4 sun_term = (max(dot(normal, light_direction), 0.0)) * sun_colour + ambient_colour;
    vec4 diffuse_colour = sun_term * interp_colour * dsample;
    // uncomment to check normals...
    //diffuse_colour = vec4(normal, 1.0);

    // rim term
    //vec3 rnormal = normalize(mat3(pl_view) * interp_normal);
    //vec3 vpos = vec3(pl_view * )

    float fog_distance = (gl_FragCoord.z / gl_FragCoord.w) / (fog_far * 100.0);
    float fog_amount = 1.0 - fog_distance;
    fog_amount *= -(fog_near / 100.0);

    pl_frag = mix(diffuse_colour, fog_colour, clamp(fog_amount, 0.0, 1.0));
}
