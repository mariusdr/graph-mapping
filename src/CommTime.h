#ifndef COMM_TIME_H
#define COMM_TIME_H

#include <graph/APSP.h>
#include <graph/SSSP.h>
#include <graph/Dijkstra.h>
#include <graph/Graph.h>

#include <functional>


namespace StaticMapping {

class ModDijsktra : public NetworKit::Dijkstra {
public:
	ModDijsktra(const NetworKit::Graph& G, NetworKit::node source,
				std::function<NetworKit::edgeweight(NetworKit::edgeweight)> edgeMod,
				bool storePaths = true,
				bool storeStack = false, NetworKit::node target = NetworKit::none);
	void run() override;
private:
	std::function<NetworKit::edgeweight(NetworKit::edgeweight)> edgeModifier;
};

class ModAPSP: public NetworKit::APSP {
public:
	ModAPSP(const NetworKit::Graph&, std::function<NetworKit::edgeweight(NetworKit::edgeweight)>);
	void run() override;
private:
	std::function<NetworKit::edgeweight(NetworKit::edgeweight)> edgeModifier;
};

class CommTime {
public:
	CommTime(const NetworKit::Graph& pg);
	CommTime(const CommTime&);
	CommTime(CommTime&&) = default;
	CommTime& operator=(const CommTime&) = default;
	CommTime& operator=(CommTime&&) = default;
	~CommTime() = default;

	void run();
	bool hasRun() const;

	double time(NetworKit::node u, NetworKit::node v) const;
private:
	const NetworKit::Graph& graph;
	bool hasrun;

	ModAPSP apsp;
};

} // namespace

#endif

