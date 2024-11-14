#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <cglm/types-struct.h>

#include "torus.h"

// Fill the vertex array with co-ordinates of the sample points.
void fillTorVertexArray(Vertex torVertices[(TOR_LONGS + 1) * (TOR_LATS + 1)]) {
    int i, j, k;

    k = 0;
    for (j = 0; j <= TOR_LATS; j++)
        for (i = 0; i <= TOR_LONGS; i++)
        {

            torVertices[k].coords.x = ( TOR_OUTRAD + TOR_INRAD * cos( (-1 + 2*(float)j/TOR_LATS) * M_PI ) ) * cos( (-1 + 2*(float)i/TOR_LONGS) * M_PI );
            torVertices[k].coords.y = ( TOR_OUTRAD + TOR_INRAD * cos( (-1 + 2*(float)j/TOR_LATS) * M_PI ) ) * sin( (-1 + 2*(float)i/TOR_LONGS) * M_PI );
            torVertices[k].coords.z = TOR_INRAD * sin( (-1 + 2*(float)j/TOR_LATS) * M_PI );
            torVertices[k].coords.w = 1.0;

            torVertices[k].normal.x = ( TOR_OUTRAD + TOR_INRAD * cos( (-1 + 2*(float)j/TOR_LATS) * M_PI ) ) * cos( (-1 + 2*(float)i/TOR_LONGS) * M_PI );
            torVertices[k].normal.y = ( TOR_OUTRAD + TOR_INRAD * cos( (-1 + 2*(float)j/TOR_LATS) * M_PI ) ) * sin( (-1 + 2*(float)i/TOR_LONGS) * M_PI );
            torVertices[k].normal.z = TOR_INRAD * sin( (-1 + 2*(float)j/TOR_LATS) * M_PI );

            torVertices[k].texCoords.x = (float)i/TOR_LONGS;
            torVertices[k].texCoords.y = (float)j/TOR_LATS;

            k++;
        }
}

// Fill the array of index arrays.
void fillTorIndices(unsigned int torIndices[TOR_LATS][2*(TOR_LONGS+1)]) {
    int i, j;
    for(j = 0; j < TOR_LATS; j++)
    {
        for (i = 0; i <= TOR_LONGS; i++)
        {
            torIndices[j][2*i] = (j+1)*(TOR_LONGS + 1) + i;
            torIndices[j][2*i+1] = j*(TOR_LONGS + 1) + i;
        }
    }
}

// Fill the array of counts.
void fillTorCounts(int torCounts[TOR_LATS]) {
    int j;
    for(j = 0; j < TOR_LATS; j++) torCounts[j] = 2*(TOR_LONGS + 1);
}

// Fill the array of buffer offsets.
void fillTorOffsets(void* torOffsets[TOR_LATS]) {
    int j;
    for(j = 0; j < TOR_LATS; j++) torOffsets[j] = (GLvoid*)(2*(TOR_LONGS+1)*j*sizeof(unsigned int));
}

// Initialize the torus.
void fillTorus(Vertex torVertices[(TOR_LONGS + 1) * (TOR_LATS + 1)],
               unsigned int torIndices[TOR_LATS][2*(TOR_LONGS+1)],
               int torCounts[TOR_LATS],
               void* torOffsets[TOR_LATS]) {
    fillTorVertexArray(torVertices);
    fillTorIndices(torIndices);
    fillTorCounts(torCounts);
    fillTorOffsets(torOffsets);
}
