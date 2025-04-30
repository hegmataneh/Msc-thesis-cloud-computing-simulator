#define uses_string_format
#define uses_random
#define uses_DAGGenerator
#define uses_LOG
#define uses_CNFG
#define uses_min
#define uses_CBenchmark

#include "../inc/dep"

XBT_LOG_NEW_DEFAULT_CATEGORY( DGG , "Messages specific for this s4u example" );

DGG::DGG()
{
	static int _id = 0;
	d_DGG_Id = ++_id;

	d_resume_iteration = NULL;

	d_finished_dag_count = 0;

	d_pDGGeneratorHost = NULL;
	d_pDGGeneratorActor = NULL;

	d_pDGSchdulerHost = NULL;
	d_pDGSchdulerActor = NULL;

	d_pEstimateHost = NULL;
	d_pEstimateActor = NULL;

	d_sort_iteration = -1;
	//d_in_each_sort_try_schedule = -1;
	//d_in_each_sort_try_schedule_max = -1;

	d_total_proccess_node_edge = 0;
	d_proccessed_node_edge = 0;
}

void DGG::flush_cache()
{

}

void DGG::init_DAGenerator( sg4::NetZone * srcZoneRoot , int simulationIteration )
{
	BEGIN_TRY
	if ( !SETTINGS_FAST()->d_Mode_MAKE_DAX2_FOR_SIMULATION )
	{
		d_pDGGeneratorHost = srcZoneRoot->create_host( string_format( "hDGG%d" , simulationIteration ) , SETTINGS_FAST()->d_DAG_GENERATOR_HOST_SPEED );
		std::function<void()> f_callback_actor_dag_generator = std::bind( &DGG::callback_actor_dag_generator , this );
		d_pDGGeneratorActor = sg4::Actor::create( string_format( "aDGG%d" , simulationIteration ) , d_pDGGeneratorHost , f_callback_actor_dag_generator );
				
		d_pDGSchdulerHost = srcZoneRoot->create_host( string_format( "hDGS%d" , simulationIteration ) , SETTINGS_FAST()->d_DAG_GENERATOR_HOST_SPEED );
		std::function<void()> f_callback_actor_scheduler = std::bind( &DGG::callback_actor_scheduler , this );
		d_pDGSchdulerActor = sg4::Actor::create( string_format( "aDGS%d" , simulationIteration ) , d_pDGSchdulerHost , f_callback_actor_scheduler );

		if ( SETTINGS_FAST()->d_run_estimator )
		{
			d_pEstimateHost = srcZoneRoot->create_host( string_format( "hDGE%d" , simulationIteration ) , SETTINGS_FAST()->d_DAG_GENERATOR_HOST_SPEED );
			std::function<void()> f_callback_actor_estimator = std::bind( &DGG::callback_actor_estimator , this );
			d_pEstimateActor = sg4::Actor::create( string_format( "aDGE%d" , simulationIteration ) , d_pEstimateHost , f_callback_actor_estimator );
		}
	}
	CATCH_TRY
}

void DGG::init_DAGStorage()
{
	BEGIN_TRY
		SECTION_ALIVE_CHECK("" );

		// init dag storage here
		if ( SETTINGS_FAST()->d_Mode_MAKE_DAX2_FOR_SIMULATION )
		{
			//load_backup_dag_guid(); // comment this

			init_xml_dag_guid_config_to_write();

			

			//load_backup_dag_guid();
			//d_dag_storage.read_and_convert_dax2_to_dags();
			//XBT_INFO( "#(%d) " , __LINE__ );
			d_dag_storage.read_and_convert_dax1_to_dags();
			////d_dag_storage.convert_dags_to_dax2();

			finish_xml_dag_guid_config_for_write();

		}
		else
		{
			if ( SETTINGS_FAST()->d_notify_dag_loaded )
			{
				XBT_INFO( "#(%d) begin add dags. this may take along time" , __LINE__ );
			}
			if ( SETTINGS_FAST()->d_add_dax2_graph )
			{
				assert( SETTINGS_FAST()->d_dax1_file_path.empty() ^ SETTINGS_FAST()->d_dax2_file_path.empty() );

				if ( !SETTINGS_FAST()->d_dax1_file_path.empty() )
				{
					{
						SECTION_ALIVE_CHECK( "" );
						read_xml_dag_guid_config( false );
					}
					d_dag_storage.read_and_convert_dax1_to_dags();
					finish_xml_dag_guid_config_read();
				}
				else if ( !SETTINGS_FAST()->d_dax2_file_path.empty() )
				{
					d_dag_storage.read_and_convert_dax2_to_dags();
				}
			}

			for ( auto & dag : d_dag_storage.d_dags )
			{
				for ( auto & node : *dag->d_pRuntime->d_ordered_nodes )
				{
					d_total_proccess_node_edge += node->d_task_cpu_usage_flops * node->d_Katz_value;
				}
			}

			if ( SETTINGS()->d_shuffle_dags_after_loaded_to_storage )
			{
				if ( d_dag_storage.d_dags.size() > 0 )
				{
					std::random_device rd;
					std::mt19937 generator( rd() );
					std::shuffle( d_dag_storage.d_dags.begin() , d_dag_storage.d_dags.end() , generator );
				}
			}
			else
			{
				_ONE_TIME_VAR();
				_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
			}
			if ( SETTINGS_FAST()->d_notify_dag_loaded )
			{
				XBT_INFO( "#(%d) dags(%lu) added" , __LINE__ , this->d_dag_storage.d_dags.size() );
			}
			if ( SETTINGS_FAST()->d_dump_dag_storage_d_dags_in_init_DAGenerator )
			{
				for ( auto & ref_dag : d_dag_storage.d_dags )
				{
					ref_dag->dump( false , true );
				}
			}
		}
	CATCH_TRY
}

