#include "IOUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <typeinfo>

#include "GreedyMapping.h"
#include "InitialMapping.h"
#include "RandomMapping.h"
#include "WeightedGreedyMapping.h"

namespace StaticMapping {
namespace Util {

Writer::Writer(std::string filename):
	bufferedItems(std::vector<std::string>()),
	filename(filename),
	stream(std::ofstream())
{}

void Writer::bufferItem(std::string&& item) {
	bufferedItems.push_back(item);
}

void Writer::write() {
	stream.open(filename);
	for (auto item: bufferedItems)
		stream << item;
	stream.close();
}

const std::vector<std::string>& Writer::getBuffer() {
	return bufferedItems;
}

std::ofstream& Writer::getStream() {
	return stream;
}

std::string prettifyAlgoName(std::string oldname) {
	if (oldname == typeid(GreedyMapping).name()) {
		return "GreedyMapping";
	}
	if (oldname == typeid(InitialMapping).name()) {
		return "InitialMapping";
	}
	if (oldname == typeid(RandomMapping).name()) {
		return "RandomMapping";
	}
	if (oldname == typeid(WeightedGreedyMapping).name()) {
		return "WeightedGreedyMapping";
	}
	throw std::runtime_error("cant lookup algo name:" + oldname);
}

MappingWriter::MappingWriter(std::string filename):
	Writer(filename)
{}

void MappingWriter::bufferItem(std::vector<size_t> mapping) {
	std::stringstream sstr;

	for (size_t i = 0; i < mapping.size() - 1; i++) {
		sstr << "(" << i << ", " << mapping[i] << ")";
		sstr << ", \n";
	}
	auto n = mapping.size() - 1;
	sstr << "(" << n << ", " << mapping[n] << ") \n";

	std::string s = sstr.str();
	Writer::bufferItem(std::move(s));
}

BenchmarkWriter::BenchmarkWriter(std::string filename):
	Writer(filename),
	filename(filename)
{}

void BenchmarkWriter::bufferItem(BenchmarkResults& results, std::string algoName) {
	std::stringstream sstr;
	sstr << "\"Results_" << prettifyAlgoName(algoName) << "\": { \n";
	sstr << "    \"runtime\": " << results.runtime << ", \n";
	sstr << "    \"avgDilation\": " << results.avgDilation << ", \n";
	sstr << "    \"maxDilation\": " << results.maxDilation << ", \n";
	sstr << "    \"maxCongestion\": " << results.maxCongestion << " \n";
	sstr << "}";

	std::string s = sstr.str();
	Writer::bufferItem(std::move(s));
}

void BenchmarkWriter::write() {
	auto& buffer = Writer::getBuffer();
	auto& stream = Writer::getStream();
	stream.open(filename);
		
	size_t n = buffer.size() - 1;
	for (size_t i = 0; i < n; i++) {
		stream << buffer[i] << ", \n";
	}
	stream << buffer[n] << "\n";
	stream.close();
}


}} // namespace
