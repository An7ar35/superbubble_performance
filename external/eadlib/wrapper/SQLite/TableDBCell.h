/**
    @class          eadlib::TableDBCell
    @brief          Cell data type for eadlib::TableDB class
    @dependencies   eadlib::logger::Logger
    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/
#ifndef EADLIB_TABLEDBCELL_H
#define EADLIB_TABLEDBCELL_H

#include <string>
#include <math.h>
#include <utility>

#include "eadlib/logger/Logger.h"

namespace eadlib {
    class TableDBCell {
      public:
        enum class DataType {
            NONE,
            BOOL,
            INT,
            DOUBLE,
            STRING
        };
        //Constructors/Destructor
        TableDBCell();
        TableDBCell( const TableDBCell &cell );
        TableDBCell( TableDBCell &&cell );
        TableDBCell( const bool &value );
        TableDBCell( const long long int &value );
        TableDBCell( const double &value );
        TableDBCell( const char *value ) : TableDBCell( std::string( value ) ) {};
        TableDBCell( const std::string &value );
        ~TableDBCell();
        //stream operator
        friend std::ostream &operator <<( std::ostream &out, const TableDBCell &cell );
        //assignment operators
        TableDBCell &operator =( const TableDBCell &item ); //copy
        TableDBCell &operator =( const bool &bool_item );
        TableDBCell &operator =( const long long int &int_item );
        TableDBCell &operator =( const double &double_item );
        TableDBCell &operator =( const std::string &string_item );
        //boolean evaluation operators
        bool operator <( const TableDBCell &rhs ) const;
        bool operator >( const TableDBCell &rhs ) const;
        bool operator <=( const TableDBCell &rhs ) const;
        bool operator >=( const TableDBCell &rhs ) const;
        bool operator ==( const TableDBCell &rhs ) const;
        bool operator !=( const TableDBCell &rhs ) const;
        //getters
        bool getBool() const;
        long long int getInt() const;
        double getDouble() const;
        std::string getString() const;
        DataType getType() const;
        //swap
        void swap( TableDBCell &cell );

      private:
        DataType _type;
        union {
            bool             _BOOL_val;
            signed long long _INT_val;
            double           _DOUBLE_val;
            std::string      _STRING_val;
        };
    };

    //-----------------------------------------------------------------------------------------------------------------
    // TableDBCell class public method implementations
    //-----------------------------------------------------------------------------------------------------------------
    /**
     * Default Constructor
     */
    inline TableDBCell::TableDBCell() :
        _type( DataType::NONE )
    {}

    /**
     * Copy Constructor
     * @param cell TableDBCell to copy over
     */
    inline TableDBCell::TableDBCell( const TableDBCell &cell ) {
        _type = cell._type;
        switch( cell._type ) {
            case DataType::NONE:
                break;
            case DataType::BOOL:
                _INT_val = cell._BOOL_val;
                break;
            case DataType::INT:
                _INT_val = cell._INT_val;
                break;
            case DataType::DOUBLE:
                _DOUBLE_val = cell._DOUBLE_val;
                break;
            case DataType::STRING:
                new ( &_STRING_val ) std::string { cell._STRING_val };
                break;
        }
    }

    /**
     * Move Constructor
     * @param cell TableDBCell to move over
     */
    inline TableDBCell::TableDBCell( TableDBCell &&cell ) {
        _type = cell._type;
        switch( cell._type ) {
            case DataType::NONE:
                break;
            case DataType::BOOL:
                _BOOL_val = cell._BOOL_val;
                break;
            case DataType::INT:
                _INT_val = cell._INT_val;
                break;
            case DataType::DOUBLE:
                _DOUBLE_val = cell._DOUBLE_val;
                break;
            case DataType::STRING:
                new ( &_STRING_val ) std::string { cell._STRING_val };
                break;
        }
    }

    /**
     * Constructor with value
     * @value Value of boolean type
     */
    inline TableDBCell::TableDBCell( const bool &value ) {
        _type = DataType::BOOL;
        _BOOL_val = value;
    }

    /**
     * Constructor with value
     * @value Value of integer type
     */
    inline TableDBCell::TableDBCell( const long long int &value ) {
        _type = DataType::INT;
        _INT_val = value;
    }

    /**
     * Constructor with value
     * @value Value of double type
     */
    inline TableDBCell::TableDBCell( const double &value ) {
        _type = DataType::DOUBLE;
        _DOUBLE_val = value;
    }

    /**
     * Constructor with value
     * @value Value of string type
     */
    inline TableDBCell::TableDBCell( const std::string &value ) {
        _type = DataType::STRING;
        new( &_STRING_val ) std::string { value };
    }

    /**
     * Destructor
     */
    inline TableDBCell::~TableDBCell() {
        if( _type == DataType::STRING ) {
            _STRING_val.~basic_string();
        }
    }

    /**
     * Output stream
     * @param out  Output stream
     * @param cell Cell to stream out
     * @return Output stream
     */
    inline std::ostream & operator <<( std::ostream &out, const TableDBCell &cell ) {
        switch( cell._type ) {
            case TableDBCell::DataType::NONE:
                out << std::string( "NULL" );
                break;
            case TableDBCell::DataType::BOOL:
                out << std::to_string( cell._BOOL_val );
                break;
            case TableDBCell::DataType::INT:
                out << std::to_string( cell._INT_val );
                break;
            case TableDBCell::DataType::DOUBLE:
                out << std::to_string( cell._DOUBLE_val );
                break;
            case TableDBCell::DataType::STRING:
                out << cell._STRING_val;
                break;
        }
        return out;
    }

    /*
     * Operator - assignment
     * @param item TableDBCell to assign
     */
    inline TableDBCell & TableDBCell::operator =( const TableDBCell &item ) {
        if( _type == DataType::STRING && item._type == DataType::STRING ) {
            _STRING_val = item._STRING_val;
            return *this;
        }
        if( _type == DataType::STRING ) {
            _STRING_val.~basic_string();
        }
        _type = item._type;
        switch( _type ) {
            case DataType::NONE:
                break;
            case DataType::BOOL:
                _BOOL_val = item._BOOL_val;
                break;
            case DataType::INT:
                _INT_val = item._INT_val;
                break;
            case DataType::DOUBLE:
                _DOUBLE_val = item._DOUBLE_val;
                break;
            case DataType::STRING:
                new ( &_STRING_val ) std::string { item._STRING_val };
                break;
        }
        return *this;
    }

    /**
     * Operator - assignment (boolean)
     * @param item Boolean value to assign
     */
    inline TableDBCell & TableDBCell::operator =( const bool &bool_item ) {
        switch( _type ) {
            case DataType::NONE:
                _type = DataType::BOOL;
                _INT_val = bool_item;
                break;
            case DataType::BOOL:
                _BOOL_val = bool_item;
                break;
            case DataType::INT:
                _INT_val = bool_item ? 1 : 0;
                break;
            case DataType::DOUBLE:
                _DOUBLE_val = bool_item ? 1 : 0;
                break;
            case DataType::STRING:
                _STRING_val.~basic_string();
                new ( &_STRING_val ) std::string { ( bool_item ? "TRUE" : "FALSE" ) };
                break;
        }
        return *this;
    }

    /**
     * Operator - assignment (integer)
     * @param item Integer value to assign
     */
    inline TableDBCell & TableDBCell::operator =( const long long int &int_item ) {
        switch( _type ) {
            case DataType::NONE:
                _type = DataType::INT;
                _INT_val = int_item;
                break;
            case DataType::BOOL:
                _BOOL_val = int_item != 0;
                break;
            case DataType::INT:
                _INT_val = int_item;
                break;
            case DataType::DOUBLE:
                _DOUBLE_val = double( int_item );
                break;
            case DataType::STRING:
                _STRING_val.~basic_string();
                new ( &_STRING_val ) std::string { std::to_string( int_item ) };
                break;
        }
        return *this;
    }

    /**
     * Operator - assignment (double)
     * @param item Double value to assign
     */
    inline TableDBCell & TableDBCell::operator =( const double &double_item ) {
        switch( _type ) {
            case DataType::NONE:
                _type = DataType::DOUBLE;
                _INT_val = int( double_item );
                break;
            case DataType::BOOL:
                _BOOL_val = double_item != 0;
                break;
            case DataType::INT:
                _INT_val = int( round( double_item ) );
                break;
            case DataType::DOUBLE:
                _DOUBLE_val = double_item;
                break;
            case DataType::STRING:
                _STRING_val.~basic_string();
                new ( &_STRING_val ) std::string { std::to_string( double_item ) };
                break;
        }
        return *this;
    }

    /**
     * Operator - assignment (string)
     * @param item String value to assign
     */
    inline TableDBCell &TableDBCell::operator =( const std::string &string_item ) {
        switch( _type ) {
            case DataType::NONE:
                _type = DataType::STRING;
                new ( &_STRING_val ) std::string { string_item };
                break;
            case DataType::STRING:
                _STRING_val.~basic_string();
                new ( &_STRING_val ) std::string { string_item };
                break;
            default:
                //TODO
                LOG_ERROR( "[eadlib::TableDBCell::operator=( string )] dropped to the default of switch (TODO)" );
                //"TableDBCell 'Operator=' [TableDBCell<>String] used but TableDBCell was not of compatible type so nothing is done.";
                break;
        }
        return *this;
    }

    /**
     * Operator - '<' comparison
     * @param rhs TableDBCell to compare to
     * @return Result of comparison
     */
    inline bool TableDBCell::operator <( const TableDBCell &rhs ) const {
        if( _type == rhs._type ) {
            switch( _type ) {
                case DataType::NONE:
                    return false;
                case DataType::BOOL:
                    return ( _BOOL_val < rhs._BOOL_val );
                case DataType::INT:
                    return ( _INT_val < rhs._INT_val );
                case DataType::DOUBLE:
                    return ( _DOUBLE_val < rhs._DOUBLE_val );
                case DataType::STRING:
                    return ( _STRING_val < rhs._STRING_val );
            }
        }
        return false;
    }

    /**
     * Operator - '>' comparison
     * @param rhs TableDBCell to compare to
     * @return Result of comparison
     */
    inline bool TableDBCell::operator >( const TableDBCell &rhs ) const {
        return !( operator <( rhs ) || operator ==( rhs ) );
    }

    /**
     * Operator - '<=' comparison
     * @param rhs TableDBCell to compare to
     * @return Result of comparison
     */
    inline bool TableDBCell::operator <=( const TableDBCell &rhs ) const {
        return !( operator >( rhs ) );
    }

    /**
     * Operator - '>=' comparison
     * @param rhs TableDBCell to compare to
     * @return Result of comparison
     */
    inline bool TableDBCell::operator >=( const TableDBCell &rhs ) const {
        return !( operator <( rhs ) );
    }

    /**
     * Operator - '==' comparison
     * @param rhs TableDBCell to compare to
     * @return Result of comparison
     */
    inline bool TableDBCell::operator ==( const TableDBCell &rhs ) const {
        if( _type == rhs._type ) {
            switch( _type ) {
                case DataType::NONE:
                    return true;
                case DataType::BOOL:
                    return ( _BOOL_val == rhs._BOOL_val );
                case DataType::INT:
                    return ( _INT_val == rhs._INT_val );
                case DataType::DOUBLE:
                    return ( _DOUBLE_val == rhs._DOUBLE_val );
                case DataType::STRING:
                    return ( _STRING_val == rhs._STRING_val );
            }
        }
        return false;
    }

    /**
     * Operator - '!=' comparison
     * @param rhs TableDBCell to compare to
     * @return Result of comparison
     */
    inline bool TableDBCell::operator !=( const TableDBCell &rhs ) const {
        return !( operator ==( rhs ) );
    }

    /**
     * Gets boolean value of TableDBCell
     * @return Boolean value
     */
    inline bool TableDBCell::getBool() const {
        switch( _type ) {
            case DataType::NONE:
                return false;
            case DataType::BOOL:
                return _BOOL_val;
            case DataType::INT:
                return _INT_val != 0;
            case DataType::DOUBLE:
                return _DOUBLE_val != 0;
            case DataType::STRING:
                return _STRING_val.length() > 0;
        }
    }

    /**
     * Gets integer value of TableDBCell
     * @return Integer value (length if string)
     */
    inline long long int TableDBCell::getInt() const {
        switch( _type ) {
            case DataType::NONE:
                return 0;
            case DataType::BOOL:
                return ( _BOOL_val ? 1 : 0 );
            case DataType::INT:
                return _INT_val;
            case DataType::DOUBLE:
                return int( round( _DOUBLE_val ) );
            case DataType::STRING:
                return _STRING_val.length();
        }
    }

    /**
     * Gets double value of TableDBCell
     * @return Double value
     */
    inline double TableDBCell::getDouble() const {
        switch( _type ) {
            case DataType::NONE:
                return 0;
            case DataType::BOOL:
                return ( _BOOL_val ? 1 : 0 );
            case DataType::INT:
                return double( _INT_val );
            case DataType::DOUBLE:
                return _DOUBLE_val;
            case DataType::STRING:
                return double( _STRING_val.length() );
        }
    }

    /**
     * Gets string value of TableDBCell
     * @return String value
     */
    inline std::string TableDBCell::getString() const {
        switch( _type ) {
            case DataType::NONE:
                return "";
            case DataType::BOOL:
                return ( _BOOL_val ? "TRUE" : "FALSE" );
            case DataType::INT:
                return std::to_string( _INT_val );
            case DataType::DOUBLE:
                return std::to_string( _DOUBLE_val );
            case DataType::STRING:
                return _STRING_val;
        }
    }

    /**
     * Gets TableDBCell's type
     * @return DataType
     */
    inline TableDBCell::DataType TableDBCell::getType() const {
        return _type;
    }

    /**
     * Swap implementation
     * @param cell TableDBCell to swap with
     */
    inline void TableDBCell::swap( eadlib::TableDBCell &cell ) {
        std::swap( *this, cell );
    }
}



#endif //EADLIB_TABLEDBCELL_H