void DGG::callback_actor_dag_generator()
{
	// infinite loop
	while ( d_resume_iteration == NULL ) // for next iteration that not started yet
	{
		if ( d_pIteration->d_iteration_index == 1 )
		{
			sg4::this_actor::sleep_for( 1 );
		}
		else
		{
			sg4::this_actor::sleep_for( nextTime( 0.1 / SETTINGS_FAST()->d_add_one_dag_per_x_second ) ); // poisson distribution process
		}
	};
	SECTION_ALIVE_CHECK("");

	BEGIN_TRY
	while
	(
		*d_resume_iteration &&
		sg4::this_actor::get_host()->is_on() &&
		d_dag_storage.d_dags.size() > 0
	)
	{
		double nextttime = 0;
		{
			////XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

			if ( SETTINGS_FAST()->d_order_stop_loop_dag_storage_add || SETTINGS_FAST()->d_order_dump_result_and_stop_program )
			{
				XBT_INFO( "#(%d) force stop add dag storage" , __LINE__ );
				break;
			}

			bool bAnyDagAdded = false;
			for ( auto & dag : d_dag_storage.d_dags )
			{
				if ( !dag->d_dag_deadline.isDagArrivedToBeSchedule() )
				{
					bAnyDagAdded = true;
					addDAG( dag );
					break;
				}
			}
			if ( !bAnyDagAdded )
			{
				//d_dag_storage.d_dags.clear();
				//continue;
				break;
			}
			//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
			static double total_netxtime = 0;
			static int _count_nexttime = 0;
			_count_nexttime++;

			if ( SETTINGS_FAST()->d_use_poisson_distribution_for_next_add_dag_time )
			{
				nextttime = nextTime( 1.0 / SETTINGS_FAST()->d_add_one_dag_per_x_second );
			}
			else
			{
				_ONE_TIME_VAR();
				_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
				nextttime = SETTINGS_FAST()->d_add_one_dag_per_x_second;
			}

			total_netxtime += nextttime;
			if ( SETTINGS_FAST()->d_echo_nextTime_add_dag )
			{
				XBT_INFO( "#(%d) nextTime(%f) totalnexttime(%f) avg_nexttime(%f)" , __LINE__ , nextttime , total_netxtime , total_netxtime / _count_nexttime );
			}
		}
		sg4::this_actor::sleep_for( nextttime ); // poisson distribution process
	}

	if ( SETTINGS_FAST()->d_notify_callback_actor_DGGenerator_finished )
	{
		//XBT_INFO_EMPTY_LINE();
		XBT_INFO( "#(%d) ~callback dag_generator finished" , __LINE__ );
	}
	CATCH_TRY
}

