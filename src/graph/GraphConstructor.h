#ifndef SUPERBUBBLE_PERFORMANCE_GRAPHCONSTRUCTOR_H
#define SUPERBUBBLE_PERFORMANCE_GRAPHCONSTRUCTOR_H

#include <vector>
#include <string>

#include "eadlib/logger/Logger.h"
#include "eadlib/datastructure/WeightedGraph.h"

namespace sbp {
    namespace graph {
        class GraphConstructor {
          public:
            GraphConstructor( eadlib::WeightedGraph<std::string> &graph, const size_t &kmer_length );
            ~GraphConstructor();
            bool addToGraph( std::vector<char> &read );
            uint64_t getKmerCount();
            uint64_t getReadCount();
          private:
            eadlib::WeightedGraph<std::string> &_graph;
            size_t _kmer_length;
            uint64_t _kmer_processed;
            uint64_t _read_processed;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_GRAPHCONSTRUCTOR_H
