
#define uses_string_format
#define uses_getline
#define uses_cout
#define uses_XBT_INFO
#define uses_log
#define uses_abs

#include "../inc/dep"

void ltrim( std::string & s )
{
    BEGIN_TRY
    s.erase( s.begin() , std::find_if( s.begin() , s.end() , []( unsigned char ch )
    {
        return !std::isspace( ch );
    } ) );
    CATCH_TRY
}

// trim from end (in place)
void rtrim( std::string & s )
{
    BEGIN_TRY
    s.erase( std::find_if( s.rbegin() , s.rend() , []( unsigned char ch )
    {
        return !std::isspace( ch );
    } ).base() , s.end() );
    CATCH_TRY
}

// trim from both ends (in place)
void trim( std::string & s )
{
    BEGIN_TRY
    rtrim( s );
    ltrim( s );
    CATCH_TRY
}

// trim from start (copying)
std::string ltrim_copy( std::string s )
{
    BEGIN_TRY
    ltrim( s );
    CATCH_TRY
    return s;
}

// trim from end (copying)
std::string rtrim_copy( std::string s )
{
    BEGIN_TRY
    rtrim( s );
    CATCH_TRY
    return s;
}

// trim from both ends (copying)
std::string trim_copy( std::string s )
{
    BEGIN_TRY
    trim( s );
    CATCH_TRY
    return s;
}

bool is_number( const std::string & s )
{
    BEGIN_TRY
    return !s.empty() && std::find_if( s.begin() ,
                                       s.end() , []( unsigned char c )
    {
        return !std::isdigit( c );
    } ) == s.end();
    CATCH_TRY
    return false;
}

std::string ReplaceAll( std::string & str , const std::string & from , const std::string & to )
{
    BEGIN_TRY
    size_t start_pos = 0;
    while ( ( start_pos = str.find( from , start_pos ) ) != std::string::npos )
    {
        str.replace( start_pos , from.length() , to );
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    CATCH_TRY
    return str;
}

bool try_read_token( stringstream & line_token , const char * tried_token , string & readed_token , char escapechar , bool bassert )
{
    BEGIN_TRY
    readed_token = "";
    std::getline( line_token , readed_token , escapechar ); // [dagcount
    readed_token = trim_copy( readed_token );
    if ( readed_token.rfind( tried_token , 0 ) != 0 )
    {
        if ( bassert )
        {
            assert( 0 );
            XBT_INFO( "assert" , "" );

            //cout << "assert" << endl;
        }
        return false;
    }
    return true;
    CATCH_TRY
    return false;
}

void StrTokenizer( string & source , const char * delimiter , vector<string> & Tokens )
{
    BEGIN_TRY
    size_t new_index = 0;
    size_t old_index = 0;

    while ( new_index != std::string::npos )
    {
        new_index = source.find( delimiter , old_index );
        Tokens.emplace_back( source.substr( old_index , new_index - old_index ) );

        if ( new_index != std::string::npos )
            old_index = ++new_index;
    }
    CATCH_TRY
}

string trim_floatnumber( double fn )
{
    BEGIN_TRY
    string ss = std::to_string( fn );
    while ( ss.length() > 0 && ss.find( '.' ) < ss.length() && ( ss.back() == '0' || ss.back() == '.' ) )
    {
        ss.pop_back();
    }

    vector<string> strs;
    StrTokenizer( ss , "." , strs );

    string dec_number = strs[ 0 ];
    string float_number;
    if ( strs.size() > 1 ) float_number = strs[ 1 ];

    ss = "";
    auto count = 3;
    for ( auto i = dec_number.crbegin() ; i != dec_number.crend() ; ++i )
    {
        if ( count == 0 )
        {
            ss.push_back( ',' );
            count = 3;
        }
        if ( count-- )
        {
            ss.push_back( *i );
        }
    }
    std::reverse( ss.begin() , ss.end() );

    if ( strs.size() > 1 ) ss += "." + float_number;

    return ss;
    CATCH_TRY
    return string( "" );
}

double nextTime( double rateParameter )
{
    return -log( 1.0 - ( double )rand() / ( RAND_MAX + 1.0 ) ) / rateParameter; // https://preshing.com/20111007/how-to-generate-random-timings-for-a-poisson-process/
}

double nonzero_in_range( double value , double zeroranje )
{
    if ( fabs( value ) < zeroranje )
    {
        if ( value >= 0.0 ) return zeroranje;
        if ( value < 0.0 ) return -zeroranje;
        return zeroranje;
    }
    return value;
}

string get_time( const char * style )
{
    time_t timer;
    char buffer[ 50 ];
    memset( buffer , 0 , sizeof( buffer ) );
    struct tm * tm_info;
    timer = std::time( NULL );
    tm_info = localtime( &timer );
    strftime( buffer , 26 , style , tm_info );
    return string_format( "%s" , buffer );
}
