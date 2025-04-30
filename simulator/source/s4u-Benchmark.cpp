#define uses_CBenchmark
#define uses_string_format
#define uses_XBT_INFO
#define uses_CNFG
#define uses_Global_config

#include "../inc/dep"

XBT_LOG_NEW_DEFAULT_CATEGORY( CBenchmark , "Messages specific for this s4u example" );

// BenchmarkIteration

void BenchmarkIteration::startIteration()
{
	//cout << "startIteration config_index " << d_config_index << endl;
	//cout << "startIteration iteration_index " << d_iteration_index << endl;

	LOG_TR( "BEGIN TRY" , "" );
	LOG_TR( "BEGIN TRANSACTION ITERATIONN" , "" );

	init_log_file( d_iteration_index );
	SETTINGS( d_config_index ); // load config

	//SECTION_ALIVE_CHECK("");

	SETTINGS_FAST()->d_app_mode = AeAppMode::apmd_kernel_mode;

	//XBT_INFO_EMPTY_LINE();
	XBT_INFO( "#(%d) startIteration %d on config(%d)" , __LINE__ , d_iteration_index , d_config_index );

	//LOG_TR( "----------------------------------------------------------------" , "" );
	LOG_TR( "if ( ( select count( * ) from [SimGrid].[dbo].[Config] where [name] = N'%s' ) = 0 ) begin INSERT INTO [SimGrid].[dbo].[Config]( [name] , [enable] ) VALUES( N'%s' , 1 ) end" , SETTINGS_FAST()->d_config_name.c_str() , SETTINGS_FAST()->d_config_name.c_str() );
	LOG_TR( "SELECT @config_id = [cnf_id] FROM [SimGrid].[dbo].[Config] where name = N'%s' and enable = 1" , SETTINGS_FAST()->d_config_name.c_str() );

	LOG_TR( "INSERT INTO [SimGrid].[dbo].[Config_iteration_pack]( [itrpc_id] , [cnf_id] ) VALUES( @iteration_pack_id , @config_id )" , "" );
	LOG_TR( "set @config_iteration_pack_id = SCOPE_IDENTITY()" , "" );

	if ( !SETTINGS_FAST()->d_Mode_MAKE_DAX2_FOR_SIMULATION )
	{
		pVMM->resumeIteration(); // ready to prepare vm for DGG
	}
	pDGG->resumeIteration(); // load dags and put it in queue

	std::time( &SETTINGS_FAST()->d_post_initialize_system_time );

	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose , "#(%d) ~startIteration %d" , __LINE__ , d_iteration_index );

	//cout << "~startIteration config_index " << d_config_index << endl;
	//cout << "~startIteration iteration_index " << d_iteration_index << endl;
}

void BenchmarkIteration::IterationFinished( bool b_last_iteration )
{
	//cout << "IterationFinished config_index " << d_config_index << endl;
	//cout << "IterationFinished iteration_index " << d_iteration_index << endl;

	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) IterationFinished %d" , __LINE__ , d_iteration_index );

	if ( SETTINGS_FAST()->d_dump_each_simulation_iteration_result )
	{
		if ( !SETTINGS_FAST()->d_Mode_MAKE_DAX2_FOR_SIMULATION )
		{
			pVMM->flush_cache();
		}
		pDGG->flush_cache();
	}

	// DUMP
	if ( SETTINGS_FAST()->d_dump_each_simulation_iteration_result )
	{
		if ( !SETTINGS_FAST()->d_Mode_MAKE_DAX2_FOR_SIMULATION )
		{
			pVMM->dump();
		}
		pDGG->dump();
	}

	// release unneccessary data
	if ( !SETTINGS_FAST()->d_Mode_MAKE_DAX2_FOR_SIMULATION )
	{
		pVMM->IterationFinished( b_last_iteration );
	}
	pDGG->IterationFinished( b_last_iteration );

	if ( !SETTINGS_FAST()->d_tags.empty() )
	{
		XBT_INFO_EMPTY_LINE();
		XBT_INFO( "config tags=%s" , SETTINGS_FAST()->d_tags.c_str() );
	}

	time_t clock_end;
	std::time( &clock_end );
	XBT_INFO( "executed iteration system duration %.2lf" , difftime( clock_end , SETTINGS_FAST()->d_post_initialize_system_time ) );

	XBT_INFO_EMPTY_LINE();
	XBT_INFO( "%s" , SETTINGS_FAST()->echo_config( false ).c_str() );

	//LOG_TR( "INSERT INTO [dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval],[decval]) VALUES()" , "" );
	//LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval]) VALUES(@config_iteration_pack_id,'tags','%s')" , SETTINGS_FAST()->d_tags.c_str() );
	LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[relval]) VALUES(@config_iteration_pack_id,'iteration system duration',%.2lf)" , difftime( clock_end , SETTINGS_FAST()->d_post_initialize_system_time ) );
	LOG_TR( "UPDATE [dbo].[Config_iteration_pack] SET [cnf_tags] = '%s' WHERE [cnfitrpc_id] = @config_iteration_pack_id" , SETTINGS_FAST()->d_tags.c_str() );
	
	LOG_TR( "COMMIT TRANSACTION ITERATIONN" , "" );
	LOG_TR( "END TRY" , "" );
	LOG_TR( "BEGIN CATCH" , "" );
	LOG_TR( "IF ( @@TRANCOUNT > 0 )" , "" );
	LOG_TR( "BEGIN" , "" );
	LOG_TR( "ROLLBACK TRANSACTION ITERATIONN" , "" );
	LOG_TR( "PRINT 'rollbacked'" , "" );
	LOG_TR( "END" , "" );
	LOG_TR( "SELECT" , "" );
	LOG_TR( "ERROR_NUMBER() AS ErrorNumber ," , "" );
	LOG_TR( "ERROR_SEVERITY() AS ErrorSeverity ," , "" );
	LOG_TR( "ERROR_STATE() AS ErrorState ," , "" );
	LOG_TR( "ERROR_PROCEDURE() AS ErrorProcedure ," , "" );
	LOG_TR( "ERROR_LINE() AS ErrorLine ," , "" );
	LOG_TR( "ERROR_MESSAGE() AS ErrorMessage" , "" );
	LOG_TR( "END CATCH" , "" );

	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose , "#(%d) ~IterationFinished %d" , __LINE__ , d_iteration_index );

	SETTINGS_FAST()->closeConfig();
	init_log_file( d_iteration_index + 1 );

	//cout << "~IterationFinished config_index " << d_config_index << endl;
	//cout << "~IterationFinished iteration_index " << d_iteration_index << endl;
}

