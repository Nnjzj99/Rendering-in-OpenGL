#ifndef TORUS_H_INCLUDED
#define TORUS_H_INCLUDED

#include "vertex.h"

#define TOR_OUTRAD 0.5 // Torus outer radius.
#define TOR_INRAD 0.3 // Torus inner radius.
#define TOR_LONGS 25 // Number of longitudinal slices.
#define TOR_LATS 25 // Number of latitudinal slices.
#define TOR_COLORS 0.0, 1.0, 0.0, 1.0 // Torus colors.

void fillTorVertexArray(Vertex torVertices[(TOR_LONGS + 1) * (TOR_LATS + 1)]);
void fillTorIndices(unsigned int torIndices[TOR_LATS][2*(TOR_LONGS+1)]);
void fillTorCounts(int torCounts[TOR_LATS]);
void fillTorOffsets(void* torOffsets[TOR_LATS]);

void fillTorus(Vertex torVertices[(TOR_LONGS + 1) * (TOR_LATS + 1)],
	         unsigned int torIndices[TOR_LATS][2*(TOR_LONGS+1)],
			 int torCounts[TOR_LATS],
			 void* torOffsets[TOR_LATS]);

#endif // TORUS_H_INCLUDED
