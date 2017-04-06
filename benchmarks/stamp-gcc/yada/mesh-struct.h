#ifndef MESH_H
#define MESH_H 1


#include "element-struct.h"
#include "queue-struct-v2.h"
#include "rbtree-struct-v2.h"
#include "random.h"
#include "vector.h"
#include "set.h"
#include "map.h"

typedef struct mesh  mesh_t;

struct mesh {
    element_t* rootElementPtr;
    queue_t* initBadQueuePtr;
    long size;
    SET_T* boundarySetPtr;
};



#endif /* MESH_H */
