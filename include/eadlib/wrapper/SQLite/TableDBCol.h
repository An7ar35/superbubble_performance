/**
    @class          eadlib::TableDBCol
    @brief          Column structure for eadlib::TableDB class
    @dependencies   eadlib::logger::Logger, eadlib::TableDBCell
    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/
#ifndef EADLIB_TABLEDBCOL_H
#define EADLIB_TABLEDBCOL_H

#include "../../logger/Logger.h"
#include "TableDBCell.h"

namespace eadlib {
    struct TableDBCol {
        TableDBCol( TableDBCell::DataType type, std::string heading ) : _type( type ), _heading( heading ) {};
        TableDBCell::DataType _type;
        std::string _heading;
    };
}

#endif //EADLIB_TABLEDBCOL_H
