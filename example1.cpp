


#if 1

//===========================================================================
//
// Cube on cube on sphere
//
//
//===========================================================================

#define _CRT_SECURE_NO_WARNINGS 1
#include "Angel.h"
//#include <unistd.h>
#include <iostream>
#include <ctime>
#include "framerate.h"
#include "keyStates.h"
#include "camera.h"
#include "cbspline.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

//---------------------------------------------------------------------------
//-----  Global variables that should be configurable but aren't because I'm in a rush
double rotSpeed = 25;
double camSpeed = 0.15;
double animSpeed = 7.0;
double animClock = 0.0;

const int numCubes = 5;
mat4 cubeTRS[5]; // cube 0 is the 'body' of the robot
mat4 cubeAni[5]; // matrices generated from interpolating the animation paths

const int animCycles = 12;
const int animStepsPerCycle = 4;
vec4 cubeAniPath[5][animStepsPerCycle*animCycles][4]; // for each cube / path segment / arc

//---------------------------------------------------------------------------
//-----  Global objects

Camera cam = Camera(vec3(30.0, 15.0, -10.0), vec3(0.0, 3.0, 0.0));

//---------------------------------------------------------------------------
//-----  Texture mapping

GLuint textures[3];

GLuint loadBMP_custom(unsigned char** data, int* w, int* h, const char * imagepath);

GLfloat texScale = 20;
GLuint TexScale;


//---------------------------------------------------------------------------
//-----  lighting

point4 light_position( 8.0, 18.0, -10.0, 0.0 );

color4 light_ambient( 0.9, 0.9, 0.9, 1.0 );
color4 light_diffuse( 0.9, 0.9, 0.9, 1.0 );
color4 light_specular(0.9, 0.9, 0.9, 1.0);

float  material_shininess = 150.0;

GLuint ka, kd, ks;

//---------------------------------------------------------------------------
//-----  Model transformation parameters

GLuint  model;  // model-view matrix uniform shader variable location

GLuint  view;  // model-view matrix uniform shader variable location

GLuint  projection; // projection matrix uniform shader variable location

GLuint LightPosition;

//---------------------------------------------------------------------------
//-----  Individual sphere model

//#define N 4  // number of subdivisions
//#define M 16*64*3  // number of resulting points

const int NumTimesToSubdivide = 3;
const int NumTriangles        = 256; //(4 faces)^(NumTimesToSubdivide + 1);  5  -> 4096  or 2 -> 64
const int NumSphereVertices         = 3 * NumTriangles;

GLsizei w=512, h=512;


point4 data[NumSphereVertices];

// four equally spaced points on the unit circle

point4 v[4]= {vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 0.942809, -0.333333, 1.0),
    vec4(-0.816497, -0.471405, -0.333333, 1.0),
    vec4(0.816497, -0.471405, -0.333333, 1.0)};


void animateCubes(int howMany, GLfloat howMuch){

	int i,j;
	GLfloat t;
	vec4 v;

	animClock += howMuch;
	while (animClock > animSpeed)
		animClock -= animSpeed;

	// The animation loops through 4 interpolation points every animSpeed seconds
	t = animCycles * animStepsPerCycle * animClock / animSpeed;
	j = (int)floor(t);
	t = fmod(t, 1.0);

	for (i = 0; i < howMany; i++)
	{
		v = cbspline(cubeAniPath[i][j][0], cubeAniPath[i][j][1], cubeAniPath[i][j][2], cubeAniPath[i][j][3], t);
		cubeAni[i] = Translate(v);
	//	std::cerr << j << " " << t << "  " << v << "\n";
	}

}

//--------- Timing and key input --------------------

void frameTick(double tick) {

	if (keyStates::get(033))
		exit(EXIT_SUCCESS);

	if (keyStates::get('['))
		rotSpeed += 5.0;

	if (keyStates::get(']'))
		rotSpeed -= 5.0;

	if (keyStates::get('w'))
		cam.slideFwd(camSpeed);
	if (keyStates::get('s'))
		cam.slideBak(camSpeed);
	if (keyStates::get('a'))
		cam.panLeft(camSpeed);
	if (keyStates::get('d'))
		cam.panRight(camSpeed);

	if (keyStates::get('q'))
	{
		cam.panRight(camSpeed);
		cam.slideLeft(camSpeed);
	}
	if (keyStates::get('e'))
	{
		cam.panLeft(camSpeed);
		cam.slideRight(camSpeed);
	}
	if (keyStates::get('z'))
		cam.panUp(camSpeed);
	if (keyStates::get('x'))
		cam.panDown(camSpeed);
	if (keyStates::get('r'))
		cam.slideUp(camSpeed);
	if (keyStates::get('f'))
		cam.slideDown(camSpeed);


	if (keyStates::get('[') && animSpeed > 0.4)
		animSpeed -= 0.2;
	if (keyStates::get(']'))
		animSpeed += 0.2;

	//std::cout << "gauche: " << cam.gauche() << "\nat: " << cam.at << "\nup: " << cam.up << "\n\n";

	// Model Animation
	animateCubes(5, tick);

	//std::cout << "tick: " << tick << '\n';
	//std::cout << "fr tick: " << framerateControl::tick << '\n';
	glutPostRedisplay();
}


