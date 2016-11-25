#ifndef SUPERBUBBLE_PERFORMANCE_FASTAPARSER_H
#define SUPERBUBBLE_PERFORMANCE_FASTAPARSER_H

#include <string>
#include "eadlib/logger/Logger.h"
#include "eadlib/io/FileReader.h"

namespace sbp {
    namespace io {
        /**
         * @brief   Return states enum for the sb::io::FastaParser
         */
        enum class FastaParserState {
            FILE_ERROR,
            PARSER_ERROR,
            DESC_PARSED,
            READ_PARSED,
            EOF_REACHED
        };

        /**
         * @brief   FASTA file parser
         */
        class FastaParser {
          public:
            FastaParser( eadlib::io::FileReader &reader );
            ~FastaParser();
            FastaParserState parse( std::vector<char> &buffer );
          private:
            enum class PeekResult { NEW_LINE, DESCRIPTION, READ, EMPTY, ERROR };
            PeekResult peek();
            eadlib::io::FileReader & _reader;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_FASTAPARSER_H
