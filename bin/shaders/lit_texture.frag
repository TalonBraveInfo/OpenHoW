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
uniform float sun_yaw = 0;
uniform float sun_pitch = 0;

uniform vec4 ambient_colour = vec4(1.0, 1.0, 1.0, 1.0);

in vec3 interp_normal;
in vec2 interp_UV;
in vec4 interp_colour;

in vec3 frag_pos;

void main() {
    vec4 diffuse = (ambient_colour * interp_colour) * texture(diffuse, interp_UV);

    vec3 normal = normalize(interp_normal);

    // todo: these are hardcoded for now, just for testing
    vec3 light_direction = normalize(vec3(2.0 * sin(sun_pitch), sun_yaw, 0) - frag_pos); //normalize(light_position - frag_pos);
    diffuse += (max(dot(normal, light_direction), 0.0)) * sun_colour;
    // uncomment to check normals...
    //diffuse = vec4(normal, 1.0);

    float fog_distance = (gl_FragCoord.z / gl_FragCoord.w) / (fog_far * 100.0);
    float fog_amount = 1.0 - fog_distance;
    fog_amount *= -(fog_near / 100.0);

    pl_frag = mix(diffuse, fog_colour, clamp(fog_amount, 0.0, 1.0));
}