//---------------------------------------------------------------------------
//-----  Individual Cube model

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
vec3   normals[NumVertices];
vec2   tex_coords[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};

// RGBA olors
color4 vertex_colors[8] = {
    color4( 0.3, 0.3, 0.3, 1.0 ),  // black
    color4( 1.0, 0.2, 0.2, 1.0 ),  // red
    color4( 1.0, 1.0, 0.2, 1.0 ),  // yellow
    color4( 0.2, 1.0, 0.2, 1.0 ),  // green
    color4( 0.2, 0.2, 1.0, 1.0 ),  // blue
    color4( 1.0, 0.2, 1.0, 1.0 ),  // magenta
    color4( 1.0, 1.0, 1.0, 1.0 ),  // white
    color4( 0.2, 1.0, 1.0, 1.0 )   // cyan
};


//----------------------------------------------------------------------------
//-----  sphere model functions

vec3   snormals[NumSphereVertices];

int aIndex = 0;

void
triangle( const point4& a, const point4& b, const point4& c )
{
    //vec3  normal = normalize( cross(b - a, c - b) );
    
    snormals[aIndex] = vec3(a.x,a.y,a.z);
    data[aIndex] = a;
//    tex_coords[aIndex] = vec2( 0.0, 1.0 );
    aIndex++;
    snormals[aIndex] = vec3(b.x,b.y,b.z);
    data[aIndex] = b;
//    tex_coords[aIndex] = vec2( 0.0, 0.0 );
    aIndex++;
    snormals[aIndex] = vec3(c.x,c.y,c.z);
    data[aIndex] = c;
//    tex_coords[aIndex] = vec2( 1.0, 0.0 );
    aIndex++;
}


point4
unit( const point4& p )
{
    float len = p.x*p.x + p.y*p.y + p.z*p.z;
    
    point4 t;
    if ( len > DivideByZeroTolerance ) {
        t = p / sqrt(len);
        t.w = 1.0;
    }
    
    return t;
}

void
divide_triangle( const point4& a, const point4& b,
                const point4& c, int count )
{
    if ( count > 0 ) {
        point4 v1 = unit( a + b );
        point4 v2 = unit( a + c );
        point4 v3 = unit( b + c );
        divide_triangle(  a, v1, v2, count - 1 );
        divide_triangle(  c, v2, v3, count - 1 );
        divide_triangle(  b, v3, v1, count - 1 );
        divide_triangle( v1, v3, v2, count - 1 );
    }
    else {
        triangle( a, b, c );
    }
}

void
tetrahedron( int count )
{
    point4 v[4] = {
        vec4( 0.0, 0.0, 1.0, 1.0 ),
        vec4( 0.0, 0.942809, -0.333333, 1.0 ),
        vec4( -0.816497, -0.471405, -0.333333, 1.0 ),
        vec4( 0.816497, -0.471405, -0.333333, 1.0 )
    };
    
    divide_triangle( v[0], v[1], v[2], count );
    divide_triangle( v[3], v[2], v[1], count );
    divide_triangle( v[0], v[3], v[1], count );
    divide_triangle( v[0], v[2], v[3], count );
}


//===========================================================================
// quad
// generates two triangles for each face and assigns colors to the vertices
//===========================================================================
int Index = 0;
void
quad( int a, int b, int c, int d, int col )
{
    // Initialize temporary vectors along the quad's edge to compute its face normal
    vec4 u = vertices[b] - vertices[a];
    vec4 v = vertices[c] - vertices[b];
    
    vec3 normal = normalize( cross(u, v) );
    
    normals[Index] = normal; points[Index] = vertices[a]; tex_coords[Index] = vec2( 0.0, 1.0 ); Index++;
    normals[Index] = normal; points[Index] = vertices[b]; tex_coords[Index] = vec2( 0.0, 0.0 ); Index++;
    normals[Index] = normal; points[Index] = vertices[c]; tex_coords[Index] = vec2( 1.0, 0.0 ); Index++;
    normals[Index] = normal; points[Index] = vertices[a]; tex_coords[Index] = vec2( 0.0, 1.0 ); Index++;
    normals[Index] = normal; points[Index] = vertices[c]; tex_coords[Index] = vec2( 1.0, 0.0 ); Index++;
    normals[Index] = normal; points[Index] = vertices[d]; tex_coords[Index] = vec2( 1.0, 1.0 ); Index++;
//    std::cout << normal << '\n';
//    std::cout << '\n';
}

