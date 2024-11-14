/*
 * Progetto: Modellazione di un templio greco con la possibilità di muoversi nella scena e di spostare la visuale rispetto l'asse y.
 * Emiliano Carulli
 * Class: Computer Graphics
 * Si usano i tasti :
 * 8 su
 * 2 giù
 * 4 destra
 * 6 sinisra
 * 9 andare avanti
 * 1 andare indietro
  'Y' e 'y' per spostare la visuale
 * tutti i valori dei materiali sono stati presi nei due seguenti siti :
 * 2 http://devernay.free.fr/cours/opengl/materials.html
 * http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
 */
 /*
 * Nota: si è scelto di utillizare un approccio di tipo gerarchico per strutturare la scena,
 *       si è fatta la scelta di limitare lo spostamento in modo tale da non
 *       far sparire mai il templio dalla scena completamente.
 */

// standard includes
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cglm/cglm.h>
#include <cglm/types-struct.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "shader.h"
#include "cylinder.h"// per creare le colonne
#include "Square.h"// per creare il dipinto
#include "torus.h"// per creare il capitello
#include "light.h"// struttura per implementare la luce
#include "material.h"//struttura per implementare il materiale
#include "readBMP.h"

#define CV 16//numero vertici del cubo senza faccia superiore e inferiore
#define QV 4//numero vertici quadrato, spesso usato per completare le facce mancanti del cubo e per fare i quadri

static enum object {BASEB,COPB, BASEA, COPA, PRATO, COPP, TETTOB, COPTB, MUROD, MUROL,PORTA, TETTOM,
                    TETTOA, TETTOF, TETTOS, CYLINDER, TOR, QUADROU, QUADROD, DIPINTOU, DIPINTOD, DIPINTOT}; // VAO uno per oggetto nella scena ids.

static enum buffer {BASEB_VERTICES, COPB_VERTICES, BASEA_VERTICES, COPA_VERTICES, PRATO_VERTICES,
                    COPP_VERTICES, TETTOB_VERTICES, COPTB_VERTICES, MUROD_VERTICES, MUROL_VERTICES,
                    PORTA_VERTICES,TETTOM_VERTICES, TETTOA_VERTICES, TETTOF_VERTICES, TETTOS_VERTICES,
                    CYL_VERTICES, CYL_INDICES, TOR_VERTICES, TOR_INDICES, QUADROU_VERTICES, QUADROD_VERTICES,
                    DIPINTOU_VERTICES,DIPINTOD_VERTICES,DIPINTOT_VERTICES
                    };// VB0 uno per oggetto nella scena

// Posizione iniziale nel mio punto di vista
static float Xdirezione = 0.0, Ydirezione = -5.0, Zdirezione = 35.0;
// Ruotazione iniziale rispetto l'asse y
static float Yangle = 0;
// Light properties.
static const Light light0 =
{
    (vec4){0.0, 0.0, 0.0, 1.0},
    (vec4){1.0, 1.0, 1.0, 1.0},
    (vec4){1.0, 1.0, 1.0, 1.0},
    (vec4){10.0, 15.0, 10.0, 0.0}//Posizione luce
};
// Global ambient.
static const vec4 globAmb = (vec4)
{
    0.2, 0.2, 0.2, 1.0
};
//Proprietà Materiale di:
// BASE
static const Material base =
{
    (vec4){0.25, 0.25, 0.25, 1.0},
    (vec4){0.55, 0.55, 0.55, 1.0},
    (vec4){0.70, 0.70, 0.70, 1.0},
    (vec4){0.0, 0.0, 0.0, 1.0},
    32.0f
};
// PRATO
static const Material prato =
{
    (vec4){0.0, 0.0, 0.0, 1.0},
    (vec4){0.1, 0.35, 0.1, 1.0},
    (vec4){0.45, 0.55, 0.45, 1.0},
    (vec4){0.0, 0.0, 0.0, 1.0},
    32.0f
};
// BASE TETTO
static const Material suptetto =
{
    (vec4){0.05, 0.05, 0.05, 1.0},
    (vec4){0.5, 0.5, 0.5, 1.0},
    (vec4){0.7, 0.7, 0.7, 1.0},
    (vec4){0.0, 0.0, 0.0, 1.0},
    10.0f
};
// TETTO
static const Material tetto =
{
    (vec4){0.0, 0.0, 0.0, 1.0},
    (vec4){0.5, 0.2, 0.0, 1.0},
    (vec4){0.7, 0.6, 0.6, 1.0},
    (vec4){0.0, 0.0, 0.0, 1.0},
    15.0f
};
//CAPITELLO
static const Material capitello =
{
    (vec4){0.223, 0.088, 0.027, 1.0},
    (vec4){0.751, 0.606, 0.226, 1.0},
    (vec4){0.628, 0.55, 0.366, 1.0},
    (vec4){0.0, 0.0, 0.0, 1.0},
    51.0f
};
//COLONNA
static const Material colonna =
{
    (vec4){0.174, 0.011, 0.011, 1.0},
    (vec4){0.614, 0.041, 0.041, 1.0},
    (vec4){0.727, 0.626, 0.626, 1.0},
    (vec4){0.0, 0.0, 0.0, 1.0},
    76.8f
};
//FRONTE
static const Material fronte =
{
    (vec4){0.25, 0.20, 0.20, 1.0},
    (vec4){1.0, 0.89, 0.89, 0.92},
    (vec4){0.29, 0.29, 0.29, 1.0},
    (vec4){0.0, 0.0, 0.0, 1.0},
    11.0f
};
//Variabili necessarie per implementare.
// COLONNA.
static Vertex cylVertices[(CYL_LONGS + 1) * (CYL_LATS + 1)];
static unsigned int cylIndices[CYL_LATS][2*(CYL_LONGS+1)];
static int cylCounts[CYL_LATS];
static void* cylOffsets[CYL_LATS];
// CAPITELLO
static Vertex torVertices[(TOR_LONGS + 1) * (TOR_LATS + 1)];
static unsigned int torIndices[TOR_LATS][2*(TOR_LONGS+1)];
static int torCounts[TOR_LATS];
void* torOffsets[TOR_LATS];

//VERTICI DEGLI OGGETTI MESSI A VIDEO: coordinate,normale,coordinate di texture

