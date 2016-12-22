#ifndef SUPERBUBBLE_PERFORMANCE_PIPELINERUNNER_H
#define SUPERBUBBLE_PERFORMANCE_PIPELINERUNNER_H

#include <eadlib/io/FileReader.h>
#include <eadlib/io/FileWriter.h>
#include <eadlib/datastructure/WeightedGraph.h>

#include "io/FastaParser.h"
#include "io/DotExport.h"
#include "io/Database.h"
#include "graph/GraphConstructor.h"
#include "graph/GraphIndexer.h"
#include "algorithm/GraphCompressor.h"
#include "algorithm/Tarjan.h"
#include "algorithm/superbubble/SB_Driver.h"
#include "algorithm/superbubble/container/SuperBubble.h"

namespace sbp {
    struct PipelineRunner {
        void loadFASTA( const std::string &fasta_file_path, const size_t &kmer_size, eadlib::WeightedGraph<std::string> &graph );
        void compressGraph( eadlib::WeightedGraph<std::string> &graph );
        void exportToDot( const std::string &file_name, eadlib::WeightedGraph<std::string> &graph );
        void exportToDot( const std::string &file_name, eadlib::WeightedGraph<size_t> &graph );
        void exportToDB( const std::string &db_file_name, eadlib::WeightedGraph<std::string> &graph );
        void importFromDB( const std::string &db_file_name, eadlib::WeightedGraph<size_t> &graph );
        void importFromDB( const std::string &db_file_name, eadlib::WeightedGraph<std::string> &graph );
        void runSuperbubble( const eadlib::WeightedGraph<size_t> &graph );
    };
}

#endif //SUPERBUBBLE_PERFORMANCE_PIPELINERUNNER_H