//===========================================================================
// colorcube
// generate 12 triangles: 36 vertices and 36 colors
//===========================================================================
void
colorcube( int color = 0 )
{
    quad( 1, 0, 3, 2, color );
    quad( 2, 3, 7, 6, color );
    quad( 3, 0, 4, 7, color );
    quad( 6, 5, 1, 2, color );
    quad( 6, 7, 4, 5, color );
    quad( 5, 4, 0, 1, color );
}

GLuint loadBMP_custom(unsigned char** data, int* w, int* h, const char* imagepath)
{
    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-byte header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height; //, hppm, vppm;
    unsigned int imageSize;   // = width*height*3
    // Actual RGB data
    
    // Open the file
    FILE * file = fopen(imagepath,"rb");
    if (!file)
    {printf("Image could not be opened\n"); return 0;}
    
    if ( fread(header, 1, 54, file)!=54 )
    {
        printf("Not a correct BMP file\n");
        return 1;
    }
    
    if ( header[0]!='B' || header[1]!='M' )
    {
        printf("Not a correct BMP file\n");
        return 1;
    }
    
    // Read ints from the byte array
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = abs(*(int*)&(header[0x16]));
    
    *w = width;
    *h = height;
    
    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)      dataPos=54; // The BMP header is done that way
    
    // Create a buffer
    *data = new unsigned char [imageSize];
    
    // Read the actual data from the file into the buffer
    fread(*data,1,imageSize,file);
    
    //Everything is in memory now, the file can be closed
    fclose(file);
    
    return 0;
}

//===========================================================================
// init
//
// OpenGL initialization
//===========================================================================
GLuint vPosition;
GLuint vNormal;
GLuint vTexCoord;
GLuint program;