//BASE BASSA:
static Vertex BasebVertices[CV]=
{
//FRONTE
    (vec4){-13.0, 0.5, 19.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 0.0},
    (vec4){-13.0, -0.5, 19.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 1.0},
    (vec4){13.0, 0.5, 19.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){4.0, 0.0},
    (vec4){13.0, -0.5, 19.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){4.0, 1.0},
//LATERALE SINISTRO
    (vec4){13.0, 0.5, -19.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){13.0, -0.5, -19.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 1.0},
//DIETRO
    (vec4){-13.0, 0.5, -19.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){4.0, 0.0},
    (vec4){-13.0, -0.5, -19.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){4.0, 1.0},
//LATERALE DESTRO
    (vec4){-13.0, 0.5, 19.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-13.0, -0.5, 19.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){.0, 1.0}
};
//COPERCHIO BASE BASSA
static Vertex CopbVertices[QV]=
{
//FRONTE
    (vec4){-13.0, -0.5, 19.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-13.0, -0.5, -19.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){0.0, 2.0},
    (vec4){13.0, -0.5, 19.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){2.0, 0.0},
    (vec4){13.0, -0.5, -19.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){2.0, 2.0}
};
//BASE ALTA
static Vertex BaseaVertices[CV]=
{
//FRONTE
    (vec4){-14.0, 0.5, 20.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 0.0},
    (vec4){-14.0, -0.5, 20.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 1.0},
    (vec4){14.0, 0.5, 20.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){6.0, 0.0},
    (vec4){14.0, -0.5, 20.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){6.0, 1.0},
//LATERALE SINISTRO
    (vec4){14.0, 0.5, -20.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){14.0, -0.5, -20.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 1.0},
//DIETRO
    (vec4){-14.0, 0.5, -20.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){6.0, 0.0},
    (vec4){-14.0, -0.5, -20.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){6.0, 1.0},
//LATERALE DESTRO
    (vec4){-14.0, 0.5, 20.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-14.0, -0.5, 20.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){.0, 1.0}
};
//COPERCHIO BASE ALTA
static Vertex CopaVertices[QV]=
{
//FRONTE
    (vec4){-14.0, -0.5, 20.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-14.0, -0.5, -20.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){0.0, 2.0},
    (vec4){14.0, -0.5, 20.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){2.0, 0.0},
    (vec4){14.0, -0.5, -20.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){2.0, 2.0}
};
//PRATO
static Vertex PratoVertices[CV]=
{
//FRONTE
    (vec4){-25.0, 3.5, 25.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 0.0},
    (vec4){-25.0, -3.5, 25.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 1.0},
    (vec4){25.0, 3.5, 25.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){10.0, 0.0},
    (vec4){25.0, -3.5, 25.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){10.0, 1.0},
//LATERALE SINISTRO
    (vec4){25.0, 3.5, -25.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){25.0, -3.5, -25.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 1.0},
//DIETRO
    (vec4){-25.0, 3.5, -25.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){10.0, 0.0},
    (vec4){-25.0, -3.5, -25.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){10.0, 1.0},
//LATERALE DESTRO
    (vec4){-25.0, 3.5, 25.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-25.0, -3.5, 25.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){.0, 1.0}
};
//COPERCHIO PRATO
static Vertex CoppVertices[QV]=
{
//FRONTE
    (vec4){-25.0, -3.5, 25.0, 1.0}, (vec3){0.0, 1.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-25.0, -3.5, -25.0, 1.0}, (vec3){0.0, 1.0, 0.0}, (vec2){0.0, 1.0},
    (vec4){25.0, -3.5, 25.0, 1.0}, (vec3){0.0, 1.0, 0.0}, (vec2){1.0, 0.0},
    (vec4){25.0, -3.5, -25.0, 1.0}, (vec3){0.0, 1.0, 0.0}, (vec2){1.0, 1.0}
};
//TETTO BASSO
static Vertex TettobVertices[CV]=
{
//FRONTE
    (vec4){-12.0, 0.5, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 0.0},
    (vec4){-12.0, -0.5, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 1.0},
    (vec4){12.0, 0.5, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){8.0, 0.0},
    (vec4){12.0, -0.5, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){8.0, 1.0},
//LATERALE SINISTRO
    (vec4){12.0, 0.5, -18.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){12.0, -0.5, -18.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 1.0},
//DIETRO
    (vec4){-12.0, 0.5, -18.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){8.0, 0.0},
    (vec4){-12.0, -0.5, -18.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){8.0, 1.0},
//LATERALE DESTRO
    (vec4){-12.0, 0.5, 18.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-12.0, -0.5, 18.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 1.0}
};
//COPERCHIO TETTO BASSO
static Vertex CoptbVertices[QV]=
{
//FRONTE
    (vec4){-12.0, 0.5, 18.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-12.0, 0.5, -18.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){0.0, 1.0},
    (vec4){12.0, 0.5, 18.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){1.0, 0.0},
    (vec4){12.0, 0.5, -18.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){1.0, 1.0}
};
//MURO DIETRO
static Vertex MurodVertices[CV]=
{
//FRONTE
    (vec4){-9.5, 4.0, 1.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 0.0},
    (vec4){-9.5, -4.0, 1.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 1.0},
    (vec4){9.5, 4.0, 1.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){1.0, 0.0},
    (vec4){9.5, -4.0, 1.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){1.0, 1.0},
//LATERALE SINISTRO
    (vec4){9.5, 4.0, -1.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){9.5, -4.0, -1.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 1.0},
//DIETRO
    (vec4){-9.5, 4.0, -1.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){1.0, 0.0},
    (vec4){-9.5, -4.0, -1.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){1.0, 1.0},
//LATERALE DESTRO
    (vec4){-9.5, 4.0, 1.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-9.5, -4.0, 1.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 1.0}
};
//MURO LATERALE
static Vertex MurolVertices[CV]=
{
//FRONTE
    (vec4){-0.5, 4.0, 15.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 0.0},
    (vec4){-0.5, -4.0, 15.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 1.0},
    (vec4){0.5, 4.0, 15.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){1.0, 0.0},
    (vec4){0.5, -4.0, 15.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){1.0, 1.0},
//LATERALE SINISTRO
    (vec4){0.5, 4.0, -15.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){0.5, -4.0, -15.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 1.0},
//DIETRO
    (vec4){-0.5, 4.0, -15.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){1.0, 0.0},
    (vec4){-0.5, -4.0, -15.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){1.0, 1.0},
//LATERALE DESTRO
    (vec4){-0.5, 4.0, 15.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-0.5, -4.0, 15.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 1.0}
};
//PORTA
static Vertex portaVertices[CV]=
{
//FRONTE
    (vec4){-4.0, 4.0, 1.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 0.0},
    (vec4){-4.0, -4.0, 1.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 1.0},
    (vec4){4.0, 4.0, 1.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){1.0, 0.0},
    (vec4){4.0, -4.0, 1.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){1.0, 1.0},
//LATERALE SINISTRO
    (vec4){4.0, 4.0, -1.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){4.0, -4.0, -1.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 1.0},
//DIETRO
    (vec4){-4.0, 4.0, -1.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){1.0, 0.0},
    (vec4){-4.0, -4.0, -1.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){1.0, 1.0},
//LATERALE DESTRO
    (vec4){-4.0, 4.0, 1.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-4.0, -4.0, 1.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 1.0}
};
//TETTO MEDIO
static Vertex TettomVertices[CV]=
{
//FRONTE
    (vec4){-12.0, 1.0, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 0.0},
    (vec4){-12.0, -1.0, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 1.0},
    (vec4){12.0, 1.0, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){1.0, 0.0},
    (vec4){12.0, -1.0, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){1.0, 1.0},
//LATERALE SINISTRO
    (vec4){12.0, 1.0, -18.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){12.0, -1.0, -18.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 1.0},
//DIETRO
    (vec4){-12.0, 1.0, -18.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){1.0, 0.0},
    (vec4){-12.0, -1.0, -18.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){1.0, 1.0},
//LATERALE DESTRO
    (vec4){-12.0, 1.0, 18.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-12.0, -1.0, 18.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 1.0}
};
//TETTO ALTO
static Vertex TettoaVertices[CV]=
{
//FRONTE
    (vec4){-12.0, 0.5, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 0.0},
    (vec4){-12.0, -0.5, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 1.0},
    (vec4){12.0, 0.5, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){4.0, 0.0},
    (vec4){12.0, -0.5, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){4.0, 1.0},
//LATERALE SINISTRO
    (vec4){12.0, 0.5, -18.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){12.0, -0.5, -18.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 1.0},
//DIETRO
    (vec4){-12.0, 0.5, -18.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){4.0, 0.0},
    (vec4){-12.0, -0.5, -18.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){4.0, 1.0},
//LATERALE DESTRO
    (vec4){-12.0, 0.5, 18.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-12.0, -0.5, 18.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 1.0}
};
// FRONTE TETTO
static Vertex TettofVertices[QV]=
{
//FRONTE
    (vec4){-12.0, 0.5, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 0.0},
    (vec4){0.0, -3.5, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 1.0},
    (vec4){12.0, 0.5, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.85, 0.0},
};
//SUPERFIE TETTO, TEGOLE
static Vertex TettosVertices[QV]=
{
 (vec4){6.5, -3.0, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 0.0},
 (vec4){-6.5, -3.0, 18.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 1.0},
 (vec4){6.5, -3.0, -18.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){1.0, 0.0},
 (vec4){-6.5, -3.0, -18.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){1.0, 1.0},
};
//  QUADRO1:
static Vertex QuadrouVertices[CV]=
{
//FRONTE
    (vec4){-3.0, 0.5, 5.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 0.0},
    (vec4){-3.0, -0.5, 5.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 1.0},
    (vec4){3.0, 0.5, 5.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){4.0, 0.0},
    (vec4){3.0, -0.5, 5.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){4.0, 1.0},
//LATERALE SINISTRO
    (vec4){3.0, 0.5, -5.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){3.0, -0.5, -5.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 1.0},
//DIETRO
    (vec4){-3.0, 0.5, -5.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){4.0, 0.0},
    (vec4){-3.0, -0.5, -5.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){4.0, 1.0},
//LATERALE DESTRO
    (vec4){-3.0, 0.5, 5.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-3.0, -0.5, 5.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){.0, 1.0}
};
//QUADRO2:
static Vertex QuadrodVertices[CV]=
{
//FRONTE
    (vec4){-3.0, 0.5, 3.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 0.0},
    (vec4){-3.0, -0.5, 3.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){0.0, 1.0},
    (vec4){3.0, 0.5, 3.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){4.0, 0.0},
    (vec4){3.0, -0.5, 3.0, 1.0}, (vec3){0.0, 0.0, 1.0}, (vec2){4.0, 1.0},
//LATERALE SINISTRO
    (vec4){3.0, 0.5, -3.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){3.0, -0.5, -3.0, 1.0}, (vec3){1.0, 0.0, 0.0}, (vec2){0.0, 1.0},
//DIETRO
    (vec4){-3.0, 0.5, -3.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){4.0, 0.0},
    (vec4){-3.0, -0.5, -3.0, 1.0}, (vec3){0.0, 0.0, -1.0}, (vec2){4.0, 1.0},
//LATERALE DESTRO
    (vec4){-3.0, 0.5, 3.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-3.0, -0.5, 3.0, 1.0}, (vec3){-1.0, 0.0, 0.0}, (vec2){.0, 1.0}
};
//DIPINTO1:
static Vertex DipintouVertices[QV]=
{
//FRONTE
    (vec4){-3.0, -0.5, 5.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-3.0, -0.5, -5.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){0.0, 1.0},
    (vec4){3.0, -0.5, 5.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){1.0, 0.0},
    (vec4){3.0, -0.5, -5.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){1.0, 1.0}
};
//DIPINTO2:
static Vertex DipintodVertices[QV]=
{
//FRONTE
    (vec4){-3.0, -0.5, 5.0, 1.0}, (vec3){0.0, 1.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-3.0, -0.5, -5.0, 1.0}, (vec3){0.0, 1.0, 0.0}, (vec2){0.0, 1.0},
    (vec4){3.0, -0.5, 5.0, 1.0}, (vec3){0.0, 1.0, 0.0}, (vec2){1.0, 0.0},
    (vec4){3.0, -0.5, -5.0, 1.0}, (vec3){0.0, 1.0, 0.0}, (vec2){1.0, 1.0}
};
//DIPINTO3:
static Vertex DipintotVertices[QV]=
{
//FRONTE
    (vec4){-3.0, -0.5, 3.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){0.0, 0.0},
    (vec4){-3.0, -0.5, -3.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){0.0, 1.0},
    (vec4){3.0, -0.5, 3.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){1.0, 0.0},
    (vec4){3.0, -0.5, -3.0, 1.0}, (vec3){0.0, -1.0, 0.0}, (vec2){1.0, 1.0}
};