void BenchmarkIteration::cleanup_last_iteration()
{
	pVMM->cleanup_on_last_benchmark_iteration();
}

void BenchmarkIteration::init_iteration( sg4::NetZone * pZone )
{
	//SETTINGS( d_config_index ); // load config

	if ( !SETTINGS_FAST()->d_Mode_MAKE_DAX2_FOR_SIMULATION )
	{
		pVMM->init_VManagement( pZone , d_iteration_index , pPrevIteration == NULL ? NULL : pPrevIteration->pVMM.get() );
		pDGG->init_DAGenerator( pZone , d_iteration_index );
	}
}

// CBenchmark

shared_ptr< BenchmarkIteration > CBenchmark::d_current_benchmark = NULL;

CBenchmark::CBenchmark()
{
	d_current_benchmark = NULL;

	init_logTr_file();

	//LOG_TR( "BEGIN TRY" , "" );
	//LOG_TR( "BEGIN TRANSACTION ITERATIONN" , "" );
	
	LOG_TR( "USE [SimGrid]" , "" );
	LOG_TR( "declare @iteration_pack_id as bigint" , "" );
	LOG_TR( "declare @config_id as bigint" , "" );
	LOG_TR( "declare @config_iteration_pack_id as bigint" , "" );
	LOG_TR( "INSERT INTO [SimGrid].[dbo].[Iteration_pack] DEFAULT VALUES" , "" );
	LOG_TR( "set @iteration_pack_id = SCOPE_IDENTITY()" , "" );

	VMachine_Small small_vm( true ); // just instance for declare vm type for system
	VMachine_Medium medium_vm( true );
	VMachine_Large large_vm( true );
	VMachine_ExtraLarge extralarge_vm( true );

	d_pZone = sg4::create_full_zone( "Zone1" );
}

CBenchmark::~CBenchmark()
{
	d_current_benchmark = NULL;

	finish_logTr_file();
}

void CBenchmark::add_one_iteration( int config_index )
{
	SETTINGS( config_index ); // load config

	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) add_one_iteration" , __LINE__ );
	for ( int i = 0 ; i < SETTINGS_FAST()->d_repeat_each_config ; i++ )
	{
		shared_ptr< BenchmarkIteration > iteration = make_shared<BenchmarkIteration>();
		iteration->pBenchmark = this;

		iteration->pVMM = make_shared<Vmm>();
		iteration->pDGG = make_shared<DGG>();

		iteration->pVMM->d_pIteration = iteration.get();
		iteration->pDGG->d_pIteration = iteration.get();

		if ( d_iterations.size() > 0 )
		{
			d_iterations.back()->pNextIteration = iteration; // last one
			iteration->pPrevIteration = d_iterations.back();
		}
		d_iterations.push_back( iteration );
		iteration->d_iteration_index = d_iterations.size();
		iteration->d_config_index = config_index;

		iteration->init_iteration( d_pZone );
	}
	
	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose , "#(%d) ~add_one_iteration" , __LINE__ );
}

void CBenchmark::startNextIteration( DGG * pFinishedDGG )
{
	if ( pFinishedDGG == NULL )
	{
		assert( d_iterations.size() > 0 );
		if ( d_iterations.size() > 0 )
		{
			d_current_benchmark = d_iterations.front();
			d_iterations.front()->startIteration(); // first one
		}
	}
	else
	{
		for ( auto & iter : d_iterations )
		{
			if ( iter != NULL && iter->pDGG != NULL && iter->pDGG->d_DGG_Id == pFinishedDGG->d_DGG_Id )
			{
				iter->IterationFinished( iter->pNextIteration == NULL );
				if ( iter->pNextIteration != NULL )
				{
					d_current_benchmark = iter->pNextIteration;
					iter->pNextIteration->startIteration();
					iter->pNextIteration->pPrevIteration = NULL;
					iter = NULL;
				}
				else
				{
					iter->cleanup_last_iteration();
				}
				break;
			}
		}
	}
}