void
init()
{
    //----- generate vertex data for the basic cube model
    
    colorcube(3);

	int i, j, k;
	GLfloat cscale = 0.3, dscale = 0.65, ascale = 0.05;

	cubeTRS[0] = Translate(-29.0, 5.0, -10.0) * RotateY(-30) * Scale(10.0, 10.0, 10.0);
	cubeTRS[1] = Translate(-dscale + 0.25, -0.35, -dscale) * Scale(cscale, cscale, cscale);
	cubeTRS[2] = Translate(-dscale + 0.25, -0.35, dscale) * Scale(cscale, cscale, cscale);
	cubeTRS[4] = Translate(dscale - 0.25, -0.35, dscale) * Scale(cscale, cscale, cscale);
	cubeTRS[3] = Translate(dscale - 0.25, -0.35, -dscale) * Scale(cscale, cscale, cscale);

	for (i = 0; i < 5; i++)
		cubeAni[i] = mat4(1.0);

	// init the robot body anim
	for (i = 0; i < animCycles; i++)
	{
		for (j = 0; j < animStepsPerCycle; j++)
		{
			for (k = 0; k < 4; k++)
			{
				cubeAniPath[0][i*animStepsPerCycle + j][k] = vec4((12 * i + 3 * j + k) * ascale, 0.0, 0.0, 0.0);
			}
		}
	}
	for (k = 1; k < numCubes; k++)
	{
		for (i = 0; i < animCycles; i++)
		{
			j = i*animStepsPerCycle + (k % 4);
			cubeAniPath[k][j + 0][0] = vec4(-1.5 * ascale / cscale, 0.0, 0.0, 0.0);
			cubeAniPath[k][j + 0][1] = vec4(-2.5 * ascale / cscale, 0.0, 0.0, 0.0);
			cubeAniPath[k][j + 0][2] = vec4(-3.5 * ascale / cscale, 0.0, 0.0, 0.0);
			cubeAniPath[k][j + 0][3] = vec4(-4.5 * ascale / cscale, 0.0, 0.0, 0.0);

			cubeAniPath[k][j + 1][0] = vec4(-4.5 * ascale / cscale, 0.0, 0.0, 0.0);
			cubeAniPath[k][j + 1][1] = vec4(-4.5 * ascale / cscale, 3 * ascale / cscale, 0.0, 0.0);
			cubeAniPath[k][j + 1][2] = vec4(4.5 * ascale / cscale, 3 * ascale / cscale, 0.0, 0.0);
			cubeAniPath[k][j + 1][3] = vec4(4.5 * ascale / cscale, 0.0, 0.0, 0.0);

			cubeAniPath[k][j + 2][0] = vec4(4.5 * ascale / cscale, 0.0, 0.0, 0.0);
			cubeAniPath[k][j + 2][1] = vec4(3.5 * ascale / cscale, 0.0, 0.0, 0.0);
			cubeAniPath[k][j + 2][2] = vec4(2.5 * ascale / cscale, 0.0, 0.0, 0.0);
			cubeAniPath[k][j + 2][3] = vec4(1.5 * ascale / cscale, 0.0, 0.0, 0.0);

			cubeAniPath[k][j + 3][0] = vec4(1.5 * ascale / cscale, 0.0, 0.0, 0.0);
			cubeAniPath[k][j + 3][1] = vec4(0.5 * ascale / cscale, 0.0, 0.0, 0.0);
			cubeAniPath[k][j + 3][2] = vec4(-0.5 * ascale / cscale, 0.0, 0.0, 0.0);
			cubeAniPath[k][j + 3][3] = vec4(-1.5 * ascale / cscale, 0.0, 0.0, 0.0);
		}
	}

    //----- generate vertex data for the basic sphere model
    
    tetrahedron(NumTimesToSubdivide);
    
    //----- load shaders
    
    program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    glUseProgram( program );
    
    
    //----- setup OpenGL plumbing to transfer vertex attribute data to shaders
    
    // Initialize texture objects
    glGenTextures( 3, textures );
    
    unsigned char* pic1 = NULL;
    int w,h;
    loadBMP_custom(&pic1, &w, &h, "sky.bmp");
    
    glBindTexture( GL_TEXTURE_2D, textures[0] );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic1);
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    
    unsigned char* pic2 = NULL;
    loadBMP_custom(&pic2, &w, &h, "companion cube.bmp");

    
    glBindTexture( GL_TEXTURE_2D, textures[1] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic2 );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    

	unsigned char* pic3 = NULL;
	loadBMP_custom(&pic3, &w, &h, "tessground.bmp");
	//	std::cerr << pic1 << "\t" << pic3 << "\n";

    glBindTexture( GL_TEXTURE_2D, textures[2] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic3 );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, textures[0] );
    
    // Create a vertex array object
    GLuint vao;
//    glGenVertexArraysAPPLE( 1, &vao );
//	glBindVertexArrayAPPLE(vao);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
    
    // Create buffer object and transfer vertex attribute data
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals) + sizeof(tex_coords) + sizeof(data) + sizeof(snormals), NULL, GL_STATIC_DRAW );
    // transfer the points
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    // transfer the normals
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), normals );
    // transfer the tex_coords
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals), sizeof(tex_coords), tex_coords );
    // transfer the sphere points
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals) + sizeof(tex_coords), sizeof(data), data );
    // transfer the sphere normals
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals) + sizeof(tex_coords) + sizeof(data), sizeof(snormals), snormals );
    
    // link with shader variables
    vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    
    vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)) );
    
    GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" );
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(normals)) );
    
    // Set the value of the fragment shader texture sampler variable (texture) to GL_TEXTURE0
    glUniform1i( glGetUniformLocation(program, "texture"), 0 );
    
    //-----  setup uniform variables for lighting
    
    ka = glGetUniformLocation(program, "ka");
    kd = glGetUniformLocation(program, "kd");
    ks = glGetUniformLocation(program, "ks");
    
    glUniform4fv( glGetUniformLocation(program, "AmbientLight"), 1, light_ambient );
    glUniform4fv( glGetUniformLocation(program, "DiffuseLight"), 1, light_diffuse );
    glUniform4fv( glGetUniformLocation(program, "SpecularLight"), 1, light_specular );
	
	LightPosition = glGetUniformLocation(program, "LightPosition");
    
    glUniform1f( glGetUniformLocation(program, "Shininess"), material_shininess );
    
    //-----  setup uniform variables for transformations
    
    model = glGetUniformLocation( program, "model" );
    view = glGetUniformLocation( program, "view" );
    projection = glGetUniformLocation( program, "projection" );
    TexScale = glGetUniformLocation(program, "texScale");
    //std::cout << TexScale;
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.4, 0.5, 0.7, 1.0 );
}