// Matrices
static mat4 modelViewMat = GLM_MAT4_IDENTITY_INIT;
static mat4 projMat = GLM_MAT4_IDENTITY_INIT;
static mat3 normalMat = GLM_MAT3_IDENTITY_INIT;

static unsigned int
programId,
vertexShaderId,
fragmentShaderId,
modelViewMatLoc,
normalMatLoc,
projMatLoc,
canLabelTexLoc,
canTopTexLoc,
objectLoc,
buffer[24],
vao[22],
texture[22],
width,
height;

static BitMapFile *image[22]; // Local storage for bmp image data.
// Initialization routine.
void setup(void)
{
    GLenum glErr;
      // ... it does not hurt to check that everything went OK
    if ((glErr = glGetError()) != 0)
    {
        printf("Errore = %d \n", glErr);
        exit(-1);
    }

    glClearColor(1.0, 1.0, 1.0, 0.0);
    glEnable(GL_DEPTH_TEST);

    // Create shader program executable.
    vertexShaderId = setShader("vertex", "vertexShader.glsl");
    fragmentShaderId = setShader("fragment", "fragmentShader.glsl");
    programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);
    glUseProgram(programId);

    //CALCOLO I VERTICI DELLA COLONNA E DEL CAPITELLO CON I RELATIVI INIDCI
    fillCylinder(cylVertices, cylIndices, cylCounts, cylOffsets);
    fillTorus(torVertices, torIndices, torCounts, torOffsets);

    //GENERO VAO E VBO:
    // si è scelto di commentare solo la parte riferita alla base bassa, in quanto i successivi sono analoghi:
    glGenVertexArrays(22, vao);
    glGenBuffers(24, buffer);

    // PASSO I DATI LATO GPU:
    // associate data with vertex shader.
    glBindVertexArray(vao[BASEB]);
    // Da qui in poi mi riferisco a buffer[BASEB_VERTICES] e dico di che tipo sia.
    glBindBuffer(GL_ARRAY_BUFFER, buffer[BASEB_VERTICES]);
    // Alloco lo spazio buffer[BasebVertices] nella memoria della GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(BasebVertices), BasebVertices, GL_STATIC_DRAW);
    // passo al vertex shader tramite le Label vertici, normale e coordinate di texture.
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(BasebVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BasebVertices[0]), (void*)sizeof(BasebVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BasebVertices[0]),
                          (void*)(sizeof(BasebVertices[0].coords)+sizeof(BasebVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[COPB]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[COPB_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CopbVertices), CopbVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(CopbVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CopbVertices[0]), (void*)sizeof(CopbVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CopbVertices[0]),
                          (void*)(sizeof(CopbVertices[0].coords)+sizeof(CopbVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[BASEA]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[BASEA_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BaseaVertices), BaseaVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(BaseaVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BaseaVertices[0]), (void*)sizeof(BaseaVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BaseaVertices[0]),
                          (void*)(sizeof(BaseaVertices[0].coords)+sizeof(BaseaVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[COPA]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[COPA_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CopaVertices), CopaVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(CopaVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CopaVertices[0]), (void*)sizeof(CopaVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CopaVertices[0]),
                          (void*)(sizeof(CopaVertices[0].coords)+sizeof(CopaVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[PRATO]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[PRATO_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PratoVertices), PratoVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PratoVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PratoVertices[0]), (void*)sizeof(PratoVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(PratoVertices[0]),
                          (void*)(sizeof(PratoVertices[0].coords)+sizeof(PratoVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[COPP]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[COPP_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CoppVertices), CoppVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(CoppVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CoppVertices[0]), (void*)sizeof(CoppVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CoppVertices[0]),
                          (void*)(sizeof(CoppVertices[0].coords)+sizeof(CoppVertices[0].normal)));
    glEnableVertexAttribArray(2);

     glBindVertexArray(vao[TETTOB]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[TETTOB_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TettobVertices), TettobVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(TettobVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TettobVertices[0]), (void*)sizeof(TettobVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TettobVertices[0]),
                          (void*)(sizeof(TettobVertices[0].coords)+sizeof(TettobVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[COPTB]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[COPTB_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CoptbVertices), CoptbVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(CoptbVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CoptbVertices[0]), (void*)sizeof(CoptbVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CoptbVertices[0]),
                          (void*)(sizeof(CoptbVertices[0].coords)+sizeof(CoptbVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[MUROD]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[MUROD_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MurodVertices), MurodVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(MurodVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MurodVertices[0]), (void*)sizeof(MurodVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MurodVertices[0]),
                          (void*)(sizeof(MurodVertices[0].coords)+sizeof(MurodVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[MUROL]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[MUROL_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MurolVertices), MurolVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(MurolVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MurolVertices[0]), (void*)sizeof(MurolVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MurolVertices[0]),
                          (void*)(sizeof(MurolVertices[0].coords)+sizeof(MurolVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[PORTA]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[PORTA_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(portaVertices), portaVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(portaVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(portaVertices[0]), (void*)sizeof(portaVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(portaVertices[0]),
                          (void*)(sizeof(portaVertices[0].coords)+sizeof(portaVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[TETTOM]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[TETTOM_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TettomVertices), TettomVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(TettomVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TettomVertices[0]), (void*)sizeof(TettomVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TettomVertices[0]),
                          (void*)(sizeof(TettomVertices[0].coords)+sizeof(TettomVertices[0].normal)));
    glEnableVertexAttribArray(2);


    glBindVertexArray(vao[TETTOA]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[TETTOA_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TettoaVertices), TettoaVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(TettoaVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TettoaVertices[0]), (void*)sizeof(TettoaVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TettoaVertices[0]),
                          (void*)(sizeof(TettoaVertices[0].coords)+sizeof(TettoaVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[TETTOF]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[TETTOF_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TettofVertices), TettofVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(TettofVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TettofVertices[0]), (void*)sizeof(TettofVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TettofVertices[0]),
                          (void*)(sizeof(TettofVertices[0].coords)+sizeof(TettofVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[TETTOS]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[TETTOS_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TettosVertices), TettosVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(TettosVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TettosVertices[0]), (void*)sizeof(TettosVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TettosVertices[0]),
                          (void*)(sizeof(TettosVertices[0].coords)+sizeof(TettosVertices[0].normal)));
    glEnableVertexAttribArray(2);

     glBindVertexArray(vao[CYLINDER]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[CYL_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cylVertices), cylVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[CYL_INDICES]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cylIndices), cylIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(cylVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(cylVertices[0]), (void*)sizeof(cylVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(cylVertices[0]),
                          (void*)(sizeof(cylVertices[0].coords)+sizeof(cylVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[TOR]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[TOR_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(torVertices), torVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[TOR_INDICES]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(torIndices), torIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(torVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(torVertices[0]), (void*)sizeof(torVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(torVertices[0]),
                          (void*)(sizeof(torVertices[0].coords)+sizeof(torVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[QUADROU]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[QUADROU_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadrouVertices), QuadrouVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(QuadrouVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(QuadrouVertices[0]), (void*)sizeof(QuadrouVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(QuadrouVertices[0]),
                          (void*)(sizeof(QuadrouVertices[0].coords)+sizeof(QuadrouVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[QUADROD]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[QUADROD_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadrodVertices), QuadrodVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(QuadrodVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(QuadrodVertices[0]), (void*)sizeof(QuadrodVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(QuadrodVertices[0]),
                          (void*)(sizeof(QuadrodVertices[0].coords)+sizeof(QuadrodVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[DIPINTOU]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[DIPINTOU_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DipintouVertices), DipintouVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(DipintouVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DipintouVertices[0]), (void*)sizeof(DipintouVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(DipintouVertices[0]),
                          (void*)(sizeof(DipintouVertices[0].coords)+sizeof(DipintouVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[DIPINTOD]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[DIPINTOD_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DipintodVertices), DipintodVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(DipintodVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DipintodVertices[0]), (void*)sizeof(DipintodVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(DipintodVertices[0]),
                          (void*)(sizeof(DipintodVertices[0].coords)+sizeof(DipintodVertices[0].normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao[DIPINTOT]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[DIPINTOT_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DipintotVertices), DipintotVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(DipintotVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DipintotVertices[0]), (void*)sizeof(DipintotVertices[0].coords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(DipintotVertices[0]),
                          (void*)(sizeof(DipintotVertices[0].coords)+sizeof(DipintotVertices[0].normal)));
    glEnableVertexAttribArray(2);

    // FINE :associate data with vertex shader.

    // Obtain modelview matrix, projection matrix, normal matrix and object uniform locations.
    modelViewMatLoc = glGetUniformLocation(programId, "modelViewMat");
    projMatLoc = glGetUniformLocation(programId, "projMat");
    normalMatLoc = glGetUniformLocation(programId, "normalMat");
    objectLoc = glGetUniformLocation(programId, "object");

    // Obtain light property uniform locations and set values.
    glUniform4fv(glGetUniformLocation(programId, "light0.ambCols"), 1, &light0.ambCols[0]);
    glUniform4fv(glGetUniformLocation(programId, "light0.difCols"), 1, &light0.difCols[0]);
    glUniform4fv(glGetUniformLocation(programId, "light0.specCols"), 1, &light0.specCols[0]);
    glUniform4fv(glGetUniformLocation(programId, "light0.coords"), 1, &light0.coords[0]);

    // Obtain global ambient uniform location and set value.
    glUniform4fv(glGetUniformLocation(programId, "globAmb"), 1, &globAmb[0]);

    // Carico le bitmap di cui ho bisogno.
    image[0] = readBMP("./Texture/bordob.bmp");
    image[1] = readBMP("./Texture/baseb.bmp");
    image[2] = readBMP("./Texture/bordob.bmp");
    image[3] = readBMP("./Texture/baseb.bmp");
    image[4] = readBMP("./Texture/prato.bmp");
    image[5] = readBMP("./Texture/prato.bmp");
    image[6] = readBMP("./Texture/decorazione.bmp");
    image[7] = readBMP("./Texture/soffitto.bmp");
    image[8] = readBMP("./Texture/muro.bmp");
    image[9] = readBMP("./Texture/muro.bmp");
    image[10] = readBMP("./Texture/muro.bmp");
    image[11] = readBMP("./Texture/decorazionem.bmp");
    image[12] = readBMP("./Texture/decoraziones.bmp");
    image[13] = readBMP("./Texture/fronte.bmp");
    image[14] = readBMP("./Texture/tetto.bmp");
    image[15] = readBMP("./Texture/colonna.bmp");
    image[16] = readBMP("./Texture/capitello.bmp");
    image[17] = readBMP("./Texture/colonna.bmp");
    image[18] = readBMP("./Texture/colonna.bmp");
    image[19] = readBMP("./Texture/dipintou.bmp");
    image[20] = readBMP("./Texture/dipintod.bmp");
    image[21] = readBMP("./Texture/dipintot.bmp");

    // Genero 22 texture
    glGenTextures(22, texture);

    // Bind can label image.
    //attivo la textue unit 0
    glActiveTexture(GL_TEXTURE0);
    //la texture[0] conterrà un immagine 2d e da ora in poi ci riferiremo a essa
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    //copio i dati letti in image[0] nella texture[0]
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->sizeX, image[0]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
    //SETTO I PARAMETRI D'INTERPOLAZIONE:
    //se ho coordinate di tetxure maggiori di 1 e minori di 0, la parte intera della coordinata verrà ignorata e verrà formato un motivo ripetuto
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     // assegniamo la parte del textel in cui è più vicino al centro calcolato
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canLabelTexLoc = glGetUniformLocation(programId, "bordob");
    glUniform1i(canLabelTexLoc, 0);

    // Bind can top image.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->sizeX, image[1]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "baseb");
    glUniform1i(canTopTexLoc, 1);

    // Bind can top image.
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->sizeX, image[2]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "bordoa");
    glUniform1i(canTopTexLoc, 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[3]->sizeX, image[3]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[3]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "basea");
    glUniform1i(canTopTexLoc, 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[4]->sizeX, image[4]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[4]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "prato");
    glUniform1i(canTopTexLoc, 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[5]->sizeX, image[5]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[5]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "copp");
    glUniform1i(canTopTexLoc, 5);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[6]->sizeX, image[6]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[6]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "bordotb");
    glUniform1i(canTopTexLoc, 6);

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[7]->sizeX, image[7]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[7]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "coptb");
    glUniform1i(canTopTexLoc, 7);

    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[8]->sizeX, image[8]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[8]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "murod");
    glUniform1i(canTopTexLoc, 8);

    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[9]->sizeX, image[9]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[9]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "murol");
    glUniform1i(canTopTexLoc, 9);

    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[10]->sizeX, image[10]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[10]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "porta");
    glUniform1i(canTopTexLoc, 10);

    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, texture[11]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[11]->sizeX, image[11]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[11]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "bordotm");
    glUniform1i(canTopTexLoc, 11);

    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, texture[12]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[12]->sizeX, image[12]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[12]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "bordota");
    glUniform1i(canTopTexLoc, 12);

    glActiveTexture(GL_TEXTURE13);
    glBindTexture(GL_TEXTURE_2D, texture[13]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[13]->sizeX, image[13]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[13]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "bordotf");
    glUniform1i(canTopTexLoc, 13);

    glActiveTexture(GL_TEXTURE14);
    glBindTexture(GL_TEXTURE_2D, texture[14]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[14]->sizeX, image[14]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[14]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "bordots");
    glUniform1i(canTopTexLoc, 14);

    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, texture[15]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[15]->sizeX, image[15]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[15]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "colonna");
    glUniform1i(canTopTexLoc, 15);

    glActiveTexture(GL_TEXTURE16);
    glBindTexture(GL_TEXTURE_2D, texture[16]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[16]->sizeX, image[16]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[16]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "toro");
    glUniform1i(canTopTexLoc, 16);

    glActiveTexture(GL_TEXTURE17);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[17]->sizeX, image[17]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[17]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "quadrou");
    glUniform1i(canTopTexLoc, 17);

    glActiveTexture(GL_TEXTURE18);
    glBindTexture(GL_TEXTURE_2D, texture[18]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[18]->sizeX, image[18]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[18]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "quadrod");
    glUniform1i(canTopTexLoc, 18);

    glActiveTexture(GL_TEXTURE19);
    glBindTexture(GL_TEXTURE_2D, texture[19]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[19]->sizeX, image[19]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[19]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "dipintou");
    glUniform1i(canTopTexLoc, 19);

    glActiveTexture(GL_TEXTURE20);
    glBindTexture(GL_TEXTURE_2D, texture[20]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[20]->sizeX, image[20]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[20]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "dipintod");
    glUniform1i(canTopTexLoc, 20);

    glActiveTexture(GL_TEXTURE21);
    glBindTexture(GL_TEXTURE_2D, texture[21]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[21]->sizeX, image[21]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[21]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    canTopTexLoc = glGetUniformLocation(programId, "dipintot");
    glUniform1i(canTopTexLoc, 21);

    // ... it does not hurt to check that everything went OK
    if ((glErr = glGetError()) != 0)
    {
        printf("Errore = %d \n", glErr);
        exit(-1);
    }
}
// Drawing routine.
void display(void)
{
    mat3 TMP;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate and update projection matrix.
    glm_perspective(30.0f, (float)width/(float)height, 1.0f, 70.0f, projMat);
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, (GLfloat *) projMat);

    // Calculate and update modelview matrix.
    glm_mat4_identity(modelViewMat);
    // I parametri nella glm_lookat, possono cambiare tramite la presione dei tasti inidicati a inizio codice,
    // questo viene getsito tramite la funzione: void keyInput()
    // ogni volta che si fa il rendering della scena, stessa cosa per la glm_rotate
    glm_lookat((vec3){Xdirezione, Ydirezione, Zdirezione}, (vec3){Xdirezione, Ydirezione, -25.0},
        (vec3){0.0, 1.0, 0.0}, modelViewMat);
    glm_rotate(modelViewMat, Yangle, (vec3){0.0, 1.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));

    // Calculate and update normal matrix.
    glm_mat4_pick3(modelViewMat, TMP);
    glm_mat3_inv(TMP, normalMat);
    glm_mat3_transpose(normalMat);
    glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, (GLfloat *)normalMat);

    // SETTO IL MATERIALE PER IL PAVIMENTO
    glUniform4fv(glGetUniformLocation(programId, "canFandB.ambRefl"), 1, &base.ambRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.difRefl"), 1, &base.difRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.specRefl"), 1, &base.specRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.emitCols"), 1, &base.emitCols[0]);
    glUniform1f(glGetUniformLocation(programId, "canFandB.shininess"), base.shininess);

    // METTO A VIDEO:

    // BASE BASSA
    glUniform1ui(objectLoc, BASEB);
    glBindVertexArray(vao[BASEB]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    //COPERCHIO BASA BASSA
    glUniform1ui(objectLoc, COPB);
    glBindVertexArray(vao[COPB]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // BASE ALTA
    glm_translate(modelViewMat, (vec3){0.0, 1.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glUniform1ui(objectLoc, BASEA);
    glBindVertexArray(vao[BASEA]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    //COPERCHIO BASE ALTA
    glUniform1ui(objectLoc, COPA);
    glBindVertexArray(vao[COPA]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //SETTO IL MATERIALE PER IL PRATO
    glUniform4fv(glGetUniformLocation(programId, "canFandB.ambRefl"), 1, &prato.ambRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.difRefl"), 1, &prato.difRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.specRefl"), 1, &prato.specRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.emitCols"), 1, &prato.emitCols[0]);
    glUniform1f(glGetUniformLocation(programId, "canFandB.shininess"), prato.shininess);
    //PRATO
    glm_translate(modelViewMat, (vec3){0.0, 4.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glUniform1ui(objectLoc, PRATO);
    glBindVertexArray(vao[PRATO]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    //COPERCHIO PRATO
    glUniform1ui(objectLoc, COPP);
    glBindVertexArray(vao[COPP]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //TORNO AL PUNTO 0.0
    glm_perspective(30.0f, (float)width/(float)height, 1.0f, 70.0f, projMat);
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, (GLfloat *) projMat);
    glm_mat4_identity(modelViewMat);
    glm_lookat((vec3){Xdirezione, Ydirezione, Zdirezione}, (vec3){Xdirezione, Ydirezione, -25.0},
        (vec3){0.0, 1.0, 0.0}, modelViewMat);
    glm_rotate(modelViewMat, Yangle, (vec3){0.0, 1.0, 0.0});
    glm_translate(modelViewMat, (vec3){0.0, -8.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    // MATERIALE PER IL TETTO
    glUniform4fv(glGetUniformLocation(programId, "canFandB.ambRefl"), 1, &suptetto.ambRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.difRefl"), 1, &suptetto.difRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.specRefl"), 1, &suptetto.specRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.emitCols"), 1, &suptetto.emitCols[0]);
    glUniform1f(glGetUniformLocation(programId, "canFandB.shininess"), suptetto.shininess);
    //TETTO BASSO
    glUniform1ui(objectLoc, TETTOB);
    glBindVertexArray(vao[TETTOB]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    //COPERCHIO TETTO BASSO
    glUniform1ui(objectLoc, COPTB);
    glBindVertexArray(vao[COPTB]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //TETTO MEZZO
    glm_translate(modelViewMat, (vec3){0.0, -1.5, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glUniform1ui(objectLoc, TETTOM);
    glBindVertexArray(vao[TETTOM]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    //TETTO ALTO
    glm_translate(modelViewMat, (vec3){0.0, -1.5, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glUniform1ui(objectLoc, TETTOA);
    glBindVertexArray(vao[TETTOA]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    //TETTO FRONTE AVANTI
    // SETTO IL MATERIALE PER DIPINTO TETTO
    glUniform4fv(glGetUniformLocation(programId, "canFandB.ambRefl"), 1, &fronte.ambRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.difRefl"), 1, &fronte.difRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.specRefl"), 1, &fronte.specRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.emitCols"), 1, &fronte.emitCols[0]);
    glUniform1f(glGetUniformLocation(programId, "canFandB.shininess"), fronte.shininess);
    glm_translate(modelViewMat, (vec3){0.0, -1.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glUniform1ui(objectLoc, TETTOF);
    glBindVertexArray(vao[TETTOF]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //TETTO FRONTE DIETRO
    glm_translate(modelViewMat, (vec3){0.0, 0.0, -36.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glUniform1ui(objectLoc, TETTOF);
    glBindVertexArray(vao[TETTOF]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //TORNO AL PUNTO 0.0
    glm_mat4_identity(modelViewMat);
    glm_lookat((vec3){Xdirezione, Ydirezione, Zdirezione}, (vec3){Xdirezione, Ydirezione, -25.0},
        (vec3){0.0, 1.0, 0.0}, modelViewMat);
    glm_rotate(modelViewMat, Yangle, (vec3){0.0, 1.0, 0.0});
    glm_translate(modelViewMat, (vec3){0.0, -4.5, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glm_translate(modelViewMat, (vec3){5.0, -5.5, 0.0});
    //RUOTO DI 20 GRADI
    glm_rotate(modelViewMat,0.35, (vec3){0.0, 0.0, 1.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    // SETTO IL MATERIALE PER SUPERFICIE TETTO, TEGOLE
    glUniform4fv(glGetUniformLocation(programId, "canFandB.ambRefl"), 1, &tetto.ambRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.difRefl"), 1, &tetto.difRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.specRefl"), 1, &tetto.specRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.emitCols"), 1, &tetto.emitCols[0]);
    glUniform1f(glGetUniformLocation(programId, "canFandB.shininess"), tetto.shininess);
    glUniform1ui(objectLoc, TETTOS);
    glBindVertexArray(vao[TETTOS]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //SPECULARE TETTO
    glm_mat4_identity(modelViewMat);
    glm_lookat((vec3){Xdirezione, Ydirezione, Zdirezione}, (vec3){Xdirezione, Ydirezione, -25.0},
        (vec3){0.0, 1.0, 0.0}, modelViewMat);
    glm_rotate(modelViewMat, Yangle, (vec3){0.0, 1.0, 0.0});
    glm_translate(modelViewMat, (vec3){0.0, -4.5, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glm_translate(modelViewMat, (vec3){-5.0, -5.5, 0.0});
    glm_rotate(modelViewMat,-0.35, (vec3){0.0, 0.0, 1.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glUniform1ui(objectLoc, TETTOS);
    glBindVertexArray(vao[TETTOS]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // TONRO ALL'ORIGINE 0,0 MURO DIETRO
    glm_mat4_identity(modelViewMat);
    glm_lookat((vec3){Xdirezione, Ydirezione, Zdirezione}, (vec3){Xdirezione, Ydirezione, -25.0},
        (vec3){0.0, 1.0, 0.0}, modelViewMat);
    glm_rotate(modelViewMat, Yangle, (vec3){0.0, 1.0, 0.0});
    glm_translate(modelViewMat, (vec3){0.0, -4.0, -14.0});
    //MATERIALE PER I MURI
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glUniform4fv(glGetUniformLocation(programId, "canFandB.ambRefl"), 1, &fronte.ambRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.difRefl"), 1, &fronte.difRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.specRefl"), 1, &fronte.specRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.emitCols"), 1, &fronte.emitCols[0]);
    glUniform1f(glGetUniformLocation(programId, "canFandB.shininess"), fronte.shininess);
    glUniform1ui(objectLoc, MUROD);
    glBindVertexArray(vao[MUROD]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    // TONRO ALL'ORIGINE 0,0 MURO LATERALE
    glm_mat4_identity(modelViewMat);
    glm_lookat((vec3){Xdirezione, Ydirezione, Zdirezione}, (vec3){Xdirezione, Ydirezione, -25.0},
        (vec3){0.0, 1.0, 0.0}, modelViewMat);
    glm_rotate(modelViewMat, Yangle, (vec3){0.0, 1.0, 0.0});
    glm_translate(modelViewMat, (vec3){10.0, -4.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glUniform1ui(objectLoc, MUROL);
    glBindVertexArray(vao[MUROL]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    glm_translate(modelViewMat, (vec3){-4.0, 0.0, 7.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    // PORTA
    glUniform1ui(objectLoc, PORTA);
    glBindVertexArray(vao[PORTA]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    glm_translate(modelViewMat, (vec3){0.0, 0.0, 7.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    //SPECULARE
    glm_mat4_identity(modelViewMat);
    glm_lookat((vec3){Xdirezione, Ydirezione, Zdirezione}, (vec3){Xdirezione, Ydirezione, -25.0},
        (vec3){0.0, 1.0, 0.0}, modelViewMat);
    glm_rotate(modelViewMat, Yangle, (vec3){0.0, 1.0, 0.0});
    glm_translate(modelViewMat, (vec3){-10.0, -4.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glUniform1ui(objectLoc, MUROL);
    glBindVertexArray(vao[MUROL]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    glm_translate(modelViewMat, (vec3){4.0, 0.0, 7.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));

    glUniform1ui(objectLoc, PORTA);
    glBindVertexArray(vao[PORTA]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    glm_translate(modelViewMat, (vec3){0.0, 0.0, 7.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);

    // TORNO ALL'ORIGINE 0,0
    glm_mat4_identity(modelViewMat);
    glm_lookat((vec3){Xdirezione, Ydirezione, Zdirezione}, (vec3){Xdirezione, Ydirezione, -25.0},
        (vec3){0.0, 1.0, 0.0}, modelViewMat);
    glm_rotate(modelViewMat, Yangle, (vec3){0.0, 1.0, 0.0});
    glm_translate(modelViewMat, (vec3){-11.5, -2.0, 17.0});
    glm_rotate(modelViewMat,1.5708, (vec3){1.0, 0.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    // SETTO IL MATERIALE PER LE COLONNE
    glUniform4fv(glGetUniformLocation(programId, "canFandB.ambRefl"), 1, &colonna.ambRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.difRefl"), 1, &colonna.difRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.specRefl"), 1, &colonna.specRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.emitCols"), 1, &colonna.emitCols[0]);
    glUniform1f(glGetUniformLocation(programId, "canFandB.shininess"),colonna.shininess);
    glUniform1ui(objectLoc, CYLINDER);
    glBindVertexArray(vao[CYLINDER]);
    //COLONNE LATERALE DESTRA
    for (int i =0; i < 11; i++){
    glMultiDrawElements(GL_TRIANGLE_STRIP, cylCounts, GL_UNSIGNED_INT,
        (const void **)cylOffsets, CYL_LATS);
    glm_translate(modelViewMat, (vec3){0.0, -3.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    }

    glm_translate(modelViewMat, (vec3){0.0, -1.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
     //COLONNE DIETRO
    for (int i =0; i < 8; i++){
    glMultiDrawElements(GL_TRIANGLE_STRIP, cylCounts, GL_UNSIGNED_INT,
        (const void **)cylOffsets, CYL_LATS);
    glm_translate(modelViewMat, (vec3){3.0, 0.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    }

    glm_translate(modelViewMat, (vec3){-1.0, 0.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
     //COLONNE LATERALE SINISTRA
     for (int i =0; i < 12; i++){
    glMultiDrawElements(GL_TRIANGLE_STRIP, cylCounts, GL_UNSIGNED_INT,
        (const void **)cylOffsets, CYL_LATS);
    glm_translate(modelViewMat, (vec3){0.0, 3.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    }

    glm_translate(modelViewMat, (vec3){-2.0, -2.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
     //COLONNE AVANTI
    for (int i =0; i < 7; i++){
    glMultiDrawElements(GL_TRIANGLE_STRIP, cylCounts, GL_UNSIGNED_INT,
        (const void **)cylOffsets, CYL_LATS);
    glm_translate(modelViewMat, (vec3){-3.0, 0.0, 0.0});
     //PERMETTE DI NON DISEGNARE LA COLONNA DAVANTI LA PORTA
    if(i == 2){
            glm_translate(modelViewMat, (vec3){-3.0, 0.0, 0.0});
        }
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    }

    //TORO INFERIORE:
    glm_mat4_identity(modelViewMat);
    glm_lookat((vec3){Xdirezione, Ydirezione, Zdirezione}, (vec3){Xdirezione, Ydirezione, -25.0},
        (vec3){0.0, 1.0, 0.0}, modelViewMat);
    glm_rotate(modelViewMat, Yangle, (vec3){0.0, 1.0, 0.0});
    glm_translate(modelViewMat, (vec3){-11.5, -2.0, 17.0});
    glm_rotate(modelViewMat,1.5708, (vec3){1.0, 0.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glm_translate(modelViewMat, (vec3){0.0, 0.0, -1.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    // SETTO IL MATERIALE PER I CAPITELLI
    glUniform4fv(glGetUniformLocation(programId, "canFandB.ambRefl"), 1, &capitello.ambRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.difRefl"), 1, &capitello.difRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.specRefl"), 1, &capitello.specRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.emitCols"), 1, &capitello.emitCols[0]);
    glUniform1f(glGetUniformLocation(programId, "canFandB.shininess"), capitello.shininess);

    glUniform1ui(objectLoc, TOR); // Update object name.
    glBindVertexArray(vao[TOR]);
    glMultiDrawElements(GL_TRIANGLE_STRIP, torCounts, GL_UNSIGNED_INT, (const void **)torOffsets, 30);
     //TORO LATERALE DESTRA
    for (int i =0; i < 11; i++){
        glMultiDrawElements(GL_TRIANGLE_STRIP, torCounts, GL_UNSIGNED_INT, (const void **)torOffsets, 30);
        glm_translate(modelViewMat, (vec3){0.0, -3.0, 0.0});
        glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    }

    glm_translate(modelViewMat, (vec3){0.0, -1.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
     //TORO DIETRO
    for (int i =0; i < 8; i++){
        glMultiDrawElements(GL_TRIANGLE_STRIP, torCounts, GL_UNSIGNED_INT, (const void **)torOffsets, 30);
        glm_translate(modelViewMat, (vec3){3.0, 0.0, 0.0});
        glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    }

    glm_translate(modelViewMat, (vec3){-1.0, 0.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    //TORO LATERALE SINISTRA
    for (int i =0; i < 12; i++){
        glMultiDrawElements(GL_TRIANGLE_STRIP, torCounts, GL_UNSIGNED_INT, (const void **)torOffsets, 30);
        glm_translate(modelViewMat, (vec3){0.0, 3.0, 0.0});
        glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    }

    glm_translate(modelViewMat, (vec3){-2.0, -2.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    //TORO AVANTI
    for (int i =0; i < 7; i++){
        glMultiDrawElements(GL_TRIANGLE_STRIP, torCounts, GL_UNSIGNED_INT, (const void **)torOffsets, 30);
        glm_translate(modelViewMat, (vec3){-3.0, 0.0, 0.0});
         //PERMETTE DI NON DISEGNARE IL TORO DAVANTI LA PORTA
        if(i == 2){
            glm_translate(modelViewMat, (vec3){-3.0, 0.0, 0.0});
        }
        glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    }
    //TORO SUPERIORE: non commentato, in quanto è uguale al precedente
    glm_mat4_identity(modelViewMat);
    glm_lookat((vec3){Xdirezione, Ydirezione, Zdirezione}, (vec3){Xdirezione, Ydirezione, -25.0},
        (vec3){0.0, 1.0, 0.0}, modelViewMat);
    glm_rotate(modelViewMat, Yangle, (vec3){0.0, 1.0, 0.0});
    glm_translate(modelViewMat, (vec3){-11.5, -2.2, 17.0});
    glm_rotate(modelViewMat,1.5708, (vec3){1.0, 0.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glm_translate(modelViewMat, (vec3){0.0, 0.0, 5.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));

    glUniform1ui(objectLoc, TOR); // Update object name.
    glBindVertexArray(vao[TOR]);
    glMultiDrawElements(GL_TRIANGLE_STRIP, torCounts, GL_UNSIGNED_INT, (const void **)torOffsets, 30);

    for (int i =0; i < 11; i++){
        glMultiDrawElements(GL_TRIANGLE_STRIP, torCounts, GL_UNSIGNED_INT, (const void **)torOffsets, 30);
        glm_translate(modelViewMat, (vec3){0.0, -3.0, 0.0});
        glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    }

    glm_translate(modelViewMat, (vec3){0.0, -1.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));

    for (int i =0; i < 8; i++){
        glMultiDrawElements(GL_TRIANGLE_STRIP, torCounts, GL_UNSIGNED_INT, (const void **)torOffsets, 30);
        glm_translate(modelViewMat, (vec3){3.0, 0.0, 0.0});
        glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    }

    glm_translate(modelViewMat, (vec3){-1.0, 0.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));

    for (int i =0; i < 12; i++){
        glMultiDrawElements(GL_TRIANGLE_STRIP, torCounts, GL_UNSIGNED_INT, (const void **)torOffsets, 30);
        glm_translate(modelViewMat, (vec3){0.0, 3.0, 0.0});
        glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    }

    glm_translate(modelViewMat, (vec3){-2.0, -2.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));

    for (int i =0; i < 7; i++){
        glMultiDrawElements(GL_TRIANGLE_STRIP, torCounts, GL_UNSIGNED_INT, (const void **)torOffsets, 30);
        glm_translate(modelViewMat, (vec3){-3.0, 0.0, 0.0});
        if(i == 2){
            glm_translate(modelViewMat, (vec3){-3.0, 0.0, 0.0});
        }
        glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    }
    //POSIZIONE CORNICI
     glm_mat4_identity(modelViewMat);
    glm_lookat((vec3){Xdirezione, Ydirezione, Zdirezione}, (vec3){Xdirezione, Ydirezione, -25.0},
        (vec3){0.0, 1.0, 0.0}, modelViewMat);
    glm_rotate(modelViewMat, Yangle, (vec3){0.0, 1.0, 0.0});
    glm_translate(modelViewMat, (vec3){9, -4.0, -6.0});
    glm_rotate(modelViewMat, 1.57, (vec3){0.0, 0.0, 1.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    // SETTO IL MATERIALE PER LE CORNICI
    glUniform4fv(glGetUniformLocation(programId, "canFandB.ambRefl"), 1, &fronte.ambRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.difRefl"), 1, &fronte.difRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.specRefl"), 1, &fronte.specRefl[0]);
    glUniform4fv(glGetUniformLocation(programId, "canFandB.emitCols"), 1, &fronte.emitCols[0]);
    glUniform1f(glGetUniformLocation(programId, "canFandB.shininess"), fronte.shininess);

    glUniform1ui(objectLoc, QUADROU);
    glBindVertexArray(vao[QUADROU]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);

    glm_translate(modelViewMat, (vec3){0.0, 18.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);

    glm_translate(modelViewMat, (vec3){0.0, -18.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));

    glm_translate(modelViewMat, (vec3){0.0, 9.0, -6.5});
    glm_rotate(modelViewMat, -1.57, (vec3){0.0, 0.0, 1.0});
    glm_rotate(modelViewMat, 1.57, (vec3){1.0, 0.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glUniform1ui(objectLoc, QUADROD);
    glBindVertexArray(vao[QUADROD]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    //POSIZIONE DIPINTI
    glm_mat4_identity(modelViewMat);
    glm_lookat((vec3){Xdirezione, Ydirezione, Zdirezione}, (vec3){Xdirezione, Ydirezione, -25.0},
        (vec3){0.0, 1.0, 0.0}, modelViewMat);
    glm_rotate(modelViewMat, Yangle, (vec3){0.0, 1.0, 0.0});
    glm_translate(modelViewMat, (vec3){8, -4.0, -6.0});
    glm_rotate(modelViewMat, 1.57, (vec3){0.0, 0.0, 1.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));

    glUniform1ui(objectLoc, DIPINTOU);
    glBindVertexArray(vao[DIPINTOU]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glm_translate(modelViewMat, (vec3){0.0, 17.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));

    glUniform1ui(objectLoc, DIPINTOD);
    glBindVertexArray(vao[DIPINTOD]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glm_translate(modelViewMat, (vec3){0.0, -9.0, -6.0});
    glm_rotate(modelViewMat, -1.57, (vec3){0.0, 0.0, 1.0});
    glm_rotate(modelViewMat, 1.57, (vec3){1.0, 0.0, 0.0});
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, (GLfloat *)(modelViewMat));
    glUniform1ui(objectLoc, DIPINTOT);
    glBindVertexArray(vao[DIPINTOT]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
}
// GESTIONE DEI TASTI PREMUTI PER MUOVERSI NELLA SCENA E RUOTARLA RISPETTO L'ASSE Y
void keyInput(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27:
        exit(0);
        break;
    case '6':
        if(Xdirezione > -40)
            Xdirezione = Xdirezione - 0.5;
        glutPostRedisplay();
        break;
    case '4':
        if(Xdirezione < 40)
            Xdirezione = Xdirezione + 0.5;
        glutPostRedisplay();
        break;
    case '2':
        if(Ydirezione < -5)
            Ydirezione = Ydirezione + 0.2;
        glutPostRedisplay();
        break;
    case '8':
        if(Ydirezione > -40)
            Ydirezione = Ydirezione - 0.2;
        glutPostRedisplay();
        break;
    case '1':
        if(Zdirezione < 40)
            Zdirezione = Zdirezione + 0.2;
        glutPostRedisplay();
        break;
    case '9':
        if(Zdirezione > -19)
            Zdirezione = Zdirezione - 0.2;
        glutPostRedisplay();
        break;
    case 'y':
        Yangle = Yangle + 0.05;
            glutPostRedisplay();
        break;
    case 'Y':
        Yangle = Yangle - 0.05;
            glutPostRedisplay();
        break;
    default:
        break;
    }

}


// Main routine.
int main(int argc, char **argv)
{
    GLenum glErr;

    printf("Breve tutorial sui comandi : \n");
    printf("Per spostare la visuale, premere 'Y' o 'y' \n");
    printf("Per spostarsi nella scena premere: \n");
    printf("8(su)-2(giu)-6(destra)-4(sinistra)-9(andare avanti)-1(andare indietro) \n");
    printf("Assicurati di aver attivato i tasti \n");
    glutInit(&argc, argv);
    glutInitContextVersion(4, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("PROGETTO Emiliano Carulli");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glewInit();

    // ... it does not hurt to check that everything went OK
    if ((glErr = glGetError()) != 0)
    {
        printf("Errore = %d \n", glErr);
        exit(-1);
    }

    setup();
    glutMainLoop();

    return 0;
}
