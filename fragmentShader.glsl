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

// Material's properties
in vec4 frontAmbDiffExport, frontSpecExport, backAmbDiffExport, backSpecExport;
// texture coordinates
in vec2 texCoordsExport;

// samplers for the textures
uniform sampler2D bordob;
uniform sampler2D baseb;
uniform sampler2D bordoa;
uniform sampler2D basea;
uniform sampler2D prato;
uniform sampler2D copp;
uniform sampler2D bordotb;
uniform sampler2D coptb;
uniform sampler2D murod;
uniform sampler2D murol;
uniform sampler2D porta;
uniform sampler2D bordotm;
uniform sampler2D bordota;
uniform sampler2D bordotf;
uniform sampler2D bordots;
uniform sampler2D colonna;
uniform sampler2D toro;
uniform sampler2D quadrou;
uniform sampler2D quadrod;
uniform sampler2D dipintou;
uniform sampler2D dipintod;
uniform sampler2D dipintot;

uniform uint object;

out vec4 colorsOut;

vec4 texColor;

void main(void)
{
    // check which object we are currently drawing
    if (object == BASEB) texColor = texture(bordob, texCoordsExport);
    if (object == COPB) texColor = texture(baseb, texCoordsExport);
    if (object == BASEA) texColor = texture(bordoa, texCoordsExport);
    if (object == COPA) texColor = texture(basea, texCoordsExport);
    if (object == PRATO) texColor = texture(prato, texCoordsExport);
    if (object == COPP) texColor = texture(copp, texCoordsExport);
    if (object == TETTOB) texColor = texture(bordotb, texCoordsExport);
    if (object == COPTB) texColor = texture(coptb, texCoordsExport);
    if (object == MUROD) texColor = texture(murod, texCoordsExport);
    if (object == MUROL) texColor = texture(murol, texCoordsExport);
    if (object == PORTA) texColor = texture(porta, texCoordsExport);
    if (object == TETTOM) texColor = texture(bordotm, texCoordsExport);
    if (object == TETTOA) texColor = texture(bordota, texCoordsExport);
    if (object == TETTOF) texColor = texture(bordotf, texCoordsExport);
    if (object == TETTOS) texColor = texture(bordots, texCoordsExport);
    if (object == CYLINDER) texColor = texture(colonna, texCoordsExport);
    if (object == TOR) texColor = texture(toro, texCoordsExport);
    if (object == QUADROU) texColor = texture(quadrou, texCoordsExport);
    if (object == QUADROD) texColor = texture(quadrod, texCoordsExport);
    if (object == DIPINTOU) texColor = texture(dipintou, texCoordsExport);
    if (object == DIPINTOD) texColor = texture(dipintod, texCoordsExport);
    if (object == DIPINTOT) texColor = texture(dipintot, texCoordsExport);
    
    // set color of the fragment
    colorsOut = gl_FrontFacing? (frontAmbDiffExport * texColor + frontSpecExport) :
                               (backAmbDiffExport * texColor + backSpecExport);
}
