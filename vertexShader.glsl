#version 430 core
// Uno per ogni oggetto nella scena
#define BASEB 0
#define COPB 1
#define BASEA 2
#define COPA 3
#define PRATO 4
#define COPP 5
#define TETTOB 6
#define COPTB 7
#define MUROD 8
#define MUROL 9
#define PORTA 10
#define TETTOM 11
#define TETTOA 12
#define TETTOF 13
#define TETTOS 14
#define CYLINDER 15
#define TOR 16
#define QUADROU 17
#define QUADROD 18
#define DIPINTOU 19
#define DIPINTOD 20
#define DIPINTOT 21


layout(location=0) in vec4 Coords;
layout(location=1) in vec3 Normal;
layout(location=2) in vec2 TexCoords;


uniform mat4 modelViewMat;
uniform mat4 projMat;
uniform mat3 normalMat;
uniform uint object;

out vec4 frontAmbDiffExport, frontSpecExport, backAmbDiffExport, backSpecExport;
out vec2 texCoordsExport;

struct Light
{
    vec4 ambCols;
    vec4 difCols;
    vec4 specCols;
    vec4 coords;
};
uniform Light light0;

uniform vec4 globAmb;

struct Material
{
    vec4 ambRefl;
    vec4 difRefl;
    vec4 specRefl;
    vec4 emitCols;
    float shininess;
};
uniform Material canFandB;

vec3 normal, lightDirection, eyeDirection, halfway;
vec4 frontEmit, frontGlobAmb, frontAmb, frontDif, frontSpec,
     backEmit, backGlobAmb, backAmb, backDif, backSpec;
vec4 coords;

void main(void)
{
    // Here we select which object we are plotting

    if (object == BASEB) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == COPB) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == BASEA) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == COPA) {
    	coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
     if (object == PRATO) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == COPP) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
     if (object == TETTOB) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == COPTB) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == MUROD) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    } 
    if (object == MUROL) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    } 
    if (object == PORTA) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    } 
    if (object == TETTOM) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == TETTOA) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    } 
    if (object == TETTOF) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == TETTOS) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == CYLINDER) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == TOR) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == QUADROU) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == QUADROD) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == DIPINTOU) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == DIPINTOD) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    if (object == DIPINTOT) {
        coords = Coords;
        normal = Normal;
        texCoordsExport = TexCoords;
    }
    // object normal
    normal = normalize(normalMat * normal);
    // direction of the light
    lightDirection = normalize(vec3(light0.coords));
    // view direction
    eyeDirection = -1.0f * normalize(vec3(modelViewMat * coords));
    halfway = (length(lightDirection + eyeDirection) == 0.0f) ?
              vec3(0.0) : (lightDirection + eyeDirection)/
              length(lightDirection + eyeDirection);

    // PHONG Model - FRONT FACE
    // That is, emitted light and global, diffuse and specular
    // components.
    frontEmit = canFandB.emitCols;
    frontGlobAmb = globAmb * canFandB.ambRefl;
    frontAmb = light0.ambCols * canFandB.ambRefl;
    frontDif = max(dot(normal, lightDirection), 0.0f) *
        (light0.difCols * canFandB.difRefl);
    frontSpec = pow(max(dot(normal, halfway), 0.0f),
                    canFandB.shininess) * (light0.specCols * canFandB.specRefl);
    frontAmbDiffExport =  vec4(vec3(min(frontEmit + frontGlobAmb +
        frontAmb + frontDif, vec4(1.0))), 1.0);
    frontSpecExport =  vec4(vec3(min(frontSpec, vec4(1.0))), 1.0);

    // PHONG Model - BACK FACE
    // That is, emitted light and global, diffuse and specular
    // components.
    normal = -1.0f * normal;
    backEmit = canFandB.emitCols;
    backGlobAmb = globAmb * canFandB.ambRefl;
    backAmb = light0.ambCols * canFandB.ambRefl;
    backDif = max(dot(normal, lightDirection), 0.0f) *
        (light0.difCols * canFandB.difRefl);
    backSpec = pow(max(dot(normal, halfway), 0.0f),
        canFandB.shininess) * (light0.specCols * canFandB.specRefl);
    backAmbDiffExport =  vec4(vec3(min(backEmit + backGlobAmb + backAmb +
        backDif, vec4(1.0))), 1.0);
    backSpecExport =  vec4(vec3(min(backSpec, vec4(1.0))), 1.0);

    gl_Position = projMat * modelViewMat * coords;
}
