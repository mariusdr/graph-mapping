#ifndef STATIC_MAPPING_GRAPH_GENERATORS_H
#define STATIC_MAPPING_GRAPH_GENERATORS_H

#include <vector>
#include <graph/Graph.h>

namespace StaticMapping {
namespace Util {

class HyperCubeGenerator {
public:
	explicit HyperCubeGenerator(unsigned int dim);
	NetworKit::Graph generate(); 
	
private:
	unsigned int dim;
	size_t numberOfNodes;
};



}} // namespace


#endif
