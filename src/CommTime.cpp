#include "CommTime.h" 

#include <algorithm>

#include <auxiliary/PrioQueue.h>

namespace StaticMapping {

ModDijsktra::ModDijsktra(const NetworKit::Graph& G, NetworKit::node source,
						 std::function<NetworKit::edgeweight(NetworKit::edgeweight)> edgeMod,
						 bool storePaths,
						 bool storeStack, NetworKit::node target):
	Dijkstra(G, source, storePaths, storeStack, target),
	edgeModifier(edgeMod)
{}

void ModDijsktra::run() {
	NetworKit::edgeweight infDist = std::numeric_limits<NetworKit::edgeweight>::max();
	distances.clear();
	distances.resize(G.upperNodeIdBound(), infDist);

	if (storePaths) {
		previous.clear();
		previous.resize(G.upperNodeIdBound());

		npaths.clear();
		npaths.resize(G.upperNodeIdBound(), 0);
		npaths[source] = 1;
	}

	if (storeStack) {
		std::vector<NetworKit::node> empty;
		std::swap(stack, empty);
	}

	distances[source] = 0;
	Aux::PrioQueue<NetworKit::edgeweight, NetworKit::node> pq(distances);

	auto relax([&](NetworKit::node u, NetworKit::node v, NetworKit::edgeweight w) {
		NetworKit::edgeweight mw = edgeModifier(w);
		if (distances[v] > distances[u] + mw) {
			distances[v] = distances[u] + mw;
			if (storePaths) {
				previous[v] = {u};
				npaths[v] = npaths[u];
			}
			pq.decreaseKey(distances[v], v);
		} else if (storePaths && (distances[v] == distances[u] + mw)) {
			previous[v].push_back(u);
			npaths[v] += npaths[u];
		}
	});

	bool breakWhenFound = (target != NetworKit::none);
	while (pq.size() > 0) {
		NetworKit::node current = pq.extractMin().second;
		if (breakWhenFound && target == current) {
			break;
		}
		if (storeStack) {
			stack.push_back(current);
		}
		G.forEdgesOf(current, relax);
	}
}

ModAPSP::ModAPSP(const NetworKit::Graph& G,
				 std::function<NetworKit::edgeweight(NetworKit::edgeweight)> edgeMod):
	APSP(G), edgeModifier(edgeMod)
{}

void ModAPSP::run() {
	std::vector<NetworKit::edgeweight> distanceVector(G.upperNodeIdBound(), 0.f);
	distances.resize(G.upperNodeIdBound(), distanceVector);

	G.parallelForNodes([&](NetworKit::node u){
	    ModDijsktra dijk(G, u, edgeModifier);
		dijk.run();
		distances[u] = dijk.getDistances();
	});
	hasRun = true;
} 

CommTime::CommTime(const NetworKit::Graph& graph):
	graph(graph), hasrun(false),
	apsp(ModAPSP(graph, [](NetworKit::edgeweight ew) {
	    if (ew == 0)
		    throw std::runtime_error("edgeweight in processor graph can't be 0");
	    return 1.0 / ew;
	}))
{}

void CommTime::run() {
	apsp.run();
	hasrun = true;
}

bool CommTime::hasRun() const {
	return hasrun;
}

double CommTime::time(NetworKit::node u, NetworKit::node v) const {
	return apsp.getDistance(u, v);
}

} // namespace
