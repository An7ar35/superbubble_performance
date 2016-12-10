/**
    @class          eadlib::TableDBRow
    @brief          Row structure for eadlib::TableDB class
    @dependencies   eadlib::logger::Logger, eadlib::TableDBCell
    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/
#ifndef EADLIB_TABLEDBROW_H
#define EADLIB_TABLEDBROW_H

#include "../../logger/Logger.h"
#include "TableDBCell.h"

namespace eadlib {
    struct TableDBRow {
        TableDBCell & at( const size_t &column );
        const TableDBCell & at( const size_t &column ) const;
        void add();
        void add( const bool &value );
        void add( const signed long long &value );
        void add( const double &value );
        void add( const std::string &value );
        TableDBCell::DataType getItemType( const size_t &column ) const;
        size_t getLength() const;
      private:
        std::vector<TableDBCell> _content;
    };

    /**
     * Cell access
     * @param column Column index
     * @return Cell in row at specified column index
     * @throws std::out_of_range when column index is not within the width of the row
     */
    inline TableDBCell &TableDBRow::at( const size_t &column ) {
        if( column >= _content.size() ) {
            LOG_ERROR( "[eadlib::TableDBRow::at( ", column, " )] index out of range" );
            throw std::out_of_range( "eadlib::TableDBRow::at() : index out of range" );
        }
        return _content.at( column );
    }

    /**
     * Cell access
     * @param column Column index
     * @return Cell in row at specified column index
     * @throws std::out_of_range when column index is not within the width of the row
     */
    inline const TableDBCell &TableDBRow::at( const size_t &column ) const {
        if( column >= _content.size() ) {
            LOG_ERROR( "[eadlib::TableDBRow::at( ", column, " )] index out of range" );
            throw std::out_of_range( "eadlib::TableDBRow::at() : index out of range" );
        }
        return _content.at( column );
    }

    /**
     * Adds a NULL cell to the Row
     * @param value
     */
    inline void TableDBRow::add() {
        _content.push_back( TableDBCell {} );
    }

    /**
     * Adds value to the Row
     * @param value
     */
    inline void TableDBRow::add( const bool &value ) {
        _content.push_back( TableDBCell { value } );
    }

    /**
     * Adds value to the Row
     * @param value
     */
    inline void TableDBRow::add( const signed long long &value ) {
        _content.push_back( TableDBCell { value } );
    }

    /**
     * Adds value to the Row
     * @param value
     */
    inline void TableDBRow::add( const double &value ) {
        _content.push_back( TableDBCell { value } );
    }

    /**
     * Adds value to the Row
     * @param value
     */
    inline void TableDBRow::add( const std::string &value ) {
        _content.push_back( TableDBCell { value } );
    }

    /**
     * Gets the DataType of an item in the row
     * @param column Column in which the Item belongs to
     * @return The type
     */
    inline TableDBCell::DataType TableDBRow::getItemType( const size_t &column ) const {
        if( column < _content.size() ) {
            return _content[ column ].getType();
        } else {
            return TableDBCell::DataType::NONE;
        }
    }

    /**
     * Gets the length (width) of the Row
     * @return Length
     */
    inline size_t TableDBRow::getLength() const {
        return _content.size();
    }
}

#endif //EADLIB_TABLEDBROW_H
