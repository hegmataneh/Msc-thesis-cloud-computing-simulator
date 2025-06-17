// aaa 22

#define uses_CBenchmark
#define uses_LOG
#define uses_random
#define uses_CNFG

#include "./inc/dep"

XBT_LOG_NEW_DEFAULT_CATEGORY( tst , "Messages specific for this s4u example" );


int main( int argc , char* argv[] )
{
    //std::cout << sizeof( int ) << endl; // 4
    //std::cout << sizeof(char *) << endl; // 8
    //std::cout << sizeof(double) << endl; // 8
    //std::cout << sizeof(long double) << endl; // 16
    //std::cout << sizeof(short) << endl; // 2
    //std::cout << sizeof(float) << endl; // 4
    //std::cout << sizeof(int64_t) << endl; // 8
    //std::cout << sizeof(vector<int>) << endl; // 24
    //std::cout << sizeof(list<int>) << endl; // 24
    //std::cout << sizeof(map<int,int>) << endl; // 48
    //assert( sizeof(int) == 4 );
    //assert( sizeof(char *) == 8 );
    //assert( sizeof(double) == 8 );
    //assert( sizeof(long double) == 16 );
    //assert( sizeof(short) == 2 );
    //assert( sizeof(float) == 4 );
    //assert( sizeof(int64_t) == 8 );
    //std::cout << sizeof(eDAG_ID::first_dag_id) << endl; // 4
  
    assert( argc == 1 || argc == 2 );
    cout << "#" << __LINE__ << " " << "version" << "1" << endl;
    cout << "#" << __LINE__ << " " << "getenv_ " << getenv("_") << endl;
    if ( argc == 2 ) // sample -> /mnt/c/simgrid/simgrid-3.32/out/build/WSL-GCC-Debug
    {
        cout << "#" << __LINE__ << " " << "app commandline " << argv[0] << "  " << argv[1] << endl;
        init_log_file( 0 , argv[1] ); // initialized by non default results root
    }
    else
    {
        cout << "#" << __LINE__ << " " << "app commandline " << argv[ 0 ] << "  default config" << endl;

        string fname = string_format( "%s" , getenv("_") );
        size_t pos = fname.find_last_of("\\/");
        string config_path = (std::string::npos == pos)
            ? "."
            : fname.substr(0, pos);

        init_log_file( 0 , string_format( "%s" , getenv("_") ) == "/usr/bin/gdb" ? __DEFAULT_CONFIG_PACKAGE_ROOT : config_path.c_str() );
    }
    SETTINGS( 1 ); // load first config
    sg4::Engine e( &argc , argv );
    XBT_INFO( "#(%d) begin kernel mode" , __LINE__ );

    if ( SETTINGS_FAST()->d_Mode_MAKE_DAX2_FOR_SIMULATION )
    {
        assert( SETTINGS_FAST()->d_config_count == 1 );
        assert( SETTINGS_FAST()->d_repeat_each_config == 1 );
    }

    time_t clock_sta;
    std::time( &clock_sta );
    simgrid::xbt::random::set_mersenne_seed( std::time( NULL ) );
    srand( std::time( NULL ) );

    XBT_INFO_EMPTY_LINE();
    XBT_INFO( "#(%d) add benchmarks" , __LINE__ );
    CBenchmark benchmarks;
    if ( SETTINGS_FAST()->d_init_Vmm_DGG )
    {
        for ( int config_index = 1 ; config_index <= SETTINGS_FAST()->d_config_count ; config_index++ )
        {
            benchmarks.add_one_iteration( config_index );
        }

        benchmarks.startNextIteration( NULL );
    }

    ////int _sort_node_policy_ver[] = { 2 , 1 };
    ////bool _use_wait_policy[] = { true , false };
    ////bool _break_after_bundle_not_suitable[] = { true , false };
    ////bool _use_least_price_vm_to_lease_new_vm[] = { true , false };
    ////int _quality_ratio_calculation_ver[] = { 2 , 1 };
    ////int iconfig = 0;
    ////for ( int i1 = 0 ; i1 < SIZEOF( _sort_node_policy_ver ) ; i1++ )
    ////{
    ////    for ( int i2 = 0 ; i2 < SIZEOF( _use_wait_policy ) ; i2++ )
    ////    {
    ////        for ( int i3 = 0 ; i3 < SIZEOF( _break_after_bundle_not_suitable ) ; i3++ )
    ////        {
    ////            for ( int i4 = 0 ; i4 < SIZEOF( _use_least_price_vm_to_lease_new_vm ) ; i4++ )
    ////            {
    ////                for ( int i5 = 0 ; i5 < SIZEOF( _quality_ratio_calculation_ver ) ; i5++ )
    ////                {
    ////                    iconfig++;
    ////                    SETTINGS( iconfig == 1 ? 1 : iconfig - 1 );
    ////                    SETTINGS_FAST()->d_simulationIteration = iconfig;
    ////                    SETTINGS_FAST()->d_tags = "";
    ////                    SETTINGS_FAST()->d_config_count = (int)pow( 2 , 5 );
    ////                    SETTINGS_FAST()->d_config_name = string_format( "%03d" , iconfig );
    ////                    SETTINGS_FAST()->d_sort_node_policy_ver = _sort_node_policy_ver[i1];
    ////                    SETTINGS_FAST()->d_use_wait_policy = _use_wait_policy[i2];
    ////                    SETTINGS_FAST()->d_break_after_bundle_not_suitable = _break_after_bundle_not_suitable[i3];
    ////                    SETTINGS_FAST()->d_use_least_price_vm_to_lease_new_vm = _use_least_price_vm_to_lease_new_vm[i4];
    ////                    SETTINGS_FAST()->d_quality_ratio_calculation_ver = _quality_ratio_calculation_ver[i5];
    ////                    SETTINGS_FAST()->make_config_tags();
    ////                    SETTINGS_FAST()->closeConfig();
    ////                }
    ////            }
    ////        }
    ////    }
    ////}

    SETTINGS( 1 ); // continue to config 1
    XBT_INFO_ALIVE_CHECK( "#(%d) begin simulation mode" , __LINE__ );
    if ( SETTINGS_FAST()->d_app_mode < AeAppMode::apmd_simulation_mode )
    {
        SETTINGS_FAST()->d_app_mode = AeAppMode::apmd_simulation_mode;
    }
    e.run();

    map< string , int > count_address;
    for ( auto & row : Global_config::d_allocation_counter )
    {
        if ( std::get<2>( row.second ) != 0 )
        {
            count_address[ string_format( "%s-%d" , std::get<0>( row.second ) , std::get<1>( row.second ) ) ] += std::get<2>( row.second );
        }
    }
    for ( auto & row : count_address )
    {
        cout << "mem leak " << row.first << " count " << row.second << endl;
    }

    

    SETTINGS( 1 );
    time_t clock_end;
    std::time( &clock_end );

    XBT_INFO_EMPTY_LINE();
    XBT_INFO( "executed system duration %.2lf" , difftime( clock_end , clock_sta ) );
    XBT_INFO( "buyyeeee " , "" );

    SETTINGS( 1 )->closeConfig();
    finish_log_file();
    return 0;
}

