#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>

#include "Square.h"

// Fill the vertex array with co-ordinates of the sample points.
void fillDiscVertexArray(Vertex discVertices[DISC_SEGS])
{
   int k=0;
    //FRONTE
      discVertices[k].coords.x = -1.0;
      discVertices[k].coords.y = 0.0;
      discVertices[k].coords.z = -1.0;
      discVertices[k].coords.w = 1.0;
      discVertices[k].normal.x = 0.0;
      discVertices[k].normal.y = 0.0;
      discVertices[k].normal.z = 1.0;
	  discVertices[k].texCoords.x = 0;
	  discVertices[k].texCoords.y = 0;
	  k++;

	    discVertices[k].coords.x = 0.0;
      discVertices[k].coords.y = -1.0;
      discVertices[k].coords.z = 0.0;
      discVertices[k].coords.w = 1.0;
      discVertices[k].normal.x = 0.0;
      discVertices[k].normal.y = 0.0;
      discVertices[k].normal.z = 1.0;
	  discVertices[k].texCoords.x = 1;
	  discVertices[k].texCoords.y = 0;
	  k++;

	    discVertices[k].coords.x = 1;
      discVertices[k].coords.y = 0;
      discVertices[k].coords.z = 1.0;
      discVertices[k].coords.w = 1.0;
      discVertices[k].normal.x = 0.0;
      discVertices[k].normal.y = 0.0;
      discVertices[k].normal.z = 1.0;
	  discVertices[k].texCoords.x = 1.0;
	  discVertices[k].texCoords.y = 1.0;
	  k++;

	    discVertices[k].coords.x = 0;
      discVertices[k].coords.y = 1;
      discVertices[k].coords.z = 0;
      discVertices[k].coords.w = 1.0;
      discVertices[k].normal.x = 0.0;
      discVertices[k].normal.y = 0.0;
      discVertices[k].normal.z = 1.0;
	  discVertices[k].texCoords.x = 0.0;
	  discVertices[k].texCoords.y = 1.0;
      k++;

	  //LATERALE SINISTRA
      discVertices[k].coords.x = -1.0;
      discVertices[k].coords.y = 0.0;
      discVertices[k].coords.z = -1.0;
      discVertices[k].coords.w = 1.0;
      discVertices[k].normal.x = 0.0;
      discVertices[k].normal.y = 0.0;
      discVertices[k].normal.z = 1.0;
	  discVertices[k].texCoords.x = 0;
	  discVertices[k].texCoords.y = 0;
	  k++;

	    discVertices[k].coords.x = -1.0;
      discVertices[k].coords.y = 0.0;
      discVertices[k].coords.z = -1.0;
      discVertices[k].coords.w = 1.0;
      discVertices[k].normal.x = 0.0;
      discVertices[k].normal.y = 0.0;
      discVertices[k].normal.z = 1.0;
	  discVertices[k].texCoords.x = 1;
	  discVertices[k].texCoords.y = 0;
	  k++;

	    discVertices[k].coords.x = 0;
      discVertices[k].coords.y = -1;
      discVertices[k].coords.z = 0.0;
      discVertices[k].coords.w = 1.0;
      discVertices[k].normal.x = 0.0;
      discVertices[k].normal.y = 0.0;
      discVertices[k].normal.z = 1.0;
	  discVertices[k].texCoords.x = 1.0;
	  discVertices[k].texCoords.y = 1.0;
	  k++;

	    discVertices[k].coords.x = 1.0;
      discVertices[k].coords.y = 0;
      discVertices[k].coords.z = 1.0;
      discVertices[k].coords.w = 1.0;
      discVertices[k].normal.x = 0.0;
      discVertices[k].normal.y = 0.0;
      discVertices[k].normal.z = 1.0;
	  discVertices[k].texCoords.x = 0.0;
	  discVertices[k].texCoords.y = 1.0;
      k++;

	    }