//===========================================================================
// display
//
// draw the world
//===========================================================================
void
display( void )
{
	int i;
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    
    glUniform1f( TexScale, 1.0 );
    
    //----- view
    mat4  v = cam.look();
    glUniformMatrix4fv( view, 1, GL_TRUE, v );
    
    //----- projection
    mat4  p = cam.frustum();
    glUniformMatrix4fv( projection, 1, GL_TRUE, p );

	//------ light
	vec4 lpos = vec4(cam.eye + cam.up,0.0);
	glUniform4fv(LightPosition, 1, lpos);
    
    //----- draw the sun
    glBindTexture( GL_TEXTURE_2D, textures[1] );
    
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)+sizeof(normals)+ sizeof(tex_coords) ));
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)+sizeof(normals)+ sizeof(tex_coords) +sizeof(data)));
    mat4 trans = Translate( -5.0, 15.5, -5.0);
    glUniformMatrix4fv( model, 1, GL_TRUE, trans );
    glUniform4fv( ka, 1, vec4(0.6, 0.4, 0.2, 1.0) );
    glUniform4fv( kd, 1, vec4(0.6, 0.4, 0.2, 1.0) );
    glUniform4fv( ks, 1, vec4(0.0, 0.0, 0.0, 1.0) );
    glDrawArrays( GL_TRIANGLES, 0, NumSphereVertices );
    
    //----- draw the ground
    glUniform1f( TexScale, texScale );
    glBindTexture( GL_TEXTURE_2D, textures[2] );
    
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)) );
    mat4 trs = Scale(100.0, 0.1, 100.0);
    glUniformMatrix4fv( model, 1, GL_TRUE, trs );
    glUniform4fv( ka, 1, vec4(0.1, 0.1, 0.1, 1.0) );
    glUniform4fv( kd, 1, vec4(0.8, 0.8, 0.8, 1.0) );
    glUniform4fv( ks, 1, vec4(0.1, 0.1, 0.1, 1.0) );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
    
    //----- draw the sky
    glUniform1f( TexScale, 1.0 );
    glBindTexture( GL_TEXTURE_2D, textures[0] );
    
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)) );
    trs = Translate(0.0, 40.0, -40) * Scale(100.0, 100.0, 0.1);
    glUniformMatrix4fv( model, 1, GL_TRUE, trs );
	glUniform4fv(ka, 1, vec4(0.1, 0.1, 0.1, 1.0));
    glUniform4fv( kd, 1, vec4(0.9, 0.9, 0.9, 1.0) );
    glUniform4fv( ks, 1, vec4(0.0, 0.0, 0.0, 1.0) );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

	// again
	trs = Translate(0.0, 40.0, 40) * Scale(100.0, 100.0, 0.1);
	glUniformMatrix4fv(model, 1, GL_TRUE, trs);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// and again and again
	trs = Translate(40.0, 40.0, 0.0) * Scale(0.1, 100.0, 100.0);
	glUniformMatrix4fv(model, 1, GL_TRUE, trs);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	trs = Translate(-40.0, 40.0, 0.0) * Scale(0.1, 100.0, 100.0);
	glUniformMatrix4fv(model, 1, GL_TRUE, trs);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	//----- draw something interesting
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	glUniform4fv(ka, 1, vec4(0.2, 0.2, 0.2, 1.0));
	glUniform4fv(kd, 1, vec4(0.8, 0.8, 0.8, 1.0));
	glUniform4fv(ks, 1, vec4(0.8, 0.8, 0.8, 1.0));

	for (i = 0; i < 5; i++)
	{
		if (i == 0)
			trs = cubeTRS[i] * cubeAni[i];
		else
			trs = cubeTRS[0] * cubeAni[0] * cubeTRS[i] * cubeAni[i];
		glUniformMatrix4fv(model, 1, GL_TRUE, trs);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}
    
    glutSwapBuffers();
}


//===========================================================================
// reshape
// callback for window resize
//===========================================================================
void
reshape( int width, int height )
{
	glViewport(0, 0, width, height);
	cam.aspect(width, height);
}


//===========================================================================
// main
//
//===========================================================================
int
main( int argc, char **argv )
{
	glutInitContextVersion(3, 0);
    glutInit( &argc, argv );
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize( 1008, 623 );
    glutCreateWindow( "Project" );

	glewExperimental = GLU_TRUE;
	glewInit();
    
    init();
    
    glutDisplayFunc( display );
	glutReshapeFunc(reshape);

	glutKeyboardFunc(keyStates::down);
	glutKeyboardUpFunc(keyStates::up);
	glutIdleFunc(framerateControl::idle);

	framerateControl::init(32, frameTick);
    
    glutMainLoop();
    return 0;
}
#endif