void DGG::callback_actor_scheduler()
{
	while ( d_resume_iteration == NULL ) // for next iteration that not started yet
	{
		if ( d_pIteration->d_iteration_index == 1 )
		{
			sg4::this_actor::sleep_for( 1 );
		}
		else
		{
			sg4::this_actor::sleep_for( nextTime( 1.0 / SETTINGS_FAST()->d_schedule_periode_delay_time ) );
		}
	};

	BEGIN_TRY
	SECTION_ALIVE_CHECK("");
	////sg4::this_actor::sleep_for( 0.001 );
	////return;
	// infinite loop
	while ( *d_resume_iteration && sg4::this_actor::get_host()->is_on() )
	{
		{
			//SECTION_ALIVE_CHECK("");

			// اینرا آوردم بالا که پاک سازی لیست زودتر صورت گیرد
			int sort_iteration = -1;
			{
				std::lock_guard lock( *d_critical_section_entered_mutex );
				if ( SETTINGS()->d_low_memory_mode )
				{
					d_lstReady2ScheduleNode.erase(
						std::remove_if(
							d_lstReady2ScheduleNode.begin() ,
							d_lstReady2ScheduleNode.end() ,
							[]( DAGNode *& ref_node ) noexcept
					{
						return ref_node->d_pRuntime->d_taskScheduled_in_vm ;
					} ) ,
					d_lstReady2ScheduleNode.end() );
				
					for ( auto & dag : d_entry_dags )
					{
						if ( dag->doesAllTasksCompleted() )
						{
							dag->clear_unnecessary_data_after_dag_complete();
						}
					}
				}
				else
				{
					_ONE_TIME_VAR();
					_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
				}

				sort_iteration = sort_schedule_list();
			}

			if ( SETTINGS_FAST()->d_order_stop_loop_dag_schedule || SETTINGS_FAST()->d_order_dump_result_and_stop_program )
			{
				XBT_INFO( "#(%d) force stop dag schedule" , __LINE__ );
				break;
			}
			if ( SETTINGS_FAST()->d_echo_Ready2Schedule_node_list || SETTINGS_FAST()->d__echo_one_time__Ready2Schedule_node_list )
			{
				SETTINGS_FAST()->d__echo_one_time__Ready2Schedule_node_list = false;
				XBT_INFO( "#(%d) d_lstReady2ScheduleNode.size(%lu)" , __LINE__ , d_lstReady2ScheduleNode.size() );

				//this->dump_dag_scheduler_status();
			}
			//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
			if ( d_dag_storage.d_dags.size() == d_finished_dag_count )
			{
				if ( !hasUnprocessedDAGinReady2ScheduleList() ) // stop if dag storage is empty
				{
					bool no_entry_dag = true;
					for ( auto & dag : d_dag_storage.d_dags )
					{
						if ( !dag->d_dag_deadline.isDagFinishedExecution() )
						{
							no_entry_dag = false;
							break;
						}
					}
					if ( d_dag_storage.d_dags.empty() || no_entry_dag )
					{
						break;
					}
				}
			}

			if ( SETTINGS_FAST()->d_app_mode < AeAppMode::apmd_pre_scheduling_mode )
			{
				SETTINGS_FAST()->d_app_mode = AeAppMode::apmd_pre_scheduling_mode;
			}

			for ( const auto & ref_node : d_lstReady2ScheduleNode )
			{
				// do not set d_taskScheduled_in_vm to true here
				// it set to true in VMQuality::start_scheduling_quality
				if ( !ref_node->d_pRuntime->d_taskScheduled_in_vm && ref_node->d_pRuntime->d_sort_iteration == sort_iteration )
				{
					//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

					//if ( SETTINGS_FAST()->d_notify_dag_first_time_scheduled )
					//{
					//	if ( !ref_node->d_pOwnerDAG->d_dag_deadline.isDagScheduled() )
					//	{
					//		//XBT_INFO_EMPTY_LINE();
					//		XBT_INFO( "#(%d) first schedule %s at (%f)" , __LINE__ ,
					//					ref_node->d_pOwnerDAG->get_name().c_str() , SIMULATION_NOW_TIME );
					//	}
					//}

					switch ( SETTINGS_FAST()->d_EPSM_algorithm_ver )
					{
						case 1:
						case 3:
						{
							this->d_pIteration->pVMM->EPSM_algorithm_scheduleOnVM( ref_node );
							break;
						}
						case 2:
						{
							this->d_pIteration->pVMM->bundling_and_scheduleOnVM( ref_node );
							break;
						}
					}
					//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
				}
				////if ( SETTINGS_FAST()->d_order_stop_loop_dag_schedule )
				////{
				////	XBT_INFO( "#(%d) force stop dag schedule" , __LINE__ );
				////	break;
				////}
			}

			if ( SETTINGS_FAST()->d_app_mode < AeAppMode::apmd_first_scheduler_iteration_mode )
			{
				SETTINGS_FAST()->d_app_mode = AeAppMode::apmd_first_scheduler_iteration_mode;
			}

			//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );

			if ( SETTINGS_FAST()->d_order_stop_loop_dag_schedule )
			{
				XBT_INFO( "#(%d) force stop dag schedule" , __LINE__ );
				break;
			}

			//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
		}
		sg4::this_actor::sleep_for( SETTINGS_FAST()->d_schedule_periode_delay_time );

		//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	}

	CATCH_TRY
	//XBT_INFO( "#(%d) " , __LINE__ );
	if ( SETTINGS()->d_notify_callback_actor_scheduler_finished )
	{
		XBT_INFO_EMPTY_LINE();
		XBT_INFO( "#(%d) ~callback scheduler finished" , __LINE__ );
	}

	this->d_pIteration->pBenchmark->startNextIteration( this );
}

void DGG::callback_actor_estimator()
{
	// infinite loop
	while ( d_resume_iteration == NULL ) // for next iteration that not started yet
	{
		if ( d_pIteration->d_iteration_index == 1 )
		{
			sg4::this_actor::sleep_for( 1 );
		}
		else
		{
			sg4::this_actor::sleep_for( nextTime( 1.0 / SETTINGS_FAST()->d_elapse_time_between_remain_time_estimation ) ); // poisson distribution process
		}
	};
	SECTION_ALIVE_CHECK("");
	
	time_t t_last_time;
	std::time( &t_last_time );
	while ( *d_resume_iteration  && sg4::this_actor::get_host()->is_on() )
	{
		//SECTION_ALIVE_CHECK("");
		time_t t_now;
		std::time( &t_now );
		if ( difftime( t_now , t_last_time ) >= SETTINGS_FAST()->d_elapse_time_between_remain_time_estimation )
		{
			t_last_time = t_now;
			echo_time_estimation();
		}

		sg4::this_actor::sleep_for( nextTime( 1.0 / SETTINGS_FAST()->d_elapse_time_between_remain_time_estimation ) );
	}
	//XBT_INFO( "#(%d) " , __LINE__ );
}

void DGG::echo_time_estimation()
{
	BEGIN_TRY
	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	time_t t_now;
	std::time( &t_now );

	//int64_t num_processed_task = 0;
	//int64_t num_processed_edge = 0;
	int64_t enter_dag_finished_count = 0;
	for ( auto & dag : d_entry_dags )
	{
		//num_processed_task += dag->d_proccessed_node_count;
		//num_processed_edge += dag->d_proccessed_edge_count;
		if ( dag->d_dag_deadline.isDagFinishedExecution() )
		{
			enter_dag_finished_count++;
		}
	}
	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	//int64_t num_task = 0;
	//int64_t num_edge = 0;
	//int64_t storage_dag_finished_count = 0;
	int64_t unscheduled_storage_dag_count = 0;
	for ( auto & dag : d_dag_storage.d_dags ) // az storage chizi nabayd kam konam . chon inja beh hamash niaz dareh
	{
		//num_task += dag->d_nodes_count;
		//num_edge += dag->d_edge_count;
		//if ( dag->d_dag_deadline.isDagFinishedExecution() )
		//{
		//	storage_dag_finished_count++;
		//}
		if ( !dag->d_dag_deadline.isDagArrivedToBeSchedule() )
		{
			unscheduled_storage_dag_count++;
		}
	}
	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	if ( d_proccessed_node_edge > 0 )
	{
		long double unprocessed_time = 0;
		
		switch ( SETTINGS_FAST()->d_estimation_ver )
		{
			case 0:
			{
				unprocessed_time = std::max( ( d_total_proccess_node_edge )-( d_proccessed_node_edge ) , ( long double )0.0 ) *
					difftime( t_now , SETTINGS_FAST()->d_post_initialize_system_time ) / ( d_proccessed_node_edge );
				break;
			}
			case 1:
			{
				unprocessed_time = std::max( log( d_total_proccess_node_edge )-log( d_proccessed_node_edge ) , ( long double )0.0 ) *
					difftime( t_now , SETTINGS_FAST()->d_post_initialize_system_time ) / log( d_proccessed_node_edge );
				break;
			}
			case 2:
			{
				unprocessed_time = std::max( pow( d_total_proccess_node_edge , 0.5 )-pow( d_proccessed_node_edge , 0.5) , ( long double )0.0 ) *
					difftime( t_now , SETTINGS_FAST()->d_post_initialize_system_time ) / pow( d_proccessed_node_edge , 0.5);
				break;
			}
		}
		XBT_INFO( "time_remain(%Lf s) enter_dag(%lus)  enter_dag_finished(%lds)  unscheduled_storage_dag(%lds) " ,
				  unprocessed_time , d_entry_dags.size() , enter_dag_finished_count , unscheduled_storage_dag_count );
	}
	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	CATCH_TRY
}

void DGG::addDAG( DAG * refDAG )
{
	BEGIN_TRY
		//SECTION_ALIVE_CHECK("");
	if ( SETTINGS()->d_echo_add_dag )
	{
		//XBT_INFO_EMPTY_LINE();
		XBT_INFO( "#(%d) add %s at (%f)" , __LINE__ , refDAG->get_name().c_str() , SIMULATION_NOW_TIME );
	}
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

	refDAG->d_pStatistic = __NEW( DAG::statistic_neccessary() );

	for ( auto & node : *refDAG->d_pRuntime->d_ordered_nodes )
	{
		assert( node->d_pRuntime == NULL );
		node->d_pRuntime = __NEW( DAGNode::runtime_neccessary() );
	}

	refDAG->d_dag_deadline.declare_dag_arrival_time(); // bayad dar zamanikeh beh system inject mi shavad zaman set shavad zira moatally barayeh yaftan m mohasebat jozv zaman deadline dag ast

	//return;

	if ( SETTINGS()->d_echo_add_dag )
	{
		refDAG->dump( true , false );
	}

	d_entry_dags.push_back( refDAG );
	std::lock_guard lock(*d_critical_section_entered_mutex);
	for ( const auto & node : refDAG->d_pRuntime->d_roots )
	{
		node->d_pRuntime->d_taskInSchedulList = true;
		d_lstReady2ScheduleNode.push_back( node );
	}

	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	CATCH_TRY
}

int DGG::sort_schedule_list()
{
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

	//if ( SETTINGS_FAST()->d_dump_Ready2ScheduleNode_order_priority )
	//{
	//	XBT_INFO_EMPTY_LINE();
	//	XBT_INFO( "before sort" , "" );
	//	for ( auto & node : d_lstReady2ScheduleNode )
	//	{
	//		if ( !node->d_pOwnerDAG->doesAllTasksCompleted() && !node->d_taskScheduled_in_vm )
	//		{
	//			XBT_INFO( "node order dag(%s) dax(%d) node(%s)" , node->d_pOwnerDAG->get_name().c_str() , node->d_dax_id , node->d_node_name.c_str() );
	//		}
	//	}
	//	XBT_INFO_EMPTY_LINE();
	//}

	switch ( SETTINGS_FAST()->d_EPSM_algorithm_ver )
	{
		case 1: // EPSM
		case 3: // EPSM
		{
			std::sort( d_lstReady2ScheduleNode.begin() , d_lstReady2ScheduleNode.end() , []( DAGNode *& node1 , DAGNode *& node2 )
			{
				if ( node1->d_pRuntime->d_taskScheduled_in_vm || node2->d_pRuntime->d_taskScheduled_in_vm ) return false;
				// الان را فرض کن شروع همه نود ها است فلذا ددلاین را از الان حساب کن ولی اونیکه تاخیر خورده و استارتش زده شده ددلانیش مشخصه
				return *node1->d_task_deadline.get_absolute_soft_deadline( true ) < *node2->d_task_deadline.get_absolute_soft_deadline( true );
			} );
			break;
		}
		case 2: // EBSM
		{
			// sort according katz value
			switch ( SETTINGS_FAST()->d_sort_schedule_list_policy_ver )
			{
				case 4: // just deadline
				{
					_ONE_TIME_VAR();
					_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this

					std::sort( d_lstReady2ScheduleNode.begin() , d_lstReady2ScheduleNode.end() , []( DAGNode *& node1 , DAGNode *& node2 )
					{
						return node1->d_task_deadline.d_task_soft_deadline_time < node2->d_task_deadline.d_task_soft_deadline_time;
					} );
					break;
				}
				case 3: // shuffle
				{
					_ONE_TIME_VAR();
					_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
					
					std::random_device rd;
					std::mt19937 generator( rd() );
					std::shuffle( d_lstReady2ScheduleNode.begin() , d_lstReady2ScheduleNode.end() , generator );

					break;
				}
				case 2:
				{
					//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
					std::sort( d_lstReady2ScheduleNode.begin() , d_lstReady2ScheduleNode.end() , []( DAGNode *& node1 , DAGNode *& node2 )
					{
						assert( !node1->d_pRuntime->d_taskScheduled_in_vm );
						assert( !node2->d_pRuntime->d_taskScheduled_in_vm );

						if ( node1->d_pRuntime->d_taskScheduled_in_vm || node2->d_pRuntime->d_taskScheduled_in_vm ) return false;
						double now = SIMULATION_NOW_TIME;

						assert( node1->d_pOwnerDAG->d_pRuntime->d_ordered_nodes != NULL );
						assert( node2->d_pOwnerDAG->d_pRuntime->d_ordered_nodes != NULL );

						assert( node1->d_pOwnerDAG->d_dag_deadline.d_dag_deadline_time >= 0 );
						assert( node1->d_pOwnerDAG->d_dag_deadline.d_dag_process_duration_in_cheapest_and_feasible_machine >= 0 );

						assert( node2->d_pOwnerDAG->d_dag_deadline.d_dag_deadline_time >= 0 );
						assert( node2->d_pOwnerDAG->d_dag_deadline.d_dag_process_duration_in_cheapest_and_feasible_machine >= 0 );

						double pr_1 = -pow( ( ( nonzero_in_range( node1->d_pOwnerDAG->d_dag_deadline.d_dag_deadline_time - now , 0.001 ) /
												nonzero_in_range( node1->d_pOwnerDAG->d_dag_deadline.d_dag_process_duration_in_cheapest_and_feasible_machine , 0.001 ) ) /
											  nonzero_in_range( node1->d_Katz_value , 0.001 ) ) ,
											copysign( 1.0 , node1->d_pOwnerDAG->d_dag_deadline.d_dag_deadline_time - now ) );

						double pr_2 = -pow( ( ( nonzero_in_range( node2->d_pOwnerDAG->d_dag_deadline.d_dag_deadline_time - now , 0.001 ) /
												nonzero_in_range( node2->d_pOwnerDAG->d_dag_deadline.d_dag_process_duration_in_cheapest_and_feasible_machine , 0.001 ) ) /
											  nonzero_in_range( node2->d_Katz_value , 0.001 ) ) ,
											copysign( 1.0 , node2->d_pOwnerDAG->d_dag_deadline.d_dag_deadline_time - now ) );

						return pr_1 > pr_2;
					} );
					break;
				}
				case 1: // just katz
				{
					_ONE_TIME_VAR();
					_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
					//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
					std::sort( d_lstReady2ScheduleNode.begin() , d_lstReady2ScheduleNode.end() , []( DAGNode *& node1 , DAGNode *& node2 )
					{
						if ( node1->d_pRuntime->d_taskScheduled_in_vm || node2->d_pRuntime->d_taskScheduled_in_vm ) return false;
						return node1->d_Katz_value > node2->d_Katz_value;
					} );
					break;
				}
				default:
				{
					_ONE_TIME_VAR();
					_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
				}
			}

			//if ( SETTINGS_FAST()->d_dump_Ready2ScheduleNode_order_priority )
			//{
			//	XBT_INFO_EMPTY_LINE();
			//	XBT_INFO( "after sort" , "" );
			//	for ( auto & node : d_lstReady2ScheduleNode )
			//	{
			//		if ( !node->d_pOwnerDAG->doesAllTasksCompleted() && !node->d_taskScheduled_in_vm )
			//		{
			//			assert( node->d_pOwnerDAG->d_dag_deadline.d_dag_deadline_time >= 0 );
			//			assert( node->d_pOwnerDAG->d_dag_deadline.d_dag_process_duration_in_cheapest_and_feasible_machine >= 0 );
			//			double now = SIMULATION_NOW_TIME;
			//			double pr_1 = -pow( ( ( nonzero_in_range( node->d_pOwnerDAG->d_dag_deadline.d_dag_deadline_time - now , 0.001 ) /
			//									nonzero_in_range( node->d_pOwnerDAG->d_dag_deadline.d_dag_process_duration_in_cheapest_and_feasible_machine , 0.001 ) ) /
			//								  nonzero_in_range( node->d_Katz_value , 0.001 ) ) ,
			//								copysign( 1.0 , node->d_pOwnerDAG->d_dag_deadline.d_dag_deadline_time - now ) );
			//			XBT_INFO( "node ordered by priority dag(%s) dax(%d) node(%s)" , node->d_pOwnerDAG->get_name().c_str() , node->d_dax_id , node->d_node_name.c_str() );
			//			XBT_INFO( "ddl - now(%f)" , nonzero_in_range( node->d_pOwnerDAG->d_dag_deadline.d_dag_deadline_time - now , 0.001 ) );
			//			XBT_INFO( "prctime(%f)" , nonzero_in_range( node->d_pOwnerDAG->d_dag_deadline.d_dag_process_duration_in_cheapest_and_feasible_machine , 0.001 ) );
			//			XBT_INFO( "katz(%f)" , nonzero_in_range( node->d_Katz_value , 0.001 ) );
			//			XBT_INFO( "sign(%f)" , copysign( 1.0 , node->d_pOwnerDAG->d_dag_deadline.d_dag_deadline_time - now ) );
			//			XBT_INFO( "priority(%f)" , pr_1 );
			//		}
			//	}
			//	XBT_INFO_EMPTY_LINE();
			//}
			break;
		}
		default:
		{
			_ONE_TIME_VAR();
			_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
			break;
		}
	}

	d_sort_iteration++;
	for ( auto & node : d_lstReady2ScheduleNode )
	{
		if ( !node->d_pRuntime->d_taskScheduled_in_vm )
		{
			node->d_pRuntime->d_sort_iteration = d_sort_iteration;
		}
	}

	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	return d_sort_iteration;
}

bool DGG::hasUnprocessedDAGinReady2ScheduleList() // به ترو و فالس دقت کن این تابع اصلاح شده شده این
{
	BEGIN_TRY
		//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
		map< eDAG_ID , DAG * > check_dag;
		for ( auto & ref_unscheduled_node : d_lstReady2ScheduleNode )
		{
			if ( check_dag.count( ref_unscheduled_node->d_pOwnerDAG->d_dag_uniq_id ) == 0 )
			{
				if ( !ref_unscheduled_node->d_pOwnerDAG->doesAllTasksCompleted() )
				{
					check_dag[ ref_unscheduled_node->d_pOwnerDAG->d_dag_uniq_id ] = ref_unscheduled_node->d_pOwnerDAG;
				}
			}
		}
		bool processed = true;
		for ( const auto & ref_dag : check_dag )
		{
			processed &= !ref_dag.second->hasUnprocessedTask(); // دست نخورد
			if ( !processed ) break;
		}
		//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
		return !processed; // دست نخورد
	CATCH_TRY
	return false;
}

void DGG::post_process_DAG( VMQuality * pQuality ) // in vm when node job done this fxn called
{
	BEGIN_TRY
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

	// hierarchical assignment
	map< eNodeID , DAGNode * > to_schedule;
	for ( const auto & ref_proccessedNode : *pQuality->d_pAllocated_bundled_nodes_ordered )
	{
		for ( auto & ref_edge : ref_proccessedNode->d_links ) // search in childs for next node
		{
			if ( !(ref_edge->d_pDestination)->d_pRuntime->d_taskProccessed && !(ref_edge->d_pDestination)->d_pRuntime->d_taskInSchedulList ) // if child not proc and not in list then
			{
				bool bSchedule = true; // وقتی همه پدر ها پروسس شد فرزند پروسس شود
				for ( auto & ref_parent : (ref_edge->d_pDestination)->d_parents ) // if all parent of child has done
				{
					if ( ref_parent->d_pRuntime->d_taskProccessed == false )
					{
						bSchedule = false;
						break;
					}
				}
				if ( bSchedule ) // if child has valid condition for scheduling then add to list
				{
					to_schedule[ (ref_edge->d_pDestination)->d_node_uniq_id ] = ref_edge->d_pDestination;
				}
			}
		}
	}
	if ( !to_schedule.empty() )
	{
		if ( SETTINGS_FAST()->d_notify_dag_new_node_to_schedule )
		{
			//XBT_INFO_EMPTY_LINE();
			XBT_INFO( "#(%d) new node to schedule" , __LINE__ );
		}

		std::lock_guard lock( *d_critical_section_entered_mutex );
		for ( const auto & ref_to_schedule : to_schedule )
		{
			ref_to_schedule.second->d_pRuntime->d_taskInSchedulList = true;
			d_lstReady2ScheduleNode.push_back( ref_to_schedule.second );

			if ( SETTINGS_FAST()->d_notify_dag_new_node_to_schedule )
			{
				XBT_INFO( "#(%d) new node to schedule nodeid(%s)" , __LINE__ , ref_to_schedule.second->d_allocated_node_name );
			}
		}
	}
	else if ( !pQuality->d_pAllocated_bundled_nodes_ordered->front()->d_pOwnerDAG->hasUnprocessedTask() )
	{
		static int _counter_dag_finish = 0;
		_counter_dag_finish++;

		if ( SETTINGS_FAST()->d_declare_dag_finish_each_n_iteration > 0 && ( _counter_dag_finish % SETTINGS_FAST()->d_declare_dag_finish_each_n_iteration ) == 0 )
		{
			XBT_INFO_EMPTY_LINE();
			XBT_INFO( "#(%d) declare_dag_finish_time at(%f) cnt(%d)" , __LINE__ , SIMULATION_NOW_TIME , _counter_dag_finish );
		}

		pQuality->d_pAllocated_bundled_nodes_ordered->front()->d_pOwnerDAG->d_dag_deadline.declare_dag_finish_time(); // first one
		d_finished_dag_count++;

		if ( SETTINGS_FAST()->d_app_mode < AeAppMode::apmd_first_dag_finished_mode )
		{
			SETTINGS_FAST()->d_app_mode = AeAppMode::apmd_first_dag_finished_mode;
		}

		//return true;
	}
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	CATCH_TRY
	//return false;
}

void DGG::resumeIteration()
{
	d_critical_section_entered_mutex = sg4::Mutex::create();
	init_DAGStorage();
	d_resume_iteration = make_unique< bool >( true );
}

void DGG::IterationFinished( bool b_last_iteration )
{
	*d_resume_iteration = false;
	d_pEstimateActor->kill();
	d_pDGGeneratorActor->kill();
	//d_pDGSchdulerActor->kill();

	//if ( SETTINGS()->d_dump_each_simulation_iteration_result )
	//{
	//	dump();
	//}
	d_pDGGeneratorActor = NULL;
	d_pDGSchdulerActor = NULL;
	d_entry_dags.clear();
	d_lstReady2ScheduleNode.clear();
	d_dag_storage.cleanup();
	{
		std::lock_guard lock( *d_critical_section_entered_mutex );
	}
	d_critical_section_entered_mutex = NULL;
}

void DGG::dump()
{
	XBT_INFO_EMPTY_LINE();
	
	//LOG_TR( "INSERT INTO [dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval],[decval]) VALUES()" , "" );
	//LOG_TR( "INSERT INTO [dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval]) VALUES(@config_iteration_pack_id,'comment','%s')" , CNFG()->d_comment.c_str() );

	XBT_INFO( "#(%d) storage dag(%lus)" , __LINE__ , d_dag_storage.d_dags.size() );
	XBT_INFO( "#(%d) entry dag(%lus)" , __LINE__ , d_entry_dags.size() );

	LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[decval])\
				VALUES(@config_iteration_pack_id,'storage dag',%lu)" , d_dag_storage.d_dags.size()/**/);
	LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[decval])\
				VALUES(@config_iteration_pack_id,'entry dag',%lu)" , d_entry_dags.size()/**/ );

	// dag type statistics
	{
		XBT_INFO( "--------------------" , "" );
		XBT_INFO( "dag type statistics" , "" );
		map< int /*dag type*/ , int /*count*/ > _dagtype_count;
		for ( auto & dag : d_entry_dags )
		{
			_dagtype_count[ dag->d_dag_config.d_dag_type ] = 0;
		}
		for ( auto & dag : d_entry_dags )
		{
			_dagtype_count[ dag->d_dag_config.d_dag_type ] += 1;
		}
		for ( auto & dt : _dagtype_count )
		{
			XBT_INFO( "dag type(%d)=%d" , dt.first , dt.second );

			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
						VALUES(@config_iteration_pack_id,'dag type statistics','dag type(%d)',%d)" ,
					dt.first/*dag type*/ , dt.second/*count*/ );
		}
	}

	// wait policy statistics
	{
		XBT_INFO( "--------------------" , "" );
		XBT_INFO( "#(%d) wait policy statistics" , __LINE__ );
		int wait_count = this->d_pIteration->pVMM->d_statistics[ AeStatisticElement::stt_bundle_scheduling_result ][ STATISTICS_bundle_wait_count ].bundle_wait_count;
		int scheduling_count = this->d_pIteration->pVMM->d_statistics[ AeStatisticElement::stt_bundle_scheduling_result ][ STATISTICS_bundle_scheduled_count ].bundle_scheduled_count;
		//for ( auto & dag : d_entry_dags )
		//{
		//	wait_count += dag->d_schedul_bundle_wait_count;
		//	scheduling_count += dag->d_schedul_bundle_scheduled_count;
		//}
		XBT_INFO( "wait_count:%ds" , wait_count );
		XBT_INFO( "scheduling_count:%ds" , scheduling_count );

		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
				VALUES(@config_iteration_pack_id,'wait policy statistics','wait_count', %d)" ,
				wait_count );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
				VALUES(@config_iteration_pack_id,'wait policy statistics','scheduling_count', %d)" ,
				scheduling_count );
	}

	// deadline statistics
	{
		XBT_INFO( "--------------------" , "" );
		XBT_INFO( "deadline statistics" , "" );
		int finished_before_deadline_count_with_wait = 0;
		int finished_before_deadline_count_without_wait = 0;
		int finished_after_deadline_count_with_wait = 0;
		int finished_after_deadline_count_without_wait = 0;
		for ( auto & dag : d_entry_dags )
		{
			if ( dag->d_dag_deadline.isDagFinishedBeforeDeadline() )
			{
				if ( this->d_pIteration->pVMM->d_statistics[ AeStatisticElement::stt_dag_waiting_count ][ STATISTICS_INT( dag->d_dag_uniq_id ) ].dag_waiting_count > 0 )
				{
					finished_before_deadline_count_with_wait++;
				}
				else
				{
					finished_before_deadline_count_without_wait++;
				}
			}
			else
			{
				if ( this->d_pIteration->pVMM->d_statistics[ AeStatisticElement::stt_dag_waiting_count ][ STATISTICS_INT( dag->d_dag_uniq_id ) ].dag_waiting_count > 0 )
				{
					finished_after_deadline_count_with_wait++;
				}
				else
				{
					finished_after_deadline_count_without_wait++;
				}
			}
		}
		XBT_INFO( "#(%d) dags status : finished before deadline with wait(%ds)" , __LINE__ ,
				  finished_before_deadline_count_with_wait );
		XBT_INFO( "#(%d) dags status : finished before deadline without wait(%ds)" , __LINE__ ,
				  finished_before_deadline_count_without_wait );

		XBT_INFO( "#(%d) dags status : finished after deadline with wait(%ds)" , __LINE__ ,
				  finished_after_deadline_count_with_wait );
		XBT_INFO( "#(%d) dags status : finished after deadline without wait(%ds)" , __LINE__ ,
				  finished_after_deadline_count_without_wait );

		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
					VALUES(@config_iteration_pack_id,'deadline statistics','finished before deadline with wait',%d)" ,
				finished_before_deadline_count_with_wait );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
					VALUES(@config_iteration_pack_id,'deadline statistics','finished before deadline without wait',%d)" ,
				finished_before_deadline_count_without_wait );


		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
					VALUES(@config_iteration_pack_id,'deadline statistics','finished after deadline with wait',%d)" ,
				finished_after_deadline_count_with_wait );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
					VALUES(@config_iteration_pack_id,'deadline statistics','finished after deadline without wait',%d)" ,
				finished_after_deadline_count_without_wait );

		int count_cross_deadline = 0;
		double total_cross_deadline = 0;

		int count_before_deadline = 0;
		double total_before_deadline = 0;

		for ( auto & dagg : d_entry_dags )
		{
			if ( dagg->d_dag_deadline.d_dag_actual_finish_time > dagg->d_dag_deadline.d_dag_deadline_time )
			{
				count_cross_deadline++;
				total_cross_deadline += dagg->d_dag_deadline.d_dag_actual_finish_time - dagg->d_dag_deadline.d_dag_deadline_time;
			}

			if ( dagg->d_dag_deadline.d_dag_actual_finish_time <= dagg->d_dag_deadline.d_dag_deadline_time )
			{
				count_before_deadline++;
				total_before_deadline += dagg->d_dag_deadline.d_dag_actual_finish_time - dagg->d_dag_deadline.d_dag_deadline_time;
			}

		}
		if ( count_cross_deadline != 0 )
		{
			XBT_INFO( "#(%d) total cross deadline(%s)  mean cross deadline(%s) " , __LINE__ , 
					  trim_floatnumber( total_cross_deadline ).c_str() , trim_floatnumber( total_cross_deadline / count_cross_deadline ).c_str() );
		}
		else
		{
			XBT_INFO( "#(%d) total cross deadline(%s) " , __LINE__ , trim_floatnumber( total_cross_deadline ).c_str() );
		}

		if ( count_before_deadline != 0 )
		{
			XBT_INFO( "#(%d) mean before deadline(%s) " , __LINE__ , trim_floatnumber( total_before_deadline / count_before_deadline ).c_str() );

			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'deadline statistics','mean before deadline',%f)" ,
					total_before_deadline / count_before_deadline/*mean before deadline*/ );
		}

		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
					VALUES(@config_iteration_pack_id,'deadline statistics','total cross deadline',%f)" ,
				total_cross_deadline );
		if ( count_cross_deadline != 0 )
		{
			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'deadline statistics','mean cross deadline',%f)" ,
					total_cross_deadline / count_cross_deadline/*mean cross deadline*/ );
		}
	}

	// least vm price statistics
	if ( 1 )
	{
		XBT_INFO( "--------------------" , "" );
		XBT_INFO( "least vm price statistics" , "" );
		map< eVM_Type , int > _least_vm_price;
		for ( auto & dagg : d_entry_dags )
		{
			_least_vm_price[ dagg->d_least_price_vmp_type ] += 1;
		}
		for ( auto & price : _least_vm_price )
		{
			XBT_INFO( "least price vm for dag(%d)=%d" , price.first , price.second );

			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
						VALUES(@config_iteration_pack_id,'least vm price statistics','least price vm (%d)',%d)" ,
					price.first/*least vm type*/ , price.second/*count*/ );
		}
	}

	// adaptation to ccr statistic
	{
		XBT_INFO( "--------------------" , "" );
		XBT_INFO( "adaptation to ccr statistic" , "" );

		map< double /*ccr*/ , vector< double > /*masscommunication/time*/ > ccr_x_masscommunication_time;
		map< double /*ccr*/ , vector< double > /*mass_slowcommunication*/ > ccr_x_mass_slowcommunication;
		map< double /*ccr*/ , vector< double > /*mass_avgprocess*/ > ccr_x_mass_avgprocess;
		map< double /*ccr*/ , int /*node count*/ > ccr_nodecount;
		map< double /*ccr*/ , int /*dag count*/ > ccr_dagcount;

		for ( auto & dagg : d_entry_dags )
		{
			ccr_x_masscommunication_time[ dagg->d_ccr ].push_back(dagg->d_pStatistic->d_dag_communication_byte_need / dagg->d_pStatistic->d_dag_communication_duration);
			ccr_x_mass_slowcommunication[ dagg->d_ccr ].push_back(dagg->d_pStatistic->d_dag_reality_communication_byte_on_slow_link / dagg->d_pStatistic->d_dag_expectation_communication_byte_on_slow_link);
			ccr_x_mass_avgprocess[ dagg->d_ccr ].push_back(dagg->d_pStatistic->d_dag_reality_computation_time / dagg->d_pStatistic->d_dag_computation_time_on_avg_machine);
		
			ccr_nodecount[ dagg->d_ccr ] += dagg->d_nodes_count;
			ccr_dagcount[ dagg->d_ccr ] += 1;
		
		}

		for ( auto & ccr : ccr_x_masscommunication_time )
		{
			if ( ccr.second.size() > 0 )
			{
				double avg = 0;
				for ( auto & mass : ccr.second )
				{
					avg += mass;
				}
				LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
					VALUES(@config_iteration_pack_id,'ccr dagsize masscommunication/time','%f',%f)" ,
						ccr.first/*ccr*/ , avg / ccr.second.size()/*avg mass coomunication*/ );
			}
		}

		for ( auto & ccr : ccr_x_mass_slowcommunication )
		{
			if ( ccr.second.size() > 0 )
			{
				double avg = 0;
				for ( auto & mass : ccr.second )
				{
					avg += mass;
				}
				LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
					VALUES(@config_iteration_pack_id,'ccr dagsize mass_slowcommunication','%f',%f)" ,
						ccr.first/*ccr*/ , avg / ccr.second.size()/*avg mass slow coomunication*/ );
			}
		}
		
		for ( auto & ccr : ccr_x_mass_avgprocess )
		{
			if ( ccr.second.size() > 0 )
			{
				double avg = 0;
				for ( auto & mass : ccr.second )
				{
					avg += mass;
				}
				LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
					VALUES(@config_iteration_pack_id,'ccr dagsize mass_avgprocess','%f',%f)" ,
						ccr.first/*ccr*/ , avg / ccr.second.size()/*avg mass computation*/ );
			}
		}

		for ( auto & ccr : ccr_nodecount )
		{
			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[decval],[relval])\
					VALUES(@config_iteration_pack_id,'ccr nodecount count',%d,%f)" ,
					ccr.second/*count*/ , ccr.first/*ccr*/ );
		}

		for ( auto & ccr : ccr_dagcount )
		{
			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[decval],[relval])\
					VALUES(@config_iteration_pack_id,'ccr dagcount',%d,%f)" ,
					ccr.second/*count*/ , ccr.first/*ccr*/ );
		}

		
	}

	// makespan statistics
	{
		XBT_INFO( "--------------------" , "" );
		XBT_INFO( "makespan statistics" , "" );

		double min_workload_makespan = -1;
		double max_workload_makespan = -1;

		map< double/*ccr*/ , tuple< double , double > > ccr_makespans;

		for ( auto & dag : this->d_entry_dags )
		{
			if ( min_workload_makespan < 0 )
			{
				min_workload_makespan = dag->d_dag_deadline.d_dag_actual_recieved_time;
			}
			if ( dag->d_dag_deadline.d_dag_actual_recieved_time < min_workload_makespan )
			{
				min_workload_makespan = dag->d_dag_deadline.d_dag_actual_recieved_time;
			}
			if ( max_workload_makespan < 0 )
			{
				max_workload_makespan = dag->d_dag_deadline.d_dag_actual_finish_time;
			}
			if ( dag->d_dag_deadline.d_dag_actual_finish_time > max_workload_makespan )
			{
				max_workload_makespan = dag->d_dag_deadline.d_dag_actual_finish_time;
			}

			if ( ccr_makespans.count( dag->d_ccr ) == 0 )
			{
				ccr_makespans[ dag->d_ccr ] = make_tuple( dag->d_dag_deadline.d_dag_actual_recieved_time , dag->d_dag_deadline.d_dag_actual_finish_time );
			}
			if ( dag->d_dag_deadline.d_dag_actual_recieved_time < std::get<0>( ccr_makespans[ dag->d_ccr ] ) )
			{
				std::get<0>( ccr_makespans[ dag->d_ccr ] ) = dag->d_dag_deadline.d_dag_actual_recieved_time;
			}
			if ( dag->d_dag_deadline.d_dag_actual_finish_time > std::get<1>( ccr_makespans[ dag->d_ccr ] ) )
			{
				std::get<1>( ccr_makespans[ dag->d_ccr ] ) = dag->d_dag_deadline.d_dag_actual_finish_time;
			}
		}

		XBT_INFO( "makespan workload statistics min(%f) max(%f)" , min_workload_makespan , max_workload_makespan );

		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
					VALUES(@config_iteration_pack_id,'makespan workload statistics','%f',%f)" ,
				min_workload_makespan , max_workload_makespan );

		for ( auto & ccr : ccr_makespans )
		{
			XBT_INFO( "makespan ccr workload statistics min(%f) max(%f)" , ccr.first , std::get<0>( ccr.second ) , std::get<1>( ccr.second ) );

			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]( [cnfitrpc_id] , [title] , [txtval] , [relval] )\
					VALUES( @config_iteration_pack_id , 'makespan ccr workload statistics' , '%f %f' , %f )" ,
					ccr.first/*ccr*/ , std::get<0>( ccr.second )/*min recieved_time*/ , std::get<1>( ccr.second )/*max finish_time*/ );
		}

	}
}
