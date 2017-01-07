#version 130

// Testing ways of getting the cube into world coords and then projection space:
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;

in vec3 position;
out vec4 frag_color;

uniform vec3 cube_coords;
uniform vec4 cube_color;


mat4 projection(
    float angle_of_view_y,
    float aspect_ratio,
    float z_near,
    float z_far
    ) {
    // from https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml
    return mat4(
             vec4(1.0/(tan(angle_of_view_y)*aspect_ratio), 0.0, 0.0, 0.0),
             vec4(0.0, angle_of_view_y, 0.0, 0.0),
             vec4(0.0, 0.0, (z_far+z_near)/(z_far-z_near), -1.0),
             vec4(0.0, 0.0, (2.0*z_far*z_near)/(z_near-z_far), 0.0)
           );
}


mat4 rotate_x(float theta) {
    return mat4(
        vec4(1.0,         0.0,        0.0, 0.0),
        vec4(0.0,  cos(theta), sin(theta), 0.0),
        vec4(0.0, -sin(theta), cos(theta), 0.0),
        vec4(0.0,         0.0,        0.0, 1.0)
    );
}

mat4 rotate_y(float theta) {
    return mat4(
        vec4(cos(theta), 0.0, -sin(theta), 0.0),
        vec4(       0.0, 1.0,         0.0, 0.0),
        vec4(sin(theta), 0.0,  cos(theta), 0.0),
        vec4(       0.0, 0.0,         0.0, 1.0)
    );
}

mat4 rotate_z(float theta) {
    return mat4(
        vec4( cos(theta), sin(theta), 0.0, 0.0),
        vec4(-sin(theta), cos(theta), 0.0, 0.0),
        vec4(        0.0,        0.0, 1.0, 0.0),
        vec4(        0.0,        0.0, 0.0, 1.0)
    );
}


// New rotation matrix for rotating around mutliple axis. Can use this once rather
// than trying to use a combination of the other 3, but rotates in the opposite direction.
// Uses "angleRad" rather than "theta" to remind me that it's an angle, in radians.
// This was mostly copied from:
// http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/

mat4 rotate(float x, float y, float z, float angleRad) {
	vec3  a  = normalize(vec3(x, y, z));
	float s  = sin(angleRad);
	float c  = cos(angleRad);
	float oc = 1.0 - c;       // Not sure exactly what "oc" stands for here, "opposite cos"?
	
    return mat4(oc * a.x * a.x + c      , oc * a.x * a.y - a.z * s, oc * a.z * a.x + a.y * s,  0.0,
                oc * a.x * a.y + a.z * s, oc * a.y * a.y + c      , oc * a.y * a.z - a.x * s,  0.0,
                oc * a.z * a.x - a.y * s, oc * a.y * a.z + a.x * s, oc * a.z * a.z + c      ,  0.0,
                0.0                     , 0.0                     , 0.0                     ,  1.0);
}

mat4 translate(float x, float y, float z) {
    return mat4(
             vec4(1.0, 0.0, 0.0, 0.0),
             vec4(0.0, 1.0, 0.0, 0.0),
             vec4(0.0, 0.0, 1.0, 0.0),
             vec4(  x,   y,   z, 1.0)
           );
}

void main() {
      gl_Position = projection(radians(45.0), 16.0/9.0, -0.1, -1000.0)  // projection?
      				* translate(0.0, 0.0, -3.0)                       // Making everything a little bit more.. away.
                    * translate(cube_coords[0], cube_coords[1], cube_coords[2])   // view? Moves cubes around based on their (world?) coords
                    * rotate(0.0, 0.0, 1.0, radians(30.0))            // model? Rotates each cube individually.
                    * vec4(position, 1.0f);
      frag_color = cube_color * 0.7; // Makes the cubes a bit darker. Just experimenting for now!
}