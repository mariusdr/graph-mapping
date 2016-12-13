#ifndef STATIC_MAPPING_IO_UTIL_H
#define STATIC_MAPPING_IO_UTIL_H

#include <string>
#include <vector>

#include "Benchmark.h"

namespace StaticMapping {
namespace Util {

class Writer {
public:
	explicit Writer(std::string filename);
	void bufferItem(std::string&& item);
	virtual void write();

protected:
	const std::vector<std::string>& getBuffer();
	std::ofstream& getStream();

private:
	std::vector<std::string> bufferedItems;
	std::string filename;
	std::ofstream stream;
};

class MappingWriter: public Writer {
public:
	explicit MappingWriter(std::string filename);
	void bufferItem(std::vector<size_t> mapping);
};

class BenchmarkWriter: public Writer {
public:
	explicit BenchmarkWriter(std::string filename);
	void bufferItem(BenchmarkResults& results, std::string algoName);
	void write() override;
private:
	std::string filename;
};


}} // namespace


#endif
