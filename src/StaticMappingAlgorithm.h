#ifndef STATIC_MAPPING_ALGORITHM_H
#define STATIC_MAPPING_ALGORITHM_H 

#include <graph/Graph.h>
#include <Globals.h>

#include <unordered_map>

using NetworKit::node;

namespace StaticMapping {

class StaticMapping {
public:
    virtual void run() = 0; 
};

} // namespace
#endif
