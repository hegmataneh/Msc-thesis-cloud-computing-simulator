#define uses_accumulate
#define uses_ofstream
#define uses_Deadline_Quota
#define uses_VMachine
#define uses_LOG
#define uses_Global_config
#define uses_DGG
#define uses_SectionTimeKeeper
#define uses_CBenchmark
#define uses_PRId64
#define uses_filesystem
#define uses_random
#include "../inc/dep"

XBT_LOG_NEW_DEFAULT_CATEGORY( VM , "Messages specific for this s4u example" );

#pragma region VMachine

VMachine_Small::VMachine_Small( bool badd )
{
	d_type = eVM_Type::vm_small;

	d_cpu_speed = CPU_SPEED_POSTFIX( 2 );
	d_memory_access_speed = MEMORY_ACCESS_SPEED_POSTFIX( 2 );
	d_lan_network_speed = SETTINGS_FAST()->d_lan_bandwidth;
	d_wan_network_speed = WAN_NETWORK_ACCESS_SPEED_POSTFIX( 1 );
	////d_network_latency = NETWORK_LATENCY_POSTFIX( 8 );

	d_dollar_cost_per_cycle = 1.0;

	if ( badd ) add_type();
}

VMachine_Medium::VMachine_Medium( bool badd )
{
	d_type = eVM_Type::vm_medium;

	d_cpu_speed = CPU_SPEED_POSTFIX( 4 );
	d_memory_access_speed = MEMORY_ACCESS_SPEED_POSTFIX( 2 );
	d_lan_network_speed = SETTINGS_FAST()->d_lan_bandwidth;
	d_wan_network_speed = WAN_NETWORK_ACCESS_SPEED_POSTFIX( 2 );
	////d_network_latency = NETWORK_LATENCY_POSTFIX(4);

	d_dollar_cost_per_cycle = 2.0;

	if ( badd ) add_type();
}

VMachine_Large::VMachine_Large( bool badd )
{
	d_type = eVM_Type::vm_large;

	d_cpu_speed = CPU_SPEED_POSTFIX( 8 );
	d_memory_access_speed = MEMORY_ACCESS_SPEED_POSTFIX( 2 );
	d_lan_network_speed = SETTINGS_FAST()->d_lan_bandwidth;
	d_wan_network_speed = WAN_NETWORK_ACCESS_SPEED_POSTFIX( 3 );
	////d_network_latency = NETWORK_LATENCY_POSTFIX(2);

	d_dollar_cost_per_cycle = 4.0;

	if ( badd ) add_type();
}

VMachine_ExtraLarge::VMachine_ExtraLarge( bool badd )
{
	d_type = eVM_Type::vm_extra_large;

	d_cpu_speed = CPU_SPEED_POSTFIX( 16 );
	d_memory_access_speed = MEMORY_ACCESS_SPEED_POSTFIX( 2 );
	d_lan_network_speed = SETTINGS_FAST()->d_lan_bandwidth;
	d_wan_network_speed = WAN_NETWORK_ACCESS_SPEED_POSTFIX( 4 );
	////d_network_latency = NETWORK_LATENCY_POSTFIX(1);

	d_dollar_cost_per_cycle = 8.0;

	if ( badd ) add_type();
}



vector< VMachine > heterogen_system_vm_types; // Global Variable
vector< VMachine > homogen_system_vm_types; // Global Variable

vector< VMachine > & VMachine::get_type_instances()
{
	switch ( SETTINGS_FAST()->d_resource_heterogeneity )
	{
		case 1:
		{
			return homogen_system_vm_types;
		}
		case 2:
		{
			return heterogen_system_vm_types;
		}
	}
	return heterogen_system_vm_types;
}

VMachine & VMachine::get_type_instance( eVM_Type type )
{
	for ( auto & vm : heterogen_system_vm_types )
	{
		if ( vm.d_type == type )
		{
			return vm;
		}
	}
	assert( 0 );
	return VMachine::strongest_machine();
}

VMachine & VMachine::weakest_machine()
{
	return VMachine::get_type_instances().front();
}
VMachine & VMachine::strongest_machine()
{
	return VMachine::get_type_instances().back();
}

bool compareInterval( VMachine & i1 , VMachine & i2 )
{
	return ( i1.d_type < i2.d_type );
}

void VMachine::add_type()
{
	BEGIN_TRY
	switch ( this->d_type )
	{
		case eVM_Type::vm_small:
		{
			heterogen_system_vm_types.push_back( *this );
			homogen_system_vm_types.push_back( *this );
			break;
		}
		case eVM_Type::vm_medium:
		{
			heterogen_system_vm_types.push_back( *this );
			break;
		}
		case eVM_Type::vm_large:
		{
			heterogen_system_vm_types.push_back( *this );
			break;
		}
		case eVM_Type::vm_extra_large:
		{
			heterogen_system_vm_types.push_back( *this );
			homogen_system_vm_types.push_back( *this );
			break;
		}
		default:
		{
			XBT_INFO( "#(%d) warning" , __LINE__ ); // donot comment this
			break;
		}
	}

	sort( homogen_system_vm_types.begin() , homogen_system_vm_types.end() , compareInterval );
	sort( heterogen_system_vm_types.begin() , heterogen_system_vm_types.end() , compareInterval );
	CATCH_TRY
}

const char * VMachine::get_vmtypeName() const
{
	BEGIN_TRY
	switch ( d_type )
	{
		case eVM_Type::vm_small:
			return ( "small" );
		case eVM_Type::vm_medium:
			return ( "medium" );
		case eVM_Type::vm_large:
			return ( "large" );
		case eVM_Type::vm_extra_large:
			return ( "extra_large" );
	}
	CATCH_TRY
	XBT_INFO( "#(%d) warning" , __LINE__ ); // donot comment this
	return ( " " );
}

double VMachine::get_cost_duration( double duration ) const
{
	return duration * d_dollar_cost_per_cycle / SETTINGS_FAST()->d_VM_LEASED_CYCLE_TIME_SEC;
}

double VMachine::get_duration_datatransfer_on_wan( double datasize ) const
{
	return datasize / d_wan_network_speed;
}

double VMachine::get_duration_datatransfer_on_lan( double datasize ) const
{
	return datasize / d_lan_network_speed;
}

double VMachine::get_duration_datatransfer_on_mem( double datasize ) const
{
	return datasize / d_memory_access_speed;
}

double VMachine::get_execution_duration( double flops ) const
{
	return flops / d_cpu_speed;
}

double VMachine::get_execution_cost( double flops ) const
{
	return get_cost_duration( get_execution_duration( flops ) );
}

double VMachine::get_cost_datatransfer_on_wan( double datasize ) const
{
	return get_cost_duration( get_duration_datatransfer_on_wan( datasize ) );
}

void VMachine::dump( bool inside_another )
{
	BEGIN_TRY
	if ( !inside_another ) XBT_INFO_EMPTY_LINE();
	XBT_INFO( "dump vmtype_%s , cpu(%s flops) , memory(%s b/s) , lan(%s b/s) , wan(%s b/s) , cost(%s $/c)" ,
			  get_vmtypeName() , trim_floatnumber( d_cpu_speed ).c_str() , trim_floatnumber( d_memory_access_speed ).c_str() ,
			  trim_floatnumber( d_lan_network_speed ).c_str() , trim_floatnumber( d_wan_network_speed ).c_str() ,
			  trim_floatnumber( d_dollar_cost_per_cycle ).c_str() );
	CATCH_TRY
}

#pragma endregion

#pragma region DAG_config

void DAG_config::init_dag_config( int dag_type )
{
	BEGIN_TRY
	d_dag_type = dag_type;

	d_container_initialization_time = SETTINGS_FAST()->d_CONTAINER_INITIALIZATION_DALEY_TIME;
	CATCH_TRY
}

////string DAG_config::get_dagtypename() const
////{
////	BEGIN_TRY
////	switch ( d_dag_type )
////	{
////		case linear_dag :
////			return string( "linear" );
////		case diamond_dag :
////			return string( "diamond" );
////		case complex1_dag :
////			return string( "complex1" );
////		case complex2_dag :
////			return string( "complex2" );
////		case complex3_dag :
////			return string( "complex3" );
////		case complex4_dag:
////			return string( "complex4" );
////	}
////	return string_format( "daxtype(%d)" , d_dag_type );
////	CATCH_TRY
////	return string( "" );
////}


#pragma endregion

#pragma region Deadline_Quota

#pragma region DAG_Deadline_Quota

DAG_Deadline_Quota::DAG_Deadline_Quota()
{
	clear();
}

void DAG_Deadline_Quota::clear()
{
	d_paired_dag = NULL;

	d_dag_requestor_deadline_duration = -1; // invalid in task
	d_dag_process_duration_in_cheapest_and_feasible_machine = -1; // not invalid in dag and task

	d_dag_actual_recieved_time = -1; // invalid in 
	d_dag_deadline_time = -1;

	d_dag_actual_first_schedule_time = -1;
	d_dag_actual_finish_time = -1;
}

void DAG_Deadline_Quota::declare_dag_arrival_time()
{
	BEGIN_TRY
	assert( d_dag_actual_recieved_time < 0 );
	if ( d_dag_actual_recieved_time >= 0 ) return;

	if ( SETTINGS_FAST()->d_enable_DAG_Deadline_Quota_notify__declare_dag_start_time )
	{
		XBT_INFO( "#(%d) DAG_Deadline_Quota::declare_dag_start_time" , __LINE__ );
	}

	d_dag_actual_recieved_time = SIMULATION_NOW_TIME;
	d_dag_deadline_time = d_dag_actual_recieved_time + d_dag_requestor_deadline_duration; // set dag deadline

	assert( d_dag_deadline_time > 0 );

	// assign deadline quota to tasks
	// at this moment task quota is declared
	// up down methode
	// hierarchical assignment
	// infinite loop
	vector< DAGNode * > inlist_node;
	map< eNodeID , eNodeID > control;
	//AbsoluteTime max_max_soft_deadline_time = -1;
	for ( auto & node : this->d_paired_dag->d_pRuntime->d_roots )
	{
		inlist_node.push_back( node );
		control[ node->d_node_uniq_id ] = node->d_node_uniq_id;
	}
	while ( !inlist_node.empty() )
	{
		vector< DAGNode * > temp_inlist_node( inlist_node );
		inlist_node.clear();
		for ( auto & node : temp_inlist_node )
		{
			for ( auto & link : node->d_links )
			{
				bool badd = true;
				for ( auto & parent : (link->d_pDestination)->d_parents ) // مطمئن می شویم نود پدر در گراف جدید ساخته شده باشد
				{
					if ( control.count( parent->d_node_uniq_id ) == 0 )
					{
						badd = false;
						break;
					}
				}
				if ( badd )
				{
					if ( control.count( (link->d_pDestination)->d_node_uniq_id ) == 0 )
					{
						inlist_node.push_back( link->d_pDestination );
						control[ (link->d_pDestination)->d_node_uniq_id ] = (link->d_pDestination)->d_node_uniq_id;
					}
				}
			}
		}
		for ( auto & node : temp_inlist_node )
		{
			assert( node->d_task_deadline.d_task_quota_duration_based_on_process_need >= 0 );
			if ( node->d_parents.size() == 0 )
			{
				////node->d_task_deadline.d_task_quota_schedule_time = this->d_dag_recieved_time;
				node->d_task_deadline.d_task_soft_deadline_time =
					this->d_dag_actual_recieved_time +
					node->d_task_deadline.d_task_quota_duration_based_on_process_need +
					node->d_task_deadline.d_task_spare_duration_quota;

				////node->d_task_deadline.d_first_time_deadline = node->d_task_deadline.d_task_soft_deadline_time;

				//if ( node->d_task_deadline.d_task_soft_deadline_time > max_max_soft_deadline_time )
				//{
				//	max_max_soft_deadline_time = node->d_task_deadline.d_task_soft_deadline_time;
				//}
			}
			else
			{
				AbsoluteTime max_soft_deadline_time = -1;
				for ( auto & parent : node->d_parents )
				{
					assert( parent->d_task_deadline.d_task_soft_deadline_time >= 0 );
					if ( parent->d_task_deadline.d_task_soft_deadline_time > max_soft_deadline_time )
					{
						max_soft_deadline_time = parent->d_task_deadline.d_task_soft_deadline_time;
					}
				}
				assert( max_soft_deadline_time >= 0 );

				////node->d_task_deadline.d_task_quota_schedule_time = max_quota_deadline_time; // یعنی تسک در این زمان دیگه باید شروع شده باشه دیگه . طبق کف ماشین توانا برای پردازش کل دگ این عدد بدست می اید
				node->d_task_deadline.d_task_soft_deadline_time =
					max_soft_deadline_time +
					node->d_task_deadline.d_task_quota_duration_based_on_process_need +
					node->d_task_deadline.d_task_spare_duration_quota;
				
				////node->d_task_deadline.d_first_time_deadline = node->d_task_deadline.d_task_soft_deadline_time;
				
				//if ( node->d_task_deadline.d_task_soft_deadline_time > max_max_soft_deadline_time )
				//{
				//	max_max_soft_deadline_time = node->d_task_deadline.d_task_soft_deadline_time;
				//}
			}

			//XBT_INFO( "#(%d) %d  %f" , __LINE__ , node->d_dax_id , node->d_task_deadline.d_task_soft_deadline_time );
		}
	};

	for ( auto & node : *this->d_paired_dag->d_pRuntime->d_ordered_nodes )
	{
		assert( !isnan( node->d_task_deadline.d_task_soft_deadline_time ) && node->d_task_deadline.d_task_soft_deadline_time >= 0 );
	}

	//assert(max_max_soft_deadline_time >= 0);
	//if ( max_max_soft_deadline_time < 0 || max_max_soft_deadline_time > d_dag_deadline_time + 0.01 )
	//{
	//	cout << max_max_soft_deadline_time << "  " << d_dag_deadline_time << endl;
	//}
	//assert(max_max_soft_deadline_time<=d_dag_deadline_time);
	CATCH_TRY
}

void DAG_Deadline_Quota::declare_dag_start_schedule()
{
	if ( d_dag_actual_first_schedule_time < 0 )
	{
		d_dag_actual_first_schedule_time = SIMULATION_NOW_TIME;
	}
}

void DAG_Deadline_Quota::declare_dag_finish_time()
{
	BEGIN_TRY
	if ( SETTINGS_FAST()->d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__finish_declare )
	{
		XBT_INFO( "#(%d) dag(%s) finished \n" , __LINE__ , this->d_paired_dag->get_name().c_str() );
	}
	assert( d_dag_actual_finish_time < 0 );
	if ( d_dag_actual_finish_time < 0 )
	{
		d_dag_actual_finish_time = SIMULATION_NOW_TIME;

		if ( SETTINGS_FAST()->d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__status_dag_to_deadline )
		{
			XBT_INFO_EMPTY_LINE();
		}
		if ( d_dag_actual_finish_time <= d_dag_deadline_time )
		{
			if ( SETTINGS_FAST()->d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__status_dag_to_deadline )
			{
				XBT_INFO( "#(%d) finish dag(%s) before deadline " , __LINE__ , this->d_paired_dag->get_name().c_str() );

				this->dump( true );
			}
		}
		else
		{
			if ( SETTINGS_FAST()->d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__status_dag_to_deadline )
			{
				XBT_INFO( "#(%d) finish dag(%s) after deadline " , __LINE__ , this->d_paired_dag->get_name().c_str() );

				this->dump( true );
			}
		}
		if ( SETTINGS_FAST()->d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__dump_dag )
		{
			d_paired_dag->dump( false , false );
		}
	}
	CATCH_TRY
}

bool DAG_Deadline_Quota::isDagFinishedBeforeDeadline() const
{
	if ( d_dag_actual_finish_time >= 0 && d_dag_deadline_time >= 0 )
	{
		return d_dag_actual_finish_time <= d_dag_deadline_time;
	}
	return false;
}

void DAG_Deadline_Quota::dump( bool inside_another )
{
	BEGIN_TRY
	if ( !inside_another ) XBT_INFO_EMPTY_LINE();
	XBT_INFO( "dump deadline dag(%s) , requestor_deadline_duration(%f s) , duration_in_cheapest_and_feasible_machine(%f s)" ,
			  this->d_paired_dag->get_name().c_str() , d_dag_requestor_deadline_duration , d_dag_process_duration_in_cheapest_and_feasible_machine );
	
	XBT_INFO( "dump deadline dag %s %s %s" ,
			  string_format( ( d_dag_actual_recieved_time >= 0 ? ", recieved_time(at % f s) " : "" ) , d_dag_actual_recieved_time ).c_str() ,
			  string_format( ( d_dag_deadline_time >= 0 ? ", dag_deadline(at %f s) " : "" ) , d_dag_deadline_time ).c_str() ,
			  string_format( ( d_dag_actual_finish_time >= 0 ? ", finish_time(at %f s)" : "" ) , d_dag_actual_finish_time ).c_str());

	string str;

	XBT_INFO( "dump deadline scheduled on vms:%s" , str.c_str() );
	CATCH_TRY
}

#pragma endregion

#pragma region Task_Deadline_Quota

Task_Deadline_Quota::Task_Deadline_Quota()
{
	clear();
}

void Task_Deadline_Quota::clear()
{
	d_paired_node = NULL;

	d_task_quota_duration_based_on_process_need = -1;
	d_task_spare_duration_quota = 0; // 0 is inportant
	d_task_soft_deadline_time = -1; // -1 is important
	//d_task_quota_deadline_time = -1;

	//d_first_time_deadline = -1;

	d_task_actual_triggered_time = -1; // trigger by sys admin to exec on container
	d_task_actual_complete_execution_time = -1;

	//d_last_time_deadline_reset = -1;
}

void Task_Deadline_Quota::declare_task_actual_start_time()
{
	BEGIN_TRY
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	if ( d_task_actual_triggered_time < 0 )
	{
		// ددلاین اصلی اینست و این یعنی اینکه ددلاین هر تسک بر اساس زمان شروع تسک تعیین می شود
		d_task_actual_triggered_time = SIMULATION_NOW_TIME;

		////d_task_soft_deadline_time = d_paired_node->d_task_triggered_time + d_task_quota_duration_based_on_process_need + d_task_spare_duration_quota;
	}
	CATCH_TRY
}

void Task_Deadline_Quota::declare_task_actual_finish_time()
{
	BEGIN_TRY
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	//if ( SETTINGS()->d_enable_Task_Deadline_Quota_notify )
	//{
	//	XBT_INFO_EMPTY_LINE();
	//	XBT_INFO( "#(%d) task_finish node_%s_%s" , __LINE__ , this->d_paired_node->d_node_name ,
	//			  this->d_paired_node->d_pOwnerDAG->get_name().c_str()
	//	);
	//}

	d_task_actual_complete_execution_time = SIMULATION_NOW_TIME;
	assert( d_task_soft_deadline_time >= 0 );

	if ( d_task_actual_complete_execution_time <= d_task_soft_deadline_time )
	{
		// پخش زمان اضافه بین زیر تسک ها

		double extra_spare_time = d_task_soft_deadline_time - d_task_actual_complete_execution_time;
		if ( SETTINGS_FAST()->d_enable_Task_Deadline_Quota_notify )
		{
			XBT_INFO( "#(%d) extra spare time(%f s)" , __LINE__ , extra_spare_time );
		}

		// donate it to descendants
		// hierarchical assignment
		// infinite loop
		//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
		vector< DAGNode * > desc_ordered;
		{
			vector< DAGNode * > ret_temp;
			map< eNodeID , eNodeID > control_loop;
			for ( auto link : d_paired_node->d_links )
			{
				if ( control_loop.count( (link->d_pDestination)->d_node_uniq_id ) == 0 )
				{
					ret_temp.push_back( link->d_pDestination );
					control_loop[ (link->d_pDestination)->d_node_uniq_id ] = (link->d_pDestination)->d_node_uniq_id;
				}
			}
			do
			{
				vector< DAGNode * > last_ret_temp( ret_temp );
				ret_temp.clear();
				for ( auto & node : last_ret_temp )
				{
					for ( auto & link : node->d_links )
					{
						if ( control_loop.count( (link->d_pDestination)->d_node_uniq_id ) == 0 )
						{
							ret_temp.push_back( link->d_pDestination );
							control_loop[ (link->d_pDestination)->d_node_uniq_id ] = (link->d_pDestination)->d_node_uniq_id;
						}
					}
				}
				for ( auto & node : last_ret_temp )
				{
					desc_ordered.push_back( node );
				}
				
			} while ( !ret_temp.empty() );
		}
		//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

		double total_subtasks_quota = 0;
		for ( const auto & ref_node : desc_ordered )
		{
			total_subtasks_quota += ref_node->d_task_deadline.d_task_quota_duration_based_on_process_need;
		}

		// بر اساس سهم هر تسک از کل زیر تسک های تسک اتمام یافته زیر تسک ها زمان اضافه را دریافت می کنند به عدالت
		for ( const auto & ref_node : desc_ordered )
		{
			ref_node->d_task_deadline.d_task_spare_duration_quota += extra_spare_time * ref_node->d_task_deadline.d_task_quota_duration_based_on_process_need / total_subtasks_quota;

			//if ( SETTINGS_FAST()->d_enable_Task_Deadline_Quota_notify )
			//{
			//	XBT_INFO_EMPTY_LINE();
			//	XBT_INFO( "update task_spare_time_duration_quota +" , "" );
			//	if ( SETTINGS_FAST()->d_dump_node_deadline_spare_time )
			//	{
			//		ref_node->dump( true , false );
			//	}
			//}
		}
	}
	else
	{
		// کسر زمان از زیر کسر ها به تناسب

		double total_deduct_time = d_task_actual_complete_execution_time - d_task_soft_deadline_time;
		//if ( SETTINGS_FAST()->d_enable_Task_Deadline_Quota_notify )
		//{
		//	XBT_INFO( "#(%d) shortage time(%f s)" , __LINE__ , total_deduct_time );
		//}

		// کسر از زیر مجموعه
		// hierarchical assignment
		// infinite loop
		//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose , "#(%d) " , __LINE__ );
		vector< DAGNode * > desc_ordered;
		{
			vector< DAGNode * > ret_temp;
			map< eNodeID , eNodeID > control_loop;
			for ( auto link : d_paired_node->d_links )
			{
				if ( control_loop.count( (link->d_pDestination)->d_node_uniq_id ) == 0 )
				{
					ret_temp.push_back( link->d_pDestination );
					control_loop[ (link->d_pDestination)->d_node_uniq_id ] = (link->d_pDestination)->d_node_uniq_id;
				}
			}
			do
			{
				vector< DAGNode * > last_ret_temp( ret_temp );
				ret_temp.clear();
				for ( auto & node : last_ret_temp )
				{
					for ( auto & link : node->d_links )
					{
						if ( control_loop.count( (link->d_pDestination)->d_node_uniq_id ) == 0 )
						{
							ret_temp.push_back( link->d_pDestination );
							control_loop[ (link->d_pDestination)->d_node_uniq_id ] = (link->d_pDestination)->d_node_uniq_id;
						}
					}
				}
				for ( auto & node : last_ret_temp )
				{
					desc_ordered.push_back( node );
				}

			} while ( !ret_temp.empty() );
		}
		//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose , "#(%d) " , __LINE__ );

		// اضافه دیداکت بعدا باز بین بچه ها تقسیم می شود
		double extra_deduct_remain = 0.0; // اگر تسکی باشد که زمان ددلاینش نتواند پاسخ کسری را بدهد کسری در این متغیر ریخته می شود
		bool b_any_action_appear_in_loop = false;
		// infinite loop
		do
		{
			double total_subtasks_quota = 0;
			for ( const auto & ref_node : desc_ordered )
			{
				if ( ref_node->d_task_deadline.d_task_spare_duration_quota > 0 )
				{
					// زمان اضافی از تسک هایی گرفته می شود که اصلا زمان داشته باشند
					total_subtasks_quota += ref_node->d_task_deadline.d_task_quota_duration_based_on_process_need;
				}
			}
			if ( extra_deduct_remain > 0.0 )
			{
				total_deduct_time = extra_deduct_remain;
				extra_deduct_remain = 0;
			}
			b_any_action_appear_in_loop = false;
			for ( const auto & ref_node : desc_ordered )
			{
				if ( ref_node->d_task_deadline.d_task_spare_duration_quota > 0 )
				{
					double deduct_task_quota =
							ref_node->d_task_deadline.d_task_quota_duration_based_on_process_need * total_deduct_time / total_subtasks_quota;

					if ( deduct_task_quota < 0 )
					{
						XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
					}

					if ( deduct_task_quota <= ref_node->d_task_deadline.d_task_spare_duration_quota )
					{
						ref_node->d_task_deadline.d_task_spare_duration_quota -= deduct_task_quota;
						deduct_task_quota = 0;
						b_any_action_appear_in_loop = true;

						//if ( SETTINGS_FAST()->d_enable_Task_Deadline_Quota_notify )
						//{
						//	XBT_INFO_EMPTY_LINE();
						//	XBT_INFO( "#(%d) update task_spare_time_duration_quota -" , __LINE__ );
						//	if ( SETTINGS_FAST()->d_dump_node_deadline_spare_time )
						//	{
						//		ref_node->dump( true , false );
						//	}
						//}
					}
					else
					{
						deduct_task_quota -= ref_node->d_task_deadline.d_task_spare_duration_quota;
						ref_node->d_task_deadline.d_task_spare_duration_quota = 0;
						b_any_action_appear_in_loop = true;

						//if ( SETTINGS_FAST()->d_enable_Task_Deadline_Quota_notify )
						//{
						//	XBT_INFO_EMPTY_LINE();
						//	XBT_INFO( "#(%d) update task_spare_time_duration_quota -" , __LINE__ );
						//	if ( SETTINGS_FAST()->d_dump_node_deadline_spare_time )
						//	{
						//		ref_node->dump( true , false );
						//	}
						//}
					}
					if ( deduct_task_quota > 0 )
					{
						extra_deduct_remain += deduct_task_quota;
					}
				}
			}
		} while ( extra_deduct_remain > 0 && b_any_action_appear_in_loop ); // اینقدر بچرخ و از بچه ها زمان کسر کن تا بالاخره ددلاین هاشون درست شه
		//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose , "#(%d) " , __LINE__ );
	}
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

	// 14020916 چرا باید زمان ددلاین را کم کنم
	d_task_soft_deadline_time = d_task_actual_complete_execution_time; // بعد از به ارث رساندت زمان اضافه یا کسر کردن زمان اضافه تایم ددلان را برابر مقدار واقعی می کنیم تا بعدی ها را بشود اصلاح کرد
	// refresh soft deadline time

	//XBT_INFO( "#(%d) %d  %f" , __LINE__ , this->d_paired_node->d_dax_id , d_paired_node->d_task_deadline->d_task_soft_deadline_time );

	// reset sub tasks deadline
	// hierarchical assignment
	// infinite loop
	if ( 1 )
	{
		vector< DAGNode * > ret_temp;
		map< eNodeID , eNodeID > control_loop;
		for ( auto link : d_paired_node->d_links )
		{
			if ( control_loop.count( (link->d_pDestination)->d_node_uniq_id ) == 0 )
			{
				ret_temp.push_back( link->d_pDestination );
				control_loop[ (link->d_pDestination)->d_node_uniq_id ] = (link->d_pDestination)->d_node_uniq_id;
			}
		}
		do
		{
			vector< DAGNode * > last_ret_temp( ret_temp );
			ret_temp.clear();
			for ( auto & node : last_ret_temp )
			{
				for ( auto & link : node->d_links )
				{
					if ( control_loop.count( (link->d_pDestination)->d_node_uniq_id ) == 0 )
					{
						ret_temp.push_back( link->d_pDestination );
						control_loop[ (link->d_pDestination)->d_node_uniq_id ] = (link->d_pDestination)->d_node_uniq_id;
					}
				}
			}
			for ( auto & node : last_ret_temp )
			{
				node->d_task_deadline.d_task_soft_deadline_time = -1;
			}

		} while ( !ret_temp.empty() );
	}
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );


	// refresh subtask deadline
	// hierarchical assignment
	vector< DAGNode * > inlist_node( { this->d_paired_node } );
	map< eNodeID , eNodeID > control_duplicate;
	control_duplicate[ d_paired_node->d_node_uniq_id ] = d_paired_node->d_node_uniq_id;
	////for ( auto & link : this->d_paired_node->d_links )
	////{
	////	if ( control.count( link->d_destination->d_node_uniq_id ) == 0 )
	////	{
	////		inlist_node.push_back( link->d_destination );
	////		control[ link->d_destination->d_node_uniq_id ] = link->d_destination->d_node_uniq_id;
	////	}
	////}

	// infinite loop
	while ( !inlist_node.empty() )
	{
		vector< DAGNode * > temp_inlist_node( inlist_node );
		inlist_node.clear();

		for ( auto & node : temp_inlist_node )
		{
			for ( auto & link : node->d_links )
			{
				bool b_all_parent_done = true;
				for ( auto & parent : (link->d_pDestination)->d_parents ) // مطمئن می شویم نود پدر در گراف جدید ساخته شده باشد
				{
					if ( parent->d_task_deadline.d_task_soft_deadline_time < 0 )
					{
						b_all_parent_done = false;
						break;
					}
				}
				if ( b_all_parent_done && control_duplicate.count( (link->d_pDestination)->d_node_uniq_id ) == 0 )
				{
					inlist_node.push_back( link->d_pDestination );
					control_duplicate[ (link->d_pDestination)->d_node_uniq_id ] = (link->d_pDestination)->d_node_uniq_id;
				}
			}
		}

		for ( auto & node : temp_inlist_node )
		{
			if ( node->d_node_uniq_id == d_paired_node->d_node_uniq_id ) continue;
			assert( node->d_task_deadline.d_task_quota_duration_based_on_process_need >= 0 );
			assert( node->d_task_deadline.d_task_soft_deadline_time == -1 );
			if ( node->d_parents.size() == 0 )
			{
				node->d_task_deadline.d_task_soft_deadline_time =
					node->d_pOwnerDAG->d_dag_deadline.d_dag_actual_recieved_time +
					node->d_task_deadline.d_task_quota_duration_based_on_process_need +
					node->d_task_deadline.d_task_spare_duration_quota;
			}
			else
			{
				bool b_all_parents_deadline_valid = true;
				AbsoluteTime max_soft_deadline_time = -1;
				for ( auto & parent : node->d_parents )
				{
					//assert( parent->d_task_deadline.d_task_soft_deadline_time >= 0 );
					if ( parent->d_task_deadline.d_task_soft_deadline_time < 0 )
					{
						XBT_INFO( "#(%d) warning dagdx%d dx%d parentdx%d %f" , __LINE__ , node->d_pOwnerDAG->d_dag_dax_id , node->d_node_dax_id , parent->d_node_dax_id , node->d_task_deadline.d_task_soft_deadline_time );
					}
					if ( parent->d_task_deadline.d_task_soft_deadline_time < 0 )
					{
						b_all_parents_deadline_valid = false; // پیش امد یک نود داشتیم که والدی خیلی داخلی داشت و والد داخلی خالی شده بود و والد اولی که می خواست پرش کند کار گیر والذ داخلی می افتاد
						break;
					}
					if ( parent->d_task_deadline.d_task_soft_deadline_time > max_soft_deadline_time )
					{
						max_soft_deadline_time = parent->d_task_deadline.d_task_soft_deadline_time;
					}
				}
				if ( b_all_parents_deadline_valid )
				{
					assert( max_soft_deadline_time >= 0 );

					node->d_task_deadline.d_task_soft_deadline_time =
						max_soft_deadline_time +
						node->d_task_deadline.d_task_quota_duration_based_on_process_need +
						node->d_task_deadline.d_task_spare_duration_quota;
				}

				//XBT_INFO( "#(%d) %d  %f" , __LINE__ , node->d_dax_id , node->d_task_deadline.d_task_soft_deadline_time );
			}
		}
	};
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

	CATCH_TRY
}

////void Task_Deadline_Quota::decrease_time_duration_from_bundle( shared_ptr< vector< shared_ptr< DAGNode > > > & bundled_nodes_ordered ,
////															  SimulationDurationTime total_deduct_time )
////{
////	double extra_deduct_remain = 0.0; // اگر تسکی باشد که زمان ددلاینش نتواند پاسخ کسری را بدهد کسری در این متغیر ریخته می شود
////	bool b_any_action_appear_in_loop = false;
////	// infinite loop
////	do
////	{
////		double total_subtasks_quota = 0;
////		for ( const shared_ptr<DAGNode> & ref_node : *bundled_nodes_ordered.get() )
////		{
////			if ( ref_node->d_task_deadline->d_task_spare_duration_quota > 0 ) // 
////			{
////				// زمان اضافی از تسک هایی گرفته می شود که اصلا زمان داشته باشند
////				total_subtasks_quota += ref_node->d_task_deadline->d_task_quota_duration_based_on_process_need;
////			}
////		}
////		if ( extra_deduct_remain > 0.0 )
////		{
////			total_deduct_time = extra_deduct_remain;
////			extra_deduct_remain = 0;
////		}
////		b_any_action_appear_in_loop = false;
////		for ( const shared_ptr<DAGNode> & ref_node : *bundled_nodes_ordered.get() )
////		{
////			if ( ref_node->d_task_deadline->d_task_spare_duration_quota > 0 ) // 
////			{
////				double deduct_task_quota =
////					ref_node->d_task_deadline->d_task_quota_duration_based_on_process_need * total_deduct_time / total_subtasks_quota; // بر اساس میزان زمان پردازش هر تسک زمان اسپیر قابل کسر را بدست می اورد
////
////				if ( deduct_task_quota < 0 )
////				{
////					XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
////				}
////
////				if ( deduct_task_quota <= ref_node->d_task_deadline->d_task_spare_duration_quota )
////				{
////					ref_node->d_task_deadline->d_task_spare_duration_quota -= deduct_task_quota;
////					deduct_task_quota = 0;
////					b_any_action_appear_in_loop = true;
////
////					if ( SETTINGS_FAST()->d_enable_Task_Deadline_Quota_notify )
////					{
////						XBT_INFO_EMPTY_LINE();
////						XBT_INFO( "#(%d) update task_spare_time_duration_quota -" , __LINE__ );
////						if ( SETTINGS_FAST()->d_dump_node_deadline_spare_time )
////						{
////							ref_node->dump( true , false );
////						}
////					}
////				}
////				else
////				{
////					deduct_task_quota -= ref_node->d_task_deadline->d_task_spare_duration_quota;
////					ref_node->d_task_deadline->d_task_spare_duration_quota = 0;
////					b_any_action_appear_in_loop = true;
////
////					if ( SETTINGS_FAST()->d_enable_Task_Deadline_Quota_notify )
////					{
////						XBT_INFO_EMPTY_LINE();
////						XBT_INFO( "#(%d) update task_spare_time_duration_quota -" , __LINE__ );
////						if ( SETTINGS_FAST()->d_dump_node_deadline_spare_time )
////						{
////							ref_node->dump( true , false );
////						}
////					}
////				}
////
////				if ( deduct_task_quota > 0 )
////				{
////					extra_deduct_remain += deduct_task_quota;
////				}
////			}
////		}
////
////	} while ( extra_deduct_remain > 0 && b_any_action_appear_in_loop ); // اینقدر بچرخ و از بچه ها زمان کسر کن تا بالاخره ددلاین هاشون درست شه
////}

AbsoluteTime * Task_Deadline_Quota::get_absolute_soft_deadline( bool bIfNUllFromNow )
{
	if ( this->d_paired_node->d_pOwnerDAG->d_dag_deadline.isDagArrivedToBeSchedule() )
	{
		if ( d_task_soft_deadline_time < 0 || isnan( d_task_soft_deadline_time ) )
		{
			XBT_INFO( "#(%d) warning dagdx%d dx%d %f" , __LINE__ , this->d_paired_node->d_pOwnerDAG->d_dag_dax_id , this->d_paired_node->d_node_dax_id , d_task_soft_deadline_time );
		}
		assert( d_task_soft_deadline_time >= 0 );
	}

	if ( d_task_soft_deadline_time >= 0 ) return &d_task_soft_deadline_time;
	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_critical_diagnose , "absolute deadline from now" , "" );
	////if ( bIfNUllFromNow )
	////{
	////	return make_shared<AbsoluteTime>( SIMULATION_NOW_TIME + d_task_quota_duration_based_on_process_need + d_task_spare_duration_quota );
	////}
	return NULL;
}

void Task_Deadline_Quota::dump( bool inside_another )
{
	BEGIN_TRY
	if ( !inside_another ) XBT_INFO_EMPTY_LINE();
	XBT_INFO( "dump deadline task %s %s %s" ,
			  string_format( ( d_task_quota_duration_based_on_process_need >= 0 ? ", task_time_quota(%f s) " : "" ) , d_task_quota_duration_based_on_process_need ).c_str() ,
			  string_format( ( d_task_spare_duration_quota >= 0 ? ", task_spare(%f s) " : "" ) , d_task_spare_duration_quota ).c_str() ,
			  string_format( ( d_task_soft_deadline_time >= 0 ? ", task_deadline(at %f s) " : "" ) , d_task_soft_deadline_time ).c_str() );
	CATCH_TRY
}

#pragma endregion

#pragma endregion

#pragma region LOG

std::ofstream __log_file_; // Global Variable
std::ofstream __logTr_file_; // Global Variable
string __config_root_;

// log file

////shared_ptr< list< string > > _cached_log = NULL;
////string _last_writen_log;
SystemAbsoluteTime _last_writen_log_time = -1;
////SystemAbsoluteTime _last_writen_cache_time = -1;

void init_log_file( int iteration , const char * proot )
{
	if ( __config_root_.length() == 0 )
	{
		__config_root_ = proot;
	}
	if ( __log_file_.is_open() ) finish_log_file();
	cout << "\n#" << __LINE__ << " " << __config_root_ << endl;
	__log_file_.open( string_format( "%s/result%d.txt" , __config_root_.c_str() , iteration ).c_str() , std::ios::trunc );
	__log_file_ << "init " << get_time() << endl;
}
void finish_log_file()
{
	////_last_writen_cache_time = -1;
	_last_writen_log_time = -1;
	////if ( _cached_log != NULL )
	////{
	////	for ( auto & sline : *_cached_log.get() )
	////	{
	////		__log_file_ << sline;
	////	}
	////	_cached_log->clear();
	////	_cached_log = NULL;
	////}
	__log_file_.close();
}

std::string ___log( const char * fxn , int line_number , bool concattime , bool simulation_time_anouncer , const string & fmt )
{
	BEGIN_TRY

	time_t tnow;
	std::time( &tnow );

	////if ( SETTINGS_FAST()->d_use_cache_for_XBT_INFO && _cached_log == NULL )
	////{
	////	_cached_log = make_shared<list< string >>();
	////}

	struct tm * timeinfo;
	char buffer[ 15 ];
	timeinfo = localtime( &tnow );
	strftime( buffer , 20 , "%T" , timeinfo );

	bool b_kernel_mode = false;
	string stime = std::to_string( SIMULATION_NOW_TIME );
	if ( stime == "0.000000" )
	{
		stime = buffer;
		b_kernel_mode = true;
	}
	else
	{
		stime = string_format( "%s%s%s" , buffer , " - " , stime.c_str() );
	}

	vector<string> v{ /*::format("#(%d)" , line_number) ,*/ concattime ? stime + " - " : "" , fxn , strlen(fxn) > 0 ? " - " : "" , fmt };

	if ( fmt.rfind( LINE_SEPERATOR_SIGN ) != string::npos && fmt.length() == strlen(LINE_SEPERATOR_SIGN) )
	{
		v = vector<string> { "" };
	}

	string to_file = std::accumulate( v.begin() , v.end() , std::string{});

	if ( !simulation_time_anouncer )
	{
		if ( _last_writen_log_time >= 0 &&
			 difftime( tnow , _last_writen_log_time ) > 2 )
		{
			////if ( _cached_log )
			////{
			////	_cached_log->push_back( "\n" );
			////}
			////else
			{
				__log_file_ << "\n";
			}
		}
	}

	if ( simulation_time_anouncer )
	{
		if ( !b_kernel_mode /* && _last_writen_log_time >= 0 &&
			 difftime( tnow , _last_writen_log_time ) > SETTINGS_FAST()->d_simulation_time_anouncer_delay*/ )
		{
			_last_writen_log_time = tnow;
			__log_file_ << "\n" << to_file << std::endl;
		}
		return to_file;
	}

	////if ( _cached_log )
	////{
	////	_cached_log->push_back( to_file + "\n" );
	////}
	////else
	{
		__log_file_ << to_file << std::endl;
	}

	////if ( _cached_log != NULL && ( _last_writen_cache_time < 0 || difftime( tnow , _last_writen_cache_time ) > 60 ) )
	////{
	////	_last_writen_cache_time = tnow;
	////	for ( auto & sline : *_cached_log.get() )
	////	{
	////		__log_file_ << sline;
	////	}
	////	_cached_log->clear();
	////}
	//
	////_last_writen_log = to_file;
	_last_writen_log_time = tnow;

	return to_file;
	CATCH_TRY
	return string( "" );
}

// Transaction for sql

void init_logTr_file()
{
	if ( __logTr_file_.is_open() ) finish_log_file();
	__logTr_file_.open( string_format( "%s/log_transaction.txt" , __config_root_.c_str() ) , std::ios::trunc );
}
void finish_logTr_file()
{
	__logTr_file_.close();
}

std::string ___logTr( string text )
{
	BEGIN_TRY
		vector<string> v{ text };

		string to_file = std::accumulate( v.begin() , v.end() , std::string{} );

		//if (to_file.rfind("UPDATE", 0) == 0)

		__logTr_file_ << to_file << std::endl;
		return to_file;
	CATCH_TRY
	return string( "" );
}

#pragma endregion

#pragma region Global_config

shared_ptr< Global_config > Global_config::d_pActiveInstance = NULL; // Global Variable
map<void * , tuple<char * , int , int> > Global_config::d_allocation_counter;
bool Global_config::d_order_dump_result_and_stop_program = false;
AeAppMode Global_config::d_app_mode = AeAppMode::apmd_none;

Global_config::Global_config( int simulationIteration )
{
	d_simulationIteration = simulationIteration;
	//d_prev_loaded_config = NULL;
	if ( d_app_mode < AeAppMode::apmd_kernel_mode )
	{
		d_app_mode = AeAppMode::apmd_kernel_mode;
	}
}
shared_ptr< Global_config > Global_config::GetInstance( int simulationIteration )
{
	BEGIN_TRY
	time_t t_now;
	std::time( &t_now );

	if ( simulationIteration < 0 )
	{
		if ( d_pActiveInstance == NULL )
		{
			cout << "#(" << __LINE__ << ")" << " simulationIteration " << simulationIteration << endl;
			return GetInstance( 1 );
		}
		else
		{
			if ( _last_writen_log_time >= 0 &&
				 difftime( t_now , _last_writen_log_time ) > SETTINGS_FAST()->d_simulation_time_anouncer_delay )
			{
				d_pActiveInstance->dump_periodicly();
			}
		}
	}


	if ( simulationIteration > 0 )
	{
		if ( d_pActiveInstance == NULL || d_pActiveInstance->d_simulationIteration != simulationIteration )
		{
			assert( simulationIteration > 0 );
			if ( d_pActiveInstance != NULL )
			{
				d_pActiveInstance->closeConfig();
				d_pActiveInstance = NULL;
			}
			d_pActiveInstance = make_shared<Global_config>( simulationIteration );
			d_pActiveInstance->defaultConfig();
		}
		else
		{
			assert( d_pActiveInstance->d_simulationIteration > 0 );
		}
	}

	d_pActiveInstance->d_config_read_count++;
	static time_t last_ver_visit = 0;
	if ( last_ver_visit == 0 )
	{
		std::time( &last_ver_visit );
	}
	
	if ( d_pActiveInstance->d_ver.empty() || std::difftime( t_now , last_ver_visit ) > 15 )
	{
		boost::property_tree::ptree version_root;
		pt::read_json( string_format( "%s/config_ver.txt" , __config_root_.c_str() ) , version_root );
				
		std::string ver = version_root.get<std::string>( "ver" );
		if ( !d_pActiveInstance->d_ver.empty() && d_pActiveInstance->d_ver != ver )
		{
			cout << "#(" << __LINE__ << ")" << " why " << endl;
		}
		if ( d_pActiveInstance->d_ver != ver )
		{
			d_pActiveInstance->readConfig( __config_root_ );
			
			if ( !d_pActiveInstance->d_ver.empty() )
			{
				XBT_INFO( "config ver changed." , "" );
			}
			d_pActiveInstance->d_ver = ver; // last action after read and other stuff

			d_pActiveInstance->post_read_action();
		}

		std::time( &last_ver_visit );
	}
	CATCH_TRY
	return d_pActiveInstance;
}

void Global_config::dump_periodicly()
{
	if ( CBenchmark::d_current_benchmark != NULL &&
		 CBenchmark::d_current_benchmark->pDGG != NULL &&
		 CBenchmark::d_current_benchmark->pDGG->d_resume_iteration != NULL
		 && CBenchmark::d_current_benchmark->pDGG->d_resume_iteration )
	{
		map< int/*dag size*/ , int/*count*/ > _dag_count_enter;
		map< int/*dag size*/ , int/*count*/ > _dag_count_enter_finished;

		string sdump;
		//int64_t schedul_bundle_wait_count = 0;
		//int64_t schedul_bundle_scheduled_count = 0;
		if ( CBenchmark::d_current_benchmark != NULL )
		{
			for ( auto & dag : CBenchmark::d_current_benchmark->pDGG->d_dag_storage.d_dags )
			{
				if ( dag->d_dag_deadline.isDagArrivedToBeSchedule() )
				{
					_dag_count_enter[ dag->d_nodes_count ] += 1;
					if ( dag->d_dag_deadline.isDagFinishedExecution() )
					{
						_dag_count_enter_finished[ dag->d_nodes_count ] += 1;
					}
				}
				//schedul_bundle_wait_count += dag->d_schedul_bundle_wait_count;
				//schedul_bundle_scheduled_count += dag->d_schedul_bundle_scheduled_count;
			}
			int64_t schedul_bundle_wait_count = CBenchmark::d_current_benchmark->pVMM->d_statistics[ AeStatisticElement::stt_bundle_scheduling_result ][ STATISTICS_bundle_wait_count ].bundle_wait_count;
			int64_t schedul_bundle_scheduled_count = CBenchmark::d_current_benchmark->pVMM->d_statistics[ AeStatisticElement::stt_bundle_scheduling_result ][ STATISTICS_bundle_scheduled_count ].bundle_scheduled_count;

			sdump = string_format( "total_try_quality(%" PRId64 ") node_schedule_candidate_quality(%d) node_schedule_candidate_quality_max(%d) branch_and_bound(%d) _wait(%" PRId64 ")_schedul(%" PRId64 ") _sort(%d)" ,
								   CBenchmark::d_current_benchmark->pVMM->d_total_try_quality ,
								   //CBenchmark::d_current_benchmark->pDGG->d_in_each_sort_try_schedule ,
								   //CBenchmark::d_current_benchmark->pDGG->d_in_each_sort_try_schedule_max ,
								   //CBenchmark::d_current_benchmark->pVMM->d_last_subbundle_try_quality ,
								   //CBenchmark::d_current_benchmark->pVMM->d_max_subbundle_try_quality ,
								   CBenchmark::d_current_benchmark->pVMM->d_last_candidate_quality_count ,
								   CBenchmark::d_current_benchmark->pVMM->d_max_candidate_quality_count ,
								   CBenchmark::d_current_benchmark->pVMM->d_reject_vm_by_branch_and_bound ,
								   schedul_bundle_wait_count , schedul_bundle_scheduled_count , CBenchmark::d_current_benchmark->pDGG->d_sort_iteration );

			for ( auto & vm_stat : CBenchmark::d_current_benchmark->pVMM->d_statistics[ AeStatisticElement::stt_online_vm_count ] )
			{
				sdump += string_format( " vmtype(%d)↓(%d)↻(%d)^(%d)" ,
										vm_stat.first.ii ,
										std::get<0>( vm_stat.second.online_vm_count ) ,
										std::get<0>( vm_stat.second.online_vm_count ) - std::get<1>( vm_stat.second.online_vm_count ) ,
										std::get<2>( vm_stat.second.online_vm_count ) );
			}


			for ( auto & bundle : CBenchmark::d_current_benchmark->pVMM->d_statistics[AeStatisticElement::stt_bundle_task_count_frequency] )
			{
				sdump += string_format( " bundlesize(%d)(%d)" , bundle.first.ii , bundle.second.bundle_task_count );
			}
			for ( auto & bundle : CBenchmark::d_current_benchmark->pVMM->d_statistics[AeStatisticElement::stt_quality_factor_sign_count] )
			{
				sdump += string_format( " quality_sign(%d)(%d)" , bundle.first.ii , bundle.second.quality_factor_sign_count );
			}
			//if ( d_pActiveInstance->d_dump_top_quality_vm_stat )
			//{
			//	for ( auto & bundle : CBenchmark::d_current_benchmark->pVMM->d_top_quality_vm_stat )
			//	{
			//		sdump += string_format( " top_quality(%s)(%d)" , bundle.first.c_str() , bundle.second );
			//	}
			//}

		}
		for ( auto & dag : _dag_count_enter )
		{
			sdump += string_format( " dag(∞%d)↓(%d)↑(%d)" , dag.first , dag.second , _dag_count_enter_finished[ dag.first ] );
		}
		//for ( auto & vm : _vm_type_count )
		//{
		//	sdump += string_format( " vm(%d)➧(%d)⥀(%d)" , vm.first , vm.second , _vm_type_idle_count[vm.first] );
		//}

		___LOG_SIMULATION_TIME_ANOUNCER( sdump.c_str() );
	}
}

void Global_config::defaultConfig()
{
	d_config_count = 1;
	d_repeat_each_config = 1;
	d_config_write_count = 0;
	d_config_read_count = 0;
	d_tags = "";

	d_dax_file_root = "/mnt/c/simgrid/simgrid-3.32/out/build/WSL-GCC-Debug/examples/c/actor-create";
	d_dax1_file_path = "$dax_root$/DAX1_frombase.txt";
	d_dax2_file_path = "$dax_root$/DAX2_fromDAGS.txt";
	d_backup_dag_guid_tmp_path = "$dax_root$/backup_dag_guid.tmp";

	//d_config_file_root = "/mnt/c/simgrid/simgrid-3.32/out/build/WSL-GCC-Debug";
	d_config1_file_path = "$config_root$/config1.txt";

	d_init_Vmm_DGG = false;
	d_Mode_MAKE_DAX2_FOR_SIMULATION = false;
	d_make_dag_guid_config = false;
	d_make_dag_n_guid_config = 20;
	d_use_dag_guid_config_cache = true;

	d_sort_schedule_list_policy_ver = 2;
	d_use_wait_policy = true;
	d_wait_policy_ver = 4;
	d_isSuitableForAnyMachine__ver = 2;
	d_break_after_bundle_not_suitable = true;
	d_use_least_price_vm_to_lease_new_vm = true;
	d_quality_ratio_calculation_ver = 2;
	d_EPSM_algorithm_ver = 2;
	d_add_one_dag_per_x_second = 60; // default 60.0
	d_greedy_expantion_ver = 1;

	d_schedule_periode_delay_time = 10; // default 2 * 60
	d_elapse_time_between_remain_time_estimation = 90; // second
	d_reset_minimum_estimate_time_consumption = -1;
	d_reset_maximum_estimate_time_consumption = -1;
	d_region_count = 4; // رجیون زیاد باعث طولانی شدن بررسی کوالیتی می شود
	d_vm_per_region_count = 3000;
	d_shuffle_candidate_qualities = true;
	d_sort_candidate_qualities_ver = 1; // very very important
	d_use_poisson_distribution_for_next_add_dag_time = true;
	d_shuffle_dags_after_loaded_to_storage = true;
	d_reset_values_type = 2;
	d_VIRTUALMACHINE_WARMUP_TIME_SEC = 100;  // 100.0 /*second*/
	d_VM_LEASED_CYCLE_TIME_SEC = 3600;   // 3600.0 /*sec*/
	d_DAG_GENERATOR_HOST_SPEED = 1e10;   // 1e10 /*not usable for now*/
	d_VMM_HOST_SPEED = 1e10;   // 1e10 /*not usable for now*/
	d_GLOBAL_STORAGE_MEMORY_SPEED = 1e10;   // 1e10
	d_CONTAINER_INITIALIZATION_DALEY_TIME = 10;   // 10.0 /*sec*/
	d_echo_XBT_INFO_OVERRIDEABLE = AeDumpLevel::dump_until_simulation_app_mode;
	d_system_alive_check = AeDumpLevel::to_see_system_is_alive;
	d_dump_bundle_if_estimation_and_real_execution_time_different_larger_than = -1;
	d_use_deprovisioning_vm_policy = true;
	d_deprovision_delay_duration = 60;
	d_deprovisioning_delay_estimate = 5;
	d_PROV_POLLING_TIME = 100;
	d_dump_deprovision_count_if_greater_than = -1;
	d_increase_deadline_after_no_machine_match = 1;
	d_elapsed_time_in_long_loop = 15;
	d_SectionTimeKeeper_minimum_duration_to_dump_slow_part_in_milisecond = 60000;
	d_simulation_time_anouncer_delay = 30;
	d_lan_bandwidth = LAN_BANDWIDTH;
	d_all_machine_type_memory_access_speed = MEMORY_ACCESS_SPEED_POSTFIX( 2 );
	d_low_memory_mode = true;
	d_calculate_katz = true;
	d_override_cumulative_sleep = -1;
	d_declare_dag_finish_each_n_iteration = -1;
	d_estimation_ver = 0;
	d_calculate_dag_execution_duration_ver = 2;
	d_max_bundle_capacity = -1;
	d_resource_heterogeneity = 2; // 1->2 vm type , 2->4 vm type
	d_use_free_slot_policy = true;
	d_vm_share_policy_ver = 1; // very important

	d_notify_init_VManagement_finished = true;
	d_run_estimator = true;
	d_write_function_name_in_dump = true;
	d_warn_if_some_vm_type_are_not_avaiable_for_leased = true;
	d_dump_each_simulation_iteration_result = true;
	d_notify_callback_actor_scheduler_finished = true;
	d_notify_dag_loaded = true;
	d_add_dax2_graph = true; // true
	d_notify_callback_actor_DGGenerator_finished = true;
	d_warn_impossible_condition = true;
	d_force_dump_config = false;
	d_echo_add_dag = false;
	d_echo_nextTime_add_dag = false;
	d_order_stop_loop_dag_storage_add = false;
	d_order_stop_loop_dag_schedule = false;
	//d_order_dump_result_and_stop_program = false;
	d_notify_dag_first_time_scheduled = false;
	d_notify_dag_new_node_to_schedule = false;
	d_echo_Ready2Schedule_node_list = false;
	d__echo_one_time__Ready2Schedule_node_list = false;
	d_dump_dag_storage_d_dags_in_init_DAGenerator = false;
	d_dump_at_start_to_use_cycle_mgr_report = false;
	d_dump_at_end_use_cycle_mgr_report = false;
	d_echo__calculate_quality_ratio__value_chack = false;
	d_echo__callback_actor_container__start_execute = false;
	d_echo__callback_actor_container__done_execute = false;
	d_echo__callback_actor_container__done_execute_dump_bundle = false;
	d_dump_region_after_creation = false;
	d_echo__scheduleOnVM__schedule_new_node = false;
	d_echo__scheduleOnVM__nearest_node = false;
	d_echo__scheduleOnVM__try_to_find_Provisioned_vm = false;
	d_echo__scheduleOnVM__try_to_find_Provisioned_vm_calculate_return_false = false;
	d_echo__scheduleOnVM__try_to_find_UNprovisioned_vm = false;
	d_echo__scheduleOnVM__dump_calc_ratio = false;
	d_echo__scheduleOnVM__details = false;
	d_enable_DAG_Deadline_Quota_notify__declare_dag_start_time = false;
	d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__finish_declare = false;
	d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__status_dag_to_deadline = false;
	d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__dump_dag = false;
	d_enable_Task_Deadline_Quota_notify = false;
	d_echo_minimum_maximum_calc_proc_dag_on_vm = false;
	d_echo_average_deadline_calced_for_dag = false;
	d_dump_dag_with_long_name = true;
	d_dump_Ready2ScheduleNode_order_priority = false;
	d_echo__time_comsume_to_process_on__deadline = false;
	d_echo_one_time_qualities_with_wait_policy = false;
	d_dump_all_qualities_in_schedule_node = false;
	d_echo__scheduleOnVM__top_quality_with_wait_policy_details = false;
	d_echo__scheduleOnVM__top_quality_non_wait_policy_details = false;
	d_echo__scheduleOnVM__top_quality_with_wait_policy_details_full_dag_dump = false;
	d_echo__scheduleOnVM__top_quality_non_wait_policy_details_full_dag_dump = false;
	d_dump_node_deadline_spare_time = false;
	d_print_bundle_internal_externals = false;
	d_echo_target_to_allocate_new_vm_in_EPSM = false;
	d_echo_target_to_allocate_new_vm_in_EPSM_dump_quality = false;
	d_echo_target_to_allocate_new_vm_in_EPSM_dump_quality_full_dag_dump = false;
	d_use_cache_for_XBT_INFO = false;
	d_dump_top_quality_vm_stat = false;


	//d_enable_all_dump_echo_notif_just_in_runtime = false;
	//d_dump_one_uncomplete_dag = false;
	//d_dump_how_much_vm_leased_for_bundle_scheduling_in_iteration = false;

	d_max_ranje_DAG_Type = 4;
}

void Global_config::readConfig( string sroot )
{
	//if ( d_prev_loaded_config != NULL ) d_prev_loaded_config = NULL;

	boost::property_tree::ptree root;

	//{
	//	ifstream f( string_format( "%s/config%d.txt" , sroot.c_str() , d_simulationIteration ) );
	//	if ( !f.good() )
	//	{
	//		cout << "#" << __LINE__ << " make default config at" << string_format( "%s/config%d.txt" , sroot.c_str() , d_simulationIteration ) << endl;
	//		writeConfig();
	//	}
	//}

	try
	{
		pt::read_json( string_format( "%s/config%d.txt" , sroot.c_str() , d_simulationIteration ) , root );
		d_config_file_root = sroot;
	}
	catch ( ... )
	{
		assert( 0 );
		return;
	}

	BEGIN_TRY
		d_config_count = root.get<int>( "d_config_count" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_repeat_each_config = root.get<int>( "d_repeat_each_config" );
	CATCH_TRY_SILENT
		

	BEGIN_TRY
		d_config_write_count = root.get<uint64_t>( "d_config_write_count" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_config_read_count = root.get<uint64_t>( "d_config_read_count" );
	CATCH_TRY_SILENT
		
	BEGIN_TRY
		d_config_name = root.get<string>( "d_config_name" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_tags = root.get<string>( "d_tags" );
	CATCH_TRY_SILENT

	//BEGIN_TRY
	//	d_enable_all_dump_echo_notif_just_in_runtime = root.get<bool>( "d_enable_all_dump_echo_notif_just_in_runtime" );
	//CATCH_TRY_SILENT

	BEGIN_TRY
		d_force_dump_config = root.get<bool>( "d_force_dump_config" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_init_Vmm_DGG = root.get<bool>( "d_init_Vmm_DGG" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_Mode_MAKE_DAX2_FOR_SIMULATION = root.get<bool>( "d_Mode_MAKE_DAX2_FOR_SIMULATION" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_make_dag_guid_config = root.get<bool>( "d_make_dag_guid_config" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_use_dag_guid_config_cache = root.get<bool>( "d_use_dag_guid_config_cache" );
	CATCH_TRY_SILENT
		

	BEGIN_TRY
		d_make_dag_n_guid_config = root.get<int>( "d_make_dag_n_guid_config" );
	CATCH_TRY_SILENT
		
		

	BEGIN_TRY
		d_echo_add_dag = root.get<bool>( "d_echo_add_dag" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo_nextTime_add_dag = root.get<bool>( "d_echo_nextTime_add_dag" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_notify_callback_actor_DGGenerator_finished = root.get<bool>( "d_notify_callback_actor_DGGenerator_finished" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_order_stop_loop_dag_storage_add = root.get<bool>( "d_order_stop_loop_dag_storage_add" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_order_stop_loop_dag_schedule = root.get<bool>( "d_order_stop_loop_dag_schedule" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_order_dump_result_and_stop_program = root.get<bool>( "d_order_dump_result_and_stop_program" );
	CATCH_TRY_SILENT
		

	BEGIN_TRY
		d_notify_dag_first_time_scheduled = root.get<bool>( "d_notify_dag_first_time_scheduled" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_notify_dag_new_node_to_schedule = root.get<bool>( "d_notify_dag_new_node_to_schedule" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_declare_dag_finish_each_n_iteration = root.get<int>( "d_declare_dag_finish_each_n_iteration" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo_Ready2Schedule_node_list = root.get<bool>( "d_echo_Ready2Schedule_node_list" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d__echo_one_time__Ready2Schedule_node_list = root.get<bool>( "d__echo_one_time__Ready2Schedule_node_list" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_dump_dag_storage_d_dags_in_init_DAGenerator = root.get<bool>( "d_dump_dag_storage_d_dags_in_init_DAGenerator" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_add_one_dag_per_x_second = root.get<double>( "d_add_one_dag_per_x_second" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_dump_at_start_to_use_cycle_mgr_report = root.get<bool>( "d_dump_at_start_to_use_cycle_mgr_report" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_dump_at_end_use_cycle_mgr_report = root.get<bool>( "d_dump_at_end_use_cycle_mgr_report" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo__calculate_quality_ratio__value_chack = root.get<bool>( "d_echo__calculate_quality_ratio__value_chack" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo__callback_actor_container__start_execute = root.get<bool>( "d_echo__callback_actor_container__start_execute" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo__callback_actor_container__done_execute = root.get<bool>( "d_echo__callback_actor_container__done_execute" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo__callback_actor_container__done_execute_dump_bundle = root.get<bool>( "d_echo__callback_actor_container__done_execute_dump_bundle" );
	CATCH_TRY_SILENT
		
	BEGIN_TRY
		d_dump_region_after_creation = root.get<bool>( "d_dump_region_after_creation" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_notify_init_VManagement_finished = root.get<bool>( "d_notify_init_VManagement_finished" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo__scheduleOnVM__schedule_new_node = root.get<bool>( "d_echo__scheduleOnVM__schedule_new_node" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo__scheduleOnVM__nearest_node = root.get<bool>( "d_echo__scheduleOnVM__nearest_node" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo__scheduleOnVM__try_to_find_Provisioned_vm = root.get<bool>( "d_echo__scheduleOnVM__try_to_find_Provisioned_vm" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo__scheduleOnVM__try_to_find_Provisioned_vm_calculate_return_false = root.get<bool>( "d_echo__scheduleOnVM__try_to_find_Provisioned_vm_calculate_return_false" ); 
	CATCH_TRY_SILENT

		

	BEGIN_TRY
		d_echo__scheduleOnVM__try_to_find_UNprovisioned_vm = root.get<bool>( "d_echo__scheduleOnVM__try_to_find_UNprovisioned_vm" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo__scheduleOnVM__dump_calc_ratio = root.get<bool>( "d_echo__scheduleOnVM__dump_calc_ratio" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo__scheduleOnVM__details = root.get<bool>( "d_echo__scheduleOnVM__details" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_VIRTUALMACHINE_WARMUP_TIME_SEC = root.get<double>( "d_VIRTUALMACHINE_WARMUP_TIME_SEC" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_VM_LEASED_CYCLE_TIME_SEC = root.get<double>( "d_VM_LEASED_CYCLE_TIME_SEC" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_DAG_GENERATOR_HOST_SPEED = root.get<double>( "d_DAG_GENERATOR_HOST_SPEED" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_VMM_HOST_SPEED = root.get<double>( "d_VMM_HOST_SPEED" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_GLOBAL_STORAGE_MEMORY_SPEED = root.get<double>( "d_GLOBAL_STORAGE_MEMORY_SPEED" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_CONTAINER_INITIALIZATION_DALEY_TIME = root.get<double>( "d_CONTAINER_INITIALIZATION_DALEY_TIME" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_schedule_periode_delay_time = root.get<double>( "d_schedule_periode_delay_time" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_enable_DAG_Deadline_Quota_notify__declare_dag_start_time = root.get<bool>( "d_enable_DAG_Deadline_Quota_notify__declare_dag_start_time" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__finish_declare = root.get<bool>( "d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__finish_declare" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__status_dag_to_deadline = root.get<bool>( "d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__status_dag_to_deadline" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__dump_dag = root.get<bool>( "d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__dump_dag" ); 
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_enable_Task_Deadline_Quota_notify = root.get<bool>( "d_enable_Task_Deadline_Quota_notify" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_region_count = root.get<int>( "d_region_count" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_vm_per_region_count = root.get<int>( "d_vm_per_region_count" );
	CATCH_TRY_SILENT

	BEGIN_TRY
		d_add_dax2_graph = root.get<bool>( "d_add_dax2_graph" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo_minimum_maximum_calc_proc_dag_on_vm = root.get<bool>( "d_echo_minimum_maximum_calc_proc_dag_on_vm" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo_average_deadline_calced_for_dag = root.get<bool>( "d_echo_average_deadline_calced_for_dag" ); 
	CATCH_TRY_SILENT

	BEGIN_TRY
		d_reset_minimum_estimate_time_consumption = root.get<int>( "d_reset_minimum_estimate_time_consumption" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_reset_maximum_estimate_time_consumption = root.get<int>( "d_reset_maximum_estimate_time_consumption" );
	CATCH_TRY_SILENT
		

	BEGIN_TRY
		d_notify_dag_loaded = root.get<bool>( "d_notify_dag_loaded" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_dump_dag_with_long_name = root.get<bool>( "d_dump_dag_with_long_name");
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_warn_impossible_condition = root.get<bool>( "d_warn_impossible_condition");
	CATCH_TRY_SILENT
	//BEGIN_TRY
	//	d_dump_one_uncomplete_dag = root.get<bool>( "d_dump_one_uncomplete_dag" );
	//CATCH_TRY_SILENT
	BEGIN_TRY
		d_elapse_time_between_remain_time_estimation = root.get<double>( "d_elapse_time_between_remain_time_estimation" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_dump_Ready2ScheduleNode_order_priority = root.get<bool>( "d_dump_Ready2ScheduleNode_order_priority" );
	CATCH_TRY_SILENT
	//BEGIN_TRY
	//	d__time_comsume_to_process_on__use_methode_to_calculate_between_min_and_max = root.get<int>( "d__time_comsume_to_process_on__use_methode_to_calculate_between_min_and_max" );
	//CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo__time_comsume_to_process_on__deadline = root.get<bool>( "d_echo__time_comsume_to_process_on__deadline" );
	CATCH_TRY_SILENT
	//BEGIN_TRY
		//d__init_vm__use_random_methode_for_VM_Type = root.get<bool>( "d__init_vm__use_random_methode_for_VM_Type" );
	//CATCH_TRY_SILENT
	BEGIN_TRY
		d_shuffle_candidate_qualities = root.get<bool>( "d_shuffle_candidate_qualities" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_use_poisson_distribution_for_next_add_dag_time = root.get<bool>( "d_use_poisson_distribution_for_next_add_dag_time" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_shuffle_dags_after_loaded_to_storage = root.get<bool>( "d_shuffle_dags_after_loaded_to_storage" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_warn_if_some_vm_type_are_not_avaiable_for_leased = root.get<bool>( "d_warn_if_some_vm_type_are_not_avaiable_for_leased" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo_XBT_INFO_OVERRIDEABLE = ( AeDumpLevel )root.get<int>( "d_echo_XBT_INFO_OVERRIDEABLE" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_system_alive_check = ( AeDumpLevel )root.get<int>( "d_system_alive_check" );
	CATCH_TRY_SILENT
		

	BEGIN_TRY
		d_sort_schedule_list_policy_ver = root.get<int>( "d_sort_schedule_list_policy_ver" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_dump_each_simulation_iteration_result = root.get<bool>( "d_dump_each_simulation_iteration_result" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_notify_callback_actor_scheduler_finished = root.get<bool>( "d_notify_callback_actor_scheduler_finished" );
	CATCH_TRY_SILENT
	
	BEGIN_TRY
		d_use_wait_policy = root.get<bool>( "d_use_wait_policy" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_wait_policy_ver = root.get<int>( "d_wait_policy_ver" );
	CATCH_TRY_SILENT

	BEGIN_TRY
		d_echo_one_time_qualities_with_wait_policy = root.get<bool>( "d_echo_one_time_qualities_with_wait_policy" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_dump_all_qualities_in_schedule_node = root.get<bool>( "d_dump_all_qualities_in_schedule_node" );
	CATCH_TRY_SILENT
		

	BEGIN_TRY
		d_echo__scheduleOnVM__top_quality_with_wait_policy_details = root.get<bool>( "d_echo__scheduleOnVM__top_quality_with_wait_policy_details" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo__scheduleOnVM__top_quality_non_wait_policy_details = root.get<bool>( "d_echo__scheduleOnVM__top_quality_non_wait_policy_details" );
	CATCH_TRY_SILENT

	BEGIN_TRY
		d_echo__scheduleOnVM__top_quality_with_wait_policy_details_full_dag_dump = root.get<bool>( "d_echo__scheduleOnVM__top_quality_with_wait_policy_details_full_dag_dump" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo__scheduleOnVM__top_quality_non_wait_policy_details_full_dag_dump = root.get<bool>( "d_echo__scheduleOnVM__top_quality_non_wait_policy_details_full_dag_dump" );
	CATCH_TRY_SILENT
		
	BEGIN_TRY
		d_dump_node_deadline_spare_time = root.get<bool>( "d_dump_node_deadline_spare_time" );
	CATCH_TRY_SILENT
	

	BEGIN_TRY
		d_reset_values_type = root.get<int>( "d_reset_values_type" );
	CATCH_TRY_SILENT
		
	BEGIN_TRY
		d_dump_bundle_if_estimation_and_real_execution_time_different_larger_than = root.get<double>( "d_dump_bundle_if_estimation_and_real_execution_time_different_larger_than" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_print_bundle_internal_externals = root.get<bool>( "d_print_bundle_internal_externals" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_EPSM_algorithm_ver = root.get<int>( "d_EPSM_algorithm_ver" );
	CATCH_TRY_SILENT

	BEGIN_TRY
		d_echo_target_to_allocate_new_vm_in_EPSM = root.get<bool>( "d_echo_target_to_allocate_new_vm_in_EPSM" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_echo_target_to_allocate_new_vm_in_EPSM_dump_quality = root.get<bool>( "d_echo_target_to_allocate_new_vm_in_EPSM_dump_quality" );
	CATCH_TRY_SILENT
		
	BEGIN_TRY
		d_echo_target_to_allocate_new_vm_in_EPSM_dump_quality_full_dag_dump = root.get<bool>( "d_echo_target_to_allocate_new_vm_in_EPSM_dump_quality_full_dag_dump" );
	CATCH_TRY_SILENT

	BEGIN_TRY
		d_deprovision_delay_duration = root.get<double>( "d_deprovision_delay_duration" );
	CATCH_TRY_SILENT
	
	BEGIN_TRY
		d_deprovisioning_delay_estimate = root.get<double>( "d_deprovisioning_delay_estimate" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_PROV_POLLING_TIME = root.get<double>( "d_PROV_POLLING_TIME" );
	CATCH_TRY_SILENT
		
	BEGIN_TRY
		d_dump_deprovision_count_if_greater_than = root.get<int>( "d_dump_deprovision_count_if_greater_than" );
	CATCH_TRY_SILENT

	//BEGIN_TRY
	//	d_reset_value_after_load_dax2 = root.get<bool>( "d_reset_value_after_load_dax2" );
	//CATCH_TRY_SILENT
		
	BEGIN_TRY
		d_write_function_name_in_dump = root.get<bool>( "d_write_function_name_in_dump" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_increase_deadline_after_no_machine_match = root.get<double>( "d_increase_deadline_after_no_machine_match" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_use_cache_for_XBT_INFO = root.get<double>( "d_use_cache_for_XBT_INFO" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_elapsed_time_in_long_loop = root.get<double>( "d_elapsed_time_in_long_loop" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_use_deprovisioning_vm_policy = root.get<bool>( "d_use_deprovisioning_vm_policy" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_SectionTimeKeeper_minimum_duration_to_dump_slow_part_in_milisecond = root.get<double>( "d_SectionTimeKeeper_minimum_duration_to_dump_slow_part_in_milisecond" );
	CATCH_TRY_SILENT
	//BEGIN_TRY
	//	d_dump_how_much_vm_leased_for_bundle_scheduling_in_iteration = root.get<bool>( "d_dump_how_much_vm_leased_for_bundle_scheduling_in_iteration" );
	//CATCH_TRY_SILENT
	BEGIN_TRY
		d_run_estimator = root.get<bool>( "d_run_estimator" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_simulation_time_anouncer_delay = root.get<double>( "d_simulation_time_anouncer_delay" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_lan_bandwidth = root.get<double>( "d_lan_bandwidth" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_all_machine_type_memory_access_speed = root.get<double>( "d_all_machine_type_memory_access_speed" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_isSuitableForAnyMachine__ver = root.get<int>( "d_isSuitableForAnyMachine__ver" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_break_after_bundle_not_suitable = root.get<bool>( "d_break_after_bundle_not_suitable" );
	CATCH_TRY_SILENT

	BEGIN_TRY
		d_use_least_price_vm_to_lease_new_vm = root.get<bool>( "d_use_least_price_vm_to_lease_new_vm" );
	CATCH_TRY_SILENT
		BEGIN_TRY
		d_low_memory_mode = root.get<bool>( "d_low_memory_mode" );
	CATCH_TRY_SILENT
		BEGIN_TRY
		d_calculate_katz = root.get<bool>( "d_calculate_katz" );
	CATCH_TRY_SILENT
		BEGIN_TRY
		d_override_cumulative_sleep = root.get<int>( "d_override_cumulative_sleep" );
	CATCH_TRY_SILENT
		BEGIN_TRY
		d_quality_ratio_calculation_ver = root.get<int>( "d_quality_ratio_calculation_ver" );
	CATCH_TRY_SILENT
		BEGIN_TRY
		d_dump_top_quality_vm_stat = root.get<bool>( "d_dump_top_quality_vm_stat" );
	CATCH_TRY_SILENT
		BEGIN_TRY
		d_estimation_ver = root.get<int>( "d_estimation_ver" );
	CATCH_TRY_SILENT
		BEGIN_TRY
		d_calculate_dag_execution_duration_ver = root.get<int>( "d_calculate_dag_execution_duration_ver" );
	CATCH_TRY_SILENT
		BEGIN_TRY
		d_max_ranje_DAG_Type = root.get<int>( "d_max_ranje_DAG_Type" );
	CATCH_TRY_SILENT
		
		

	BEGIN_TRY
		d_config1_file_path = root.get<string>( "d_config1_file_path" );
		d_config1_file_path = ReplaceAll( d_config1_file_path , "%" , "/" );
	CATCH_TRY_SILENT

	BEGIN_TRY
		d_dax_file_root = root.get<string>( "d_dax_file_root" );
		d_dax_file_root = ReplaceAll( d_dax_file_root , "%" , "/" );
	CATCH_TRY_SILENT
		
	BEGIN_TRY
		d_dax1_file_path = root.get<string>( "d_dax1_file_path" );
		d_dax1_file_path = ReplaceAll( d_dax1_file_path , "%" , "/" );
	CATCH_TRY_SILENT

	BEGIN_TRY
		d_dax2_file_path = root.get<string>( "d_dax2_file_path" );
		d_dax2_file_path = ReplaceAll( d_dax2_file_path , "%" , "/" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_backup_dag_guid_tmp_path = root.get<string>( "d_backup_dag_guid_tmp_path" );
		d_backup_dag_guid_tmp_path = ReplaceAll( d_backup_dag_guid_tmp_path , "%" , "/" );
	CATCH_TRY_SILENT
		

	BEGIN_TRY
		d_max_bundle_capacity = root.get<int>( "d_max_bundle_capacity" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_resource_heterogeneity = root.get<int>( "d_resource_heterogeneity" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_greedy_expantion_ver = root.get<int>( "d_greedy_expantion_ver" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_sort_candidate_qualities_ver = root.get<int>( "d_sort_candidate_qualities_ver" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_use_free_slot_policy = root.get<bool>( "d_use_free_slot_policy" );
	CATCH_TRY_SILENT
	BEGIN_TRY
		d_vm_share_policy_ver = root.get<int>( "d_vm_share_policy_ver" );
	CATCH_TRY_SILENT
		
	d_config1_file_path = ReplaceAll( d_config1_file_path , "$config_root$" , d_config_file_root );
		
	
	d_dax1_file_path = ReplaceAll( d_dax1_file_path , "$dax_root$" , d_dax_file_root );
	d_dax2_file_path = ReplaceAll( d_dax2_file_path , "$dax_root$" , d_dax_file_root );
	d_backup_dag_guid_tmp_path = ReplaceAll( d_backup_dag_guid_tmp_path , "$dax_root$" , d_dax_file_root );


	d_tags = make_config_tags();
}

string Global_config::make_config_tags()
{
	string sout;
	
	switch ( d_EPSM_algorithm_ver )
	{
		case 1: // EPSM
		case 3: // EPSM
		{
			sout += string_format( "EPSM%d" , d_EPSM_algorithm_ver );

			assert( d_greedy_expantion_ver == 1 );
			if ( d_use_wait_policy )
			{
				sout += string_format( " - wait%d" , d_wait_policy_ver );
			}
			else
			{
				sout += " - no wait";
			}
			
			break;
		}
		case 2: // EBSM
		{
			sout += string_format( "EBSM%d" , d_EPSM_algorithm_ver );

			switch ( d_sort_schedule_list_policy_ver )
			{
				case 1:
				{
					sout += string_format( " - orderKatz" , "");
					break;
				}
				case 2:
				{
					sout += string_format( " - orderKtzDedlne+" , "");
					break;
				}
				case 3:
				{
					sout += string_format( " - orderShufle" , "");
					break;
				}
				case 4:
				{
					sout += string_format( " - EDF" , "");
					break;
				}
				default:
				{
					assert( 0 );
				}
			}
			
			if ( d_use_wait_policy )
			{
				switch ( d_wait_policy_ver )
				{
					case 1:
					{
						sout += string_format( " - waitRatioFix" , "");
						break;
					}
					case 2:
					{
						sout += string_format( " - wait%d" , d_wait_policy_ver ); // wait with formula
						break;
					}
					case 3:
					{
						sout += string_format( " - wait%d" , d_wait_policy_ver ); // wait quality superiurity
						break;
					}
					case 4:
					{
						sout += string_format( " - wait+%d" , d_wait_policy_ver ); // wait quality race with other
						break;
					}
					default:
					{
						assert( 0 );
					}
				}
			}
			else
			{
				sout += " - nowait, """;
			}
			
			//switch ( d_isSuitableForAnyMachine__ver )
			//{
			//	case 0:
			//	{
			//		sout += string_format( " - SuitVMBad%d" , d_isSuitableForAnyMachine__ver );
			//		break;
			//	}
			//	case 1:
			//	{
			//		sout += string_format( " - SuitVMBad%d" , d_isSuitableForAnyMachine__ver );
			//		break;
			//	}
			//	case 2:
			//	{
			//		sout += string_format( " - SuitVM+%d" , d_isSuitableForAnyMachine__ver ); // to improve quality . 14021018
			//		break;
			//	}
			//	case 3:
			//	{
			//		sout += string_format( " - SuitVMBad%d" , d_isSuitableForAnyMachine__ver ); // to improve quality to positive
			//		break;
			//	}
			//	case 4:
			//	{
			//		sout += string_format( " - SuitVMBad%d" , d_isSuitableForAnyMachine__ver ); // اگر مثبت شد از اوت به بعد تباید منفی شود
			//		break;
			//	}
			//	default:
			//	{
			//		assert( 0 );
			//		break;
			//	}
			//}

			//sout += string_format( " - breakbundle%d" , d_break_after_bundle_not_suitable ); // true is default . 14021018
			//if ( d_use_least_price_vm_to_lease_new_vm )
			//{
			//	sout += string_format( " - leastVMPolicy" , "" ); // true is default . 14021018
			//}
			
			switch ( d_quality_ratio_calculation_ver )
			{
				case 1:
				{
					sout += string_format( " - nozaribQuality" , "" ); // 1 no zarib quality 
					break;
				}
				case 2:
				{
					sout += string_format( " - zaribQuality+%d" , d_quality_ratio_calculation_ver ); // 2 is default . 
					break;
				}
				default:
				{
					assert( 0 );
					break;
				}
			}
			
			switch ( d_greedy_expantion_ver )
			{
				case 1:
				{
					sout += string_format( " - Dijkstra+" , "");
					break;
				}
				case 2:
				{
					sout += string_format( " - BFS" , "");
					break;
				}
				default:
				{
					assert( 0 );
					break;
				}
			}

			sout += string_format( " - DAGDiversity%d" , d_max_ranje_DAG_Type );
			
			switch ( d_resource_heterogeneity )
			{
				case 1:
				{
					sout += string_format( " - Homogen" , "");
					break;
				}
				case 2:
				{
					sout += string_format( " - Heterogen+" , ""); // default
					break;
				}
				default:
				{
					assert( 0 );
					break;
				}
			}

			//sout += string_format( " - region%d" , d_region_count );
			
			switch ( d_sort_candidate_qualities_ver )
			{
				case 0:
				{
					sout += string_format( " - ShfleQulty" , "");
					break;
				}
				case 1:
				{
					sout += string_format( " - sortQultyDesc+" , ""); // default
					break;
				}
				default:
				{
					assert( 0 );
					break;
				}
			}

			switch ( d_vm_share_policy_ver )
			{
				case 1:
				{
					sout += string_format( " - sharePolicy+" , ""); // default
					break;
				}
				case 2:
				{
					sout += string_format( " - shareDag" , "");
					break;
				}
				case 3:
				{
					sout += string_format( " - shareType" , "");
					break;
				}
				default:
				{
					assert( 0 );
					break;
				}
			}

			if ( !d_use_free_slot_policy )
			{
				sout += string_format( " - noFreeSlot" , "");
			}
			if ( !d_calculate_katz )
			{
				sout += string_format( " - zeroKatz" , "" );
			}
			
			sout += string_format( " - WARMUP%s" , trim_floatnumber( d_VIRTUALMACHINE_WARMUP_TIME_SEC ).c_str() );
			sout += string_format( " - CONTAINER%s" , trim_floatnumber( d_CONTAINER_INITIALIZATION_DALEY_TIME ).c_str() );
			sout += string_format( " - VM_POLLING%s" , trim_floatnumber( d_PROV_POLLING_TIME ).c_str() );
			
			break;
		}
		default:
		{
			assert( 0 );
			break;
		}
	}

	d_tags = sout;

	return sout;
}

boost::property_tree::ptree Global_config::make_json()
{
	boost::property_tree::ptree root;

	root.put( "_" , "########################################################" );
	root.put( "d_config_name" , d_config_name );
	root.put( "d_tags" , d_tags );

	string temp_config1_file_path = d_config1_file_path;
	//temp_config1_file_path = ReplaceAll( temp_config1_file_path , d_config_file_root , "$config_root$" );
	temp_config1_file_path = ReplaceAll( temp_config1_file_path , "/" , "%" );
	root.put( "d_config1_file_path" , temp_config1_file_path );
	
	string temp_dax_file_root = d_dax_file_root;
	root.put( "d_dax_file_root" , ReplaceAll( temp_dax_file_root , "/" , "%" ) );
	
	string temp_dax1_file_path = d_dax1_file_path;
	temp_dax1_file_path = ReplaceAll( temp_dax1_file_path , d_dax_file_root , "$dax_root$" );
	temp_dax1_file_path = ReplaceAll( temp_dax1_file_path , "/" , "%" );
	root.put( "d_dax1_file_path" , temp_dax1_file_path );

	string temp_dax2_file_path = d_dax2_file_path;
	temp_dax2_file_path = ReplaceAll( temp_dax2_file_path , d_dax_file_root , "$dax_root$" );
	temp_dax2_file_path = ReplaceAll( temp_dax2_file_path , "/" , "%" );
	root.put( "d_dax2_file_path" , temp_dax2_file_path );

	string temp_backup_dag_guid_tmp_path = d_backup_dag_guid_tmp_path;
	temp_backup_dag_guid_tmp_path = ReplaceAll( temp_backup_dag_guid_tmp_path , d_dax_file_root , "$dax_root$" );
	temp_backup_dag_guid_tmp_path = ReplaceAll( temp_backup_dag_guid_tmp_path , "/" , "%" );
	root.put( "d_backup_dag_guid_tmp_path" , temp_backup_dag_guid_tmp_path );


	root.put( "d_config_count" , d_config_count );
	root.put( "d_repeat_each_config" , d_repeat_each_config );
	root.put( "d_init_Vmm_DGG" , d_init_Vmm_DGG );
	root.put( "d_Mode_MAKE_DAX2_FOR_SIMULATION" , d_Mode_MAKE_DAX2_FOR_SIMULATION );
	root.put( "d_make_dag_guid_config" , d_make_dag_guid_config );
	root.put( "d_use_dag_guid_config_cache" , d_use_dag_guid_config_cache );
	

	root.put( "__" , "########################################################" );
	root.put( "d_sort_schedule_list_policy_ver" , d_sort_schedule_list_policy_ver );
	root.put( "d_use_wait_policy" , d_use_wait_policy );
	root.put( "d_quality_ratio_calculation_ver" , d_quality_ratio_calculation_ver );
	root.put( "d_EPSM_algorithm_ver" , d_EPSM_algorithm_ver );
	root.put( "d_add_one_dag_per_x_second" , d_add_one_dag_per_x_second );
	root.put( "d_greedy_expantion_ver" , d_greedy_expantion_ver );
	root.put( "d_sort_candidate_qualities_ver" , d_sort_candidate_qualities_ver );
	root.put( "d_reset_values_type" , d_reset_values_type );
	root.put( "d_VIRTUALMACHINE_WARMUP_TIME_SEC" , d_VIRTUALMACHINE_WARMUP_TIME_SEC );
	root.put( "d_CONTAINER_INITIALIZATION_DALEY_TIME" , d_CONTAINER_INITIALIZATION_DALEY_TIME );
	root.put( "d_PROV_POLLING_TIME" , d_PROV_POLLING_TIME );
	root.put( "d_max_ranje_DAG_Type" , d_max_ranje_DAG_Type );
	root.put( "d_max_bundle_capacity" , d_max_bundle_capacity );
	root.put( "d_resource_heterogeneity" , d_resource_heterogeneity );
	root.put( "d_use_free_slot_policy" , d_use_free_slot_policy );
	root.put( "d_vm_share_policy_ver" , d_vm_share_policy_ver );

	root.put( "___" , "########################################################" );
	root.put( "d_wait_policy_ver" , d_wait_policy_ver );
	root.put( "d_isSuitableForAnyMachine__ver" , d_isSuitableForAnyMachine__ver );
	root.put( "d_break_after_bundle_not_suitable" , d_break_after_bundle_not_suitable );
	root.put( "d_use_least_price_vm_to_lease_new_vm" , d_use_least_price_vm_to_lease_new_vm );
	root.put( "d_schedule_periode_delay_time" , d_schedule_periode_delay_time );
	root.put( "d_elapse_time_between_remain_time_estimation" , d_elapse_time_between_remain_time_estimation );
	root.put( "d_reset_minimum_estimate_time_consumption" , d_reset_minimum_estimate_time_consumption );
	root.put( "d_reset_maximum_estimate_time_consumption" , d_reset_maximum_estimate_time_consumption );
	root.put( "d_region_count" , d_region_count );
	root.put( "d_vm_per_region_count" , d_vm_per_region_count );
	root.put( "d_shuffle_candidate_qualities" , d_shuffle_candidate_qualities );
	root.put( "d_use_poisson_distribution_for_next_add_dag_time" , d_use_poisson_distribution_for_next_add_dag_time );
	root.put( "d_shuffle_dags_after_loaded_to_storage" , d_shuffle_dags_after_loaded_to_storage );
	root.put( "d_VM_LEASED_CYCLE_TIME_SEC" , d_VM_LEASED_CYCLE_TIME_SEC );
	root.put( "d_DAG_GENERATOR_HOST_SPEED" , d_DAG_GENERATOR_HOST_SPEED );
	root.put( "d_VMM_HOST_SPEED" , d_VMM_HOST_SPEED );
	root.put( "d_GLOBAL_STORAGE_MEMORY_SPEED" , d_GLOBAL_STORAGE_MEMORY_SPEED );
	root.put( "d_echo_XBT_INFO_OVERRIDEABLE" , ( int )d_echo_XBT_INFO_OVERRIDEABLE );
	root.put( "d_system_alive_check" , ( int )d_system_alive_check );
	root.put( "d_dump_bundle_if_estimation_and_real_execution_time_different_larger_than" , d_dump_bundle_if_estimation_and_real_execution_time_different_larger_than );
	root.put( "d_use_deprovisioning_vm_policy" , d_use_deprovisioning_vm_policy );
	root.put( "d_deprovision_delay_duration" , d_deprovision_delay_duration );
	root.put( "d_deprovisioning_delay_estimate" , d_deprovisioning_delay_estimate );
	root.put( "d_dump_deprovision_count_if_greater_than" , d_dump_deprovision_count_if_greater_than );
	root.put( "d_increase_deadline_after_no_machine_match" , d_increase_deadline_after_no_machine_match );
	root.put( "d_elapsed_time_in_long_loop" , d_elapsed_time_in_long_loop );
	root.put( "d_SectionTimeKeeper_minimum_duration_to_dump_slow_part_in_milisecond" , d_SectionTimeKeeper_minimum_duration_to_dump_slow_part_in_milisecond );
	root.put( "d_simulation_time_anouncer_delay" , d_simulation_time_anouncer_delay );
	root.put( "d_lan_bandwidth" , d_lan_bandwidth );
	root.put( "d_all_machine_type_memory_access_speed" , d_all_machine_type_memory_access_speed );
	root.put( "d_low_memory_mode" , d_low_memory_mode );
	root.put( "d_calculate_katz" , d_calculate_katz );
	root.put( "d_override_cumulative_sleep" , d_override_cumulative_sleep );
	root.put( "d_declare_dag_finish_each_n_iteration" , d_declare_dag_finish_each_n_iteration );
	root.put( "d_estimation_ver" , d_estimation_ver );
	root.put( "d_calculate_dag_execution_duration_ver" , d_calculate_dag_execution_duration_ver );
	root.put( "d_make_dag_n_guid_config" , d_make_dag_n_guid_config );
	
	
	root.put( "____" , "########################################################" );
	root.put( "d_notify_init_VManagement_finished" , d_notify_init_VManagement_finished );
	root.put( "d_run_estimator" , d_run_estimator );
	root.put( "d_write_function_name_in_dump" , d_write_function_name_in_dump );
	root.put( "d_warn_if_some_vm_type_are_not_avaiable_for_leased" , d_warn_if_some_vm_type_are_not_avaiable_for_leased );
	root.put( "d_dump_each_simulation_iteration_result" , d_dump_each_simulation_iteration_result );
	root.put( "d_notify_callback_actor_scheduler_finished" , d_notify_callback_actor_scheduler_finished );
	root.put( "d_notify_dag_loaded" , d_notify_dag_loaded );
	root.put( "d_add_dax2_graph" , d_add_dax2_graph );
	root.put( "d_notify_callback_actor_DGGenerator_finished" , d_notify_callback_actor_DGGenerator_finished );
	root.put( "d_warn_impossible_condition" , d_warn_impossible_condition );
	//root.put( "d_enable_all_dump_echo_notif_just_in_runtime" , false );
	root.put( "d_force_dump_config" , false );
	root.put( "d_echo_add_dag" , d_echo_add_dag );
	root.put( "d_echo_nextTime_add_dag" , d_echo_nextTime_add_dag );
	root.put( "d_order_stop_loop_dag_storage_add" , false );
	root.put( "d_order_stop_loop_dag_schedule" , false );
	root.put( "d_order_dump_result_and_stop_program" , false );
	root.put( "d_notify_dag_first_time_scheduled" , d_notify_dag_first_time_scheduled );
	root.put( "d_notify_dag_new_node_to_schedule" , d_notify_dag_new_node_to_schedule );
	root.put( "d_echo_Ready2Schedule_node_list" , d_echo_Ready2Schedule_node_list );
	root.put( "d__echo_one_time__Ready2Schedule_node_list" , false );
	root.put( "d_dump_dag_storage_d_dags_in_init_DAGenerator" , d_dump_dag_storage_d_dags_in_init_DAGenerator );
	root.put( "d_dump_at_start_to_use_cycle_mgr_report" , d_dump_at_start_to_use_cycle_mgr_report );
	root.put( "d_dump_at_end_use_cycle_mgr_report" , d_dump_at_end_use_cycle_mgr_report );
	root.put( "d_echo__calculate_quality_ratio__value_chack" , d_echo__calculate_quality_ratio__value_chack );
	root.put( "d_echo__callback_actor_container__start_execute" , d_echo__callback_actor_container__start_execute );
	root.put( "d_echo__callback_actor_container__done_execute" , d_echo__callback_actor_container__done_execute );
	root.put( "d_echo__callback_actor_container__done_execute_dump_bundle" , d_echo__callback_actor_container__done_execute_dump_bundle );
	root.put( "d_dump_region_after_creation" , d_dump_region_after_creation );
	root.put( "d_echo__scheduleOnVM__schedule_new_node" , d_echo__scheduleOnVM__schedule_new_node );
	root.put( "d_echo__scheduleOnVM__nearest_node" , d_echo__scheduleOnVM__nearest_node );
	root.put( "d_echo__scheduleOnVM__try_to_find_Provisioned_vm" , d_echo__scheduleOnVM__try_to_find_Provisioned_vm );
	root.put( "d_echo__scheduleOnVM__try_to_find_Provisioned_vm_calculate_return_false" , d_echo__scheduleOnVM__try_to_find_Provisioned_vm_calculate_return_false );
	root.put( "d_echo__scheduleOnVM__try_to_find_UNprovisioned_vm" , d_echo__scheduleOnVM__try_to_find_UNprovisioned_vm );
	root.put( "d_echo__scheduleOnVM__dump_calc_ratio" , d_echo__scheduleOnVM__dump_calc_ratio );
	root.put( "d_echo__scheduleOnVM__details" , d_echo__scheduleOnVM__details );
	root.put( "d_enable_DAG_Deadline_Quota_notify__declare_dag_start_time" , d_enable_DAG_Deadline_Quota_notify__declare_dag_start_time );
	root.put( "d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__finish_declare" , d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__finish_declare );
	root.put( "d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__status_dag_to_deadline" , d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__status_dag_to_deadline );
	root.put( "d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__dump_dag" , d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__dump_dag );
	root.put( "d_enable_Task_Deadline_Quota_notify" , d_enable_Task_Deadline_Quota_notify );
	root.put( "d_echo_minimum_maximum_calc_proc_dag_on_vm" , d_echo_minimum_maximum_calc_proc_dag_on_vm );
	root.put( "d_echo_average_deadline_calced_for_dag" , d_echo_average_deadline_calced_for_dag );
	root.put( "d_dump_dag_with_long_name" , d_dump_dag_with_long_name );
	root.put( "d_dump_Ready2ScheduleNode_order_priority" , d_dump_Ready2ScheduleNode_order_priority );
	root.put( "d_echo__time_comsume_to_process_on__deadline" , d_echo__time_comsume_to_process_on__deadline );
	root.put( "d_echo_one_time_qualities_with_wait_policy" , false );
	root.put( "d_dump_all_qualities_in_schedule_node" , d_dump_all_qualities_in_schedule_node );
	root.put( "d_echo__scheduleOnVM__top_quality_with_wait_policy_details" , d_echo__scheduleOnVM__top_quality_with_wait_policy_details );
	root.put( "d_echo__scheduleOnVM__top_quality_non_wait_policy_details" , d_echo__scheduleOnVM__top_quality_non_wait_policy_details );
	root.put( "d_echo__scheduleOnVM__top_quality_with_wait_policy_details_full_dag_dump" , d_echo__scheduleOnVM__top_quality_with_wait_policy_details_full_dag_dump );
	root.put( "d_echo__scheduleOnVM__top_quality_non_wait_policy_details_full_dag_dump" , d_echo__scheduleOnVM__top_quality_non_wait_policy_details_full_dag_dump );
	root.put( "d_dump_node_deadline_spare_time" , d_dump_node_deadline_spare_time );
	root.put( "d_print_bundle_internal_externals" , d_print_bundle_internal_externals );
	root.put( "d_echo_target_to_allocate_new_vm_in_EPSM" , d_echo_target_to_allocate_new_vm_in_EPSM );
	root.put( "d_echo_target_to_allocate_new_vm_in_EPSM_dump_quality" , d_echo_target_to_allocate_new_vm_in_EPSM_dump_quality );
	root.put( "d_echo_target_to_allocate_new_vm_in_EPSM_dump_quality_full_dag_dump" , d_echo_target_to_allocate_new_vm_in_EPSM_dump_quality_full_dag_dump );
	root.put( "d_use_cache_for_XBT_INFO" , d_use_cache_for_XBT_INFO );
	root.put( "d_dump_top_quality_vm_stat" , d_dump_top_quality_vm_stat );
	

	//root.put( "d_dump_how_much_vm_leased_for_bundle_scheduling_in_iteration" , d_dump_how_much_vm_leased_for_bundle_scheduling_in_iteration );
	root.put( "_____" , "########################################################" );
	root.put( "UpdateTime" , get_time() );
	root.put( "d_config_write_count" , d_config_write_count );
	root.put( "d_config_read_count" , d_config_read_count );
	

	return root;
}

void Global_config::post_read_action()
{
	BEGIN_TRY

	//if ( d_enable_all_dump_echo_notif_just_in_runtime )
	//{
	//	d_enable_all_dump_echo_notif_just_in_runtime = false;
	//	d_prev_loaded_config = make_shared<Global_config>( *this );

	//	d_echo_add_dag = true;
	//	d_echo_nextTime_add_dag = true;
	//	d_notify_callback_actor_DGGenerator_finished = true;
	//	d_notify_dag_first_time_scheduled = true;
	//	d_notify_dag_new_node_to_schedule = true;
	//	d_declare_dag_finish_each_n_iteration = 1;
	//	d_echo_Ready2Schedule_node_list = true;
	//	d_dump_at_start_to_use_cycle_mgr_report = true;
	//	d_dump_at_end_use_cycle_mgr_report = true;
	//	d_echo__calculate_quality_ratio__value_chack = true;
	//	d_echo__callback_actor_container__start_execute = true;
	//	d_echo__callback_actor_container__done_execute = true;
	//	d_echo__callback_actor_container__done_execute_dump_bundle = true;
	//	d_echo__scheduleOnVM__schedule_new_node = true;
	//	d_echo__scheduleOnVM__nearest_node = true;
	//	d_echo__scheduleOnVM__try_to_find_Provisioned_vm = true;
	//	d_echo__scheduleOnVM__try_to_find_UNprovisioned_vm = true;
	//	d_echo__scheduleOnVM__dump_calc_ratio = true;
	//	d_echo__scheduleOnVM__details = true;
	//	d_enable_DAG_Deadline_Quota_notify__declare_dag_start_time = true;
	//	d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__finish_declare = true;
	//	d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__status_dag_to_deadline = true;
	//	d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__dump_dag = true;
	//	d_enable_Task_Deadline_Quota_notify = true;
	//	d_warn_impossible_condition = true;
	//	d_dump_Ready2ScheduleNode_order_priority = true;
	//	d_warn_if_some_vm_type_are_not_avaiable_for_leased = true;
	//	d_echo_XBT_INFO_OVERRIDEABLE = AeDumpLevel::full_diagnose;
	//}
	if ( d_force_dump_config )
	{
		d_force_dump_config = false;
		echo_config( true );
	}
	CATCH_TRY
}

//void Global_config::load_prev_config()
//{
//	d_echo_add_dag = d_prev_loaded_config->d_echo_add_dag;
//	d_echo_nextTime_add_dag = d_prev_loaded_config->d_echo_nextTime_add_dag;
//	d_notify_callback_actor_DGGenerator_finished = d_prev_loaded_config->d_notify_callback_actor_DGGenerator_finished;
//	d_notify_dag_first_time_scheduled = d_prev_loaded_config->d_notify_dag_first_time_scheduled;
//	d_notify_dag_new_node_to_schedule = d_prev_loaded_config->d_notify_dag_new_node_to_schedule;
//	d_declare_dag_finish_each_n_iteration = d_prev_loaded_config->d_declare_dag_finish_each_n_iteration;
//	d_echo_Ready2Schedule_node_list = d_prev_loaded_config->d_echo_Ready2Schedule_node_list;
//	d_dump_at_start_to_use_cycle_mgr_report = d_prev_loaded_config->d_dump_at_start_to_use_cycle_mgr_report;
//	d_dump_at_end_use_cycle_mgr_report = d_prev_loaded_config->d_dump_at_end_use_cycle_mgr_report;
//	d_echo__calculate_quality_ratio__value_chack = d_prev_loaded_config->d_echo__calculate_quality_ratio__value_chack;
//	d_echo__callback_actor_container__start_execute = d_prev_loaded_config->d_echo__callback_actor_container__start_execute;
//	d_echo__callback_actor_container__done_execute = d_prev_loaded_config->d_echo__callback_actor_container__done_execute;
//	d_echo__callback_actor_container__done_execute_dump_bundle = d_prev_loaded_config->d_echo__callback_actor_container__done_execute_dump_bundle;
//	d_echo__scheduleOnVM__schedule_new_node = d_prev_loaded_config->d_echo__scheduleOnVM__schedule_new_node;
//	d_echo__scheduleOnVM__nearest_node = d_prev_loaded_config->d_echo__scheduleOnVM__nearest_node;
//	d_echo__scheduleOnVM__try_to_find_Provisioned_vm = d_prev_loaded_config->d_echo__scheduleOnVM__try_to_find_Provisioned_vm;
//	d_echo__scheduleOnVM__try_to_find_UNprovisioned_vm = d_prev_loaded_config->d_echo__scheduleOnVM__try_to_find_UNprovisioned_vm;
//	d_echo__scheduleOnVM__dump_calc_ratio = d_prev_loaded_config->d_echo__scheduleOnVM__dump_calc_ratio;
//	d_echo__scheduleOnVM__details = d_prev_loaded_config->d_echo__scheduleOnVM__details;
//	d_enable_DAG_Deadline_Quota_notify__declare_dag_start_time = d_prev_loaded_config->d_enable_DAG_Deadline_Quota_notify__declare_dag_start_time;
//	d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__finish_declare = d_prev_loaded_config->d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__finish_declare;
//	d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__status_dag_to_deadline = d_prev_loaded_config->d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__status_dag_to_deadline;
//	d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__dump_dag = d_prev_loaded_config->d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__dump_dag;
//	d_enable_Task_Deadline_Quota_notify = d_prev_loaded_config->d_enable_Task_Deadline_Quota_notify;
//	d_warn_impossible_condition = d_prev_loaded_config->d_warn_impossible_condition;
//	d_dump_Ready2ScheduleNode_order_priority = d_prev_loaded_config->d_dump_Ready2ScheduleNode_order_priority;
//	d_warn_if_some_vm_type_are_not_avaiable_for_leased = d_prev_loaded_config->d_warn_if_some_vm_type_are_not_avaiable_for_leased;
//	d_echo_XBT_INFO_OVERRIDEABLE = d_prev_loaded_config->d_echo_XBT_INFO_OVERRIDEABLE;
//}

string Global_config::echo_config( bool on_cosole )
{
	auto root = make_json();
	stringstream stream;
	pt::write_json( stream , root );
	if ( on_cosole )
	{
		XBT_INFO_CONSOL( "%s" , stream.str().c_str() );
	}
	return stream.str();
}

void Global_config::writeConfig()
{
	auto root = make_json();
	pt::write_json( string_format( "%s/config%d.txt" , d_config_file_root.c_str() , d_simulationIteration ) , root );
}

void Global_config::closeConfig()
{
	if ( d_pActiveInstance != NULL )
	{
		//if ( d_prev_loaded_config != NULL )
		//{
		//	load_prev_config();
		//}

		d_config_write_count++;
		writeConfig();

		d_pActiveInstance = NULL;
	}
}

#pragma endregion

#pragma region xml_dag_guid_config

dags_configures __xml_dag_guid_config_; // Global Variable
std::ofstream __dag_guid_config_temp_backup_; // Global Variable

void init_xml_dag_guid_config_to_write()
{
	BEGIN_TRY
	__dag_guid_config_temp_backup_.open( SETTINGS_FAST()->d_backup_dag_guid_tmp_path , std::ios::app );
	CATCH_TRY
}

void add_dag_guid_config( _GUID uid , tuple_dag_configure dag_configuration , bool baddto_backup )
{
	BEGIN_TRY
		//cout << " guid " << uid << " s " << std::get<0>( dag_configuration )
		//<< " " << std::get<1>( dag_configuration ).size()
		//<< " " << std::get<3>( dag_configuration ).size()
		//<< " " << std::get<4>( dag_configuration )
		//<< endl;

	if ( __xml_dag_guid_config_.count( uid ) == 0 )
	{
		__xml_dag_guid_config_[ uid ] = make_pair( vector_dag_configures() , 0 );
	}
	__xml_dag_guid_config_[ uid ].first.push_back( dag_configuration );
	__xml_dag_guid_config_[ uid ].second++;
	//cout << "__xml_dag_guid_config_[ uid ] " << __xml_dag_guid_config_[ uid ].size() << endl;

	if ( baddto_backup )
	{
		__dag_guid_config_temp_backup_ << "[guid=" << uid << "][time=" << std::get<0>( dag_configuration ) << "][dag_ccr=" << std::get<4>( dag_configuration ) << "]";
		for ( auto & nodes_vars : std::get<1>( dag_configuration ) )
		{
			__dag_guid_config_temp_backup_ << "[node_vars=" <<
				nodes_vars.first <<
				"," << std::get<0>( nodes_vars.second ) << // cpu
				"," << std::get<1>( nodes_vars.second ) << // size
				"," << std::get<2>( nodes_vars.second ) << "]"; // output
		}
		__dag_guid_config_temp_backup_ << "[vmtype=" << ( int )std::get<0>( std::get<2>( dag_configuration ) ).d_type <<
			"][deadline=" << std::get<1>( std::get<2>( dag_configuration ) ) <<
			"][TimeConsume=" << std::get<2>( std::get<2>( dag_configuration ) ) <<
			"][parallism=" << std::get<3>( std::get<2>( dag_configuration ) ) <<
			"][maxParallism=" << std::get<4>( std::get<2>( dag_configuration ) ) << "]";
		for ( auto & row : std::get<5>( std::get<2>( dag_configuration ) ) )
		{
			__dag_guid_config_temp_backup_ << "[node_qouta=" << row.first << "," << row.second << "]";
		}
		for ( auto & row : std::get<3>( dag_configuration ) )
		{
			__dag_guid_config_temp_backup_ << "[node_katz=" << row.first << "," << row.second << "]";
		}
		__dag_guid_config_temp_backup_ << endl;
	}
	
	// delete random cache
	if ( baddto_backup == false )
	{
		if ( __xml_dag_guid_config_[ uid ].first.size() == SETTINGS_FAST()->d_make_dag_n_guid_config )
		{
			std::random_device rd;
			std::mt19937 generator( rd() );
			std::shuffle( __xml_dag_guid_config_[ uid ].first.begin() , __xml_dag_guid_config_[ uid ].first.end() , generator );

			__xml_dag_guid_config_[ uid ].first.erase(
				__xml_dag_guid_config_[ uid ].first.begin() + 1 ,
				__xml_dag_guid_config_[ uid ].first.end() );
		}
	}
	
	CATCH_TRY
}

void finish_xml_dag_guid_config_for_write()
{
	__dag_guid_config_temp_backup_.close();
}

void load_backup_dag_guid( bool b_on_random_per_guid )
{
	BEGIN_TRY
		cout << "#" << __LINE__ << " " << "backup_dag_guid_tmp_path " << SETTINGS_FAST()->d_backup_dag_guid_tmp_path << endl;
		std::ifstream file( SETTINGS_FAST()->d_backup_dag_guid_tmp_path );
		string line_token;
		_GUID sguid;
		string  stime , sccr , vars , vmtype , deadline , TimeConsume , parallism , maxParallism , qouta , katz;
		t_map_nodes_duration listnodeqouta;
		t_map_node_katz list_node_katz;
		t_map_nodes_vars list_node_vars;
		int __counter = 0;
		int dump_every_step = 10;

		cout << __FUNCTION__ << " #"  << __LINE__ << " starterd" << endl;

		while ( std::getline( file , line_token , '\n' ) )
		{
			stringstream line_token_stream( trim_copy( line_token ) );
			string token;
			while ( std::getline( line_token_stream , token , ']' ) && ( token = trim_copy( token ) ).length() > 0 )
			{
				__counter++;
				if ( token.rfind( "[guid=" , 0 ) == 0 )
				{
					if ( __counter % dump_every_step == 0 )
					{
						dump_every_step *= 2;
						cout << "#" << __LINE__ << "  " << __counter << " " << token << endl;
					}
					if ( listnodeqouta.size() > 0 )
					{
						tuple_dag_configure dag_configuration;
						std::get<0>( dag_configuration ) = stime;
						std::get<1>( dag_configuration ) = list_node_vars;
						std::get<2>( dag_configuration ) = make_tuple( VMachine::get_type_instance( ( eVM_Type )atoi( vmtype.c_str() ) ) ,
																	   atof( deadline.c_str() ) , atof( TimeConsume.c_str() ) , atoi( parallism.c_str() ) ,
																	   atoi( maxParallism.c_str() ) , listnodeqouta );
						std::get<3>( dag_configuration ) = list_node_katz;
						std::get<4>( dag_configuration ) = atof( sccr.c_str() );

						add_dag_guid_config( sguid , dag_configuration , false );
					}

					sguid = stime = sccr = vars = vmtype = deadline = TimeConsume = parallism = maxParallism = qouta = katz = "";
					listnodeqouta.clear();
					list_node_katz.clear();
					list_node_vars.clear();
					sguid = ReplaceAll( token , "[guid=" , "" );
				}
				else if ( token.rfind( "[time=" , 0 ) == 0 )
				{
					stime = ReplaceAll( token , "[time=" , "" );
				}
				else if ( token.rfind( "[dag_ccr=" , 0 ) == 0 )
				{
					sccr = ReplaceAll( token , "[dag_ccr=" , "" );
				}
				else if ( token.rfind( "[node_vars=" , 0 ) == 0 )
				{
					vars = ReplaceAll( token , "[node_vars=" , "" );

					stringstream vars_value_stream( trim_copy( vars ) );
					std::getline( vars_value_stream , token , ',' );
					string snode = token;
					std::getline( vars_value_stream , token , ',' );
					string scpu = token;
					std::getline( vars_value_stream , token , ',' );
					string ssize = token;
					std::getline( vars_value_stream , token , ',' );
					string soutput = token;

					list_node_vars[ atoi( snode.c_str() ) ] = make_tuple( atof( scpu.c_str() ) , atof( ssize.c_str() ) , atof( soutput.c_str() ) );
				}
				else if ( token.rfind( "[vmtype=" , 0 ) == 0 )
				{
					vmtype = ReplaceAll( token , "[vmtype=" , "" );
				}
				else if ( token.rfind( "[deadline=" , 0 ) == 0 )
				{
					deadline = ReplaceAll( token , "[deadline=" , "" );
				}
				else if ( token.rfind( "[TimeConsume=" , 0 ) == 0 )
				{
					TimeConsume = ReplaceAll( token , "[TimeConsume=" , "" );
				}
				else if ( token.rfind( "[parallism=" , 0 ) == 0 )
				{
					parallism = ReplaceAll( token , "[parallism=" , "" );
				}
				else if ( token.rfind( "[maxParallism=" , 0 ) == 0 )
				{
					maxParallism = ReplaceAll( token , "[maxParallism=" , "" );
				}
				else if ( token.rfind( "[node_qouta=" , 0 ) == 0 )
				{
					qouta = ReplaceAll( token , "[node_qouta=" , "" );
					stringstream qouta_value_stream( trim_copy( qouta ) );
					std::getline( qouta_value_stream , token , ',' );
					string snode = token;
					std::getline( qouta_value_stream , token , ',' );
					string nodedeadline = token;

					listnodeqouta[ atoi( snode.c_str() ) ] = atof( nodedeadline.c_str() );
				}
				else if ( token.rfind( "[node_katz=" , 0 ) == 0 )
				{
					katz = ReplaceAll( token , "[node_katz=" , "" );
					stringstream qouta_value_stream( trim_copy( katz ) );
					std::getline( qouta_value_stream , token , ',' );
					string snode = token;
					std::getline( qouta_value_stream , token , ',' );
					string katzvalue = token;

					list_node_katz[ atoi( snode.c_str() ) ] = atof( katzvalue.c_str() );
				}
			}
		}

		if ( listnodeqouta.size() > 0 )
		{
			tuple_dag_configure dag_configuration;
			std::get<0>( dag_configuration ) = stime;
			std::get<1>( dag_configuration ) = list_node_vars;
			std::get<2>( dag_configuration ) = make_tuple( VMachine::get_type_instance( ( eVM_Type )atoi( vmtype.c_str() ) ) ,
														   atof( deadline.c_str() ) , atof( TimeConsume.c_str() ) , atoi( parallism.c_str() ) ,
														   atoi( maxParallism.c_str() ) , listnodeqouta );
			std::get<3>( dag_configuration ) = list_node_katz;
			std::get<4>( dag_configuration ) = atof( sccr.c_str() );

			add_dag_guid_config( sguid , dag_configuration , false );
		}
		listnodeqouta.clear();
		list_node_katz.clear();
		list_node_vars.clear();

		cout << __FUNCTION__ << " #"  << __LINE__ << " ended" << endl;

	CATCH_TRY
}

void read_xml_dag_guid_config( bool b_on_random_per_guid )
{
	assert( __xml_dag_guid_config_.size() == 0 );
	BEGIN_TRY

	load_backup_dag_guid( b_on_random_per_guid );

	// control cache
	cout << "#" << __LINE__ << " " << "count guid " << __xml_dag_guid_config_.size() << endl;
	for ( auto & each_guid : __xml_dag_guid_config_ )
	{
		if ( each_guid.second.second != SETTINGS_FAST()->d_make_dag_n_guid_config )
		{
			cout << " wrong dag cache " << each_guid.second.second << " " << SETTINGS_FAST()->d_make_dag_n_guid_config << " " << each_guid.first << endl;
		}

		//int node_vars_count = 0;
		//int node_katz_count = 0;
		//int node_qouta_count = 0;

		//for ( auto & each_dag_configure : each_guid.second.first )
		//{
		//	node_vars_count += std::get<1>( each_dag_configure ).size();
		//	node_katz_count += std::get<3>( each_dag_configure ).size();
		//	node_qouta_count += std::get<5>( std::get<2>( each_dag_configure ) ).size();
		//}

		//if ( node_vars_count != node_katz_count || node_katz_count != node_qouta_count )
		//{
		//	cout << " wrong dag cache " << each_guid.second.size() << " " << each_guid.first << endl;
		//}
	}

	CATCH_TRY
}

vector_dag_configures get_xml_dag_guid_config( _GUID uid )
{
	if ( __xml_dag_guid_config_.count( uid ) != 0 )
	{
		return __xml_dag_guid_config_[ uid ].first;
	}
	return vector_dag_configures();
}

void finish_xml_dag_guid_config_read()
{
	__xml_dag_guid_config_.clear();
}

#pragma endregion

/////////////////////////////////////////////

SystemTimeKeepr::SystemTimeKeepr()
{
	std::time( &d_start );
}
SystemTimeKeepr::~SystemTimeKeepr()
{ }
bool SystemTimeKeepr::peek_time( SystemDurationTime elapsed , bool reset_time_to_now )
{
	time_t tnow;
	std::time( &tnow );

	if ( SETTINGS_FAST()->d_elapsed_time_in_long_loop < 0 )
	{
		return false;
	}
	if ( elapsed <= 0 )
	{
		elapsed = SETTINGS_FAST()->d_elapsed_time_in_long_loop;
	}
	if ( difftime( tnow , d_start ) >= elapsed )
	{
		if ( reset_time_to_now )
		{
			d_start = tnow;
		}
		return true;
	}
	return false;
}

/////////////////////////////////////////////
SectionTimeKeeper::SectionTimeKeeper( SectionTimeKeeper ** ppGlobalKeeper , double dump_minimum_time_milisecond )
{
	d_t1 = high_resolution_clock::now();
	d_dump_minimum_time_milisecond = dump_minimum_time_milisecond;
	d_ppGlobalKeeper = ppGlobalKeeper;
	*d_ppGlobalKeeper = this;
}
SectionTimeKeeper::~SectionTimeKeeper()
{
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	d_t2 = high_resolution_clock::now();

	duration<double , std::milli> ms_double_sections = d_t2 - d_t1;
	map< string , string > comments;
	if ( d_dump_minimum_time_milisecond > 0 && ms_double_sections.count() >= d_dump_minimum_time_milisecond )
	{
		bool b = false;
		for ( auto & section_line : d_section_benchmark )
		{
			if ( !b )
			{
				b = true;
				XBT_INFO_EMPTY_LINE();
				XBT_INFO( "section about (%f) milisec" , ms_double_sections.count() );
			}
			double total_section_duration = 0;
			int count = 0;
			for ( auto & section : section_line.second )
			{
				count++;
				duration<double , std::milli> ms_double_section = section.d_t2 - section.d_t1;
				total_section_duration += ms_double_section.count();

				if ( section.d_snote.length() > 0 && comments.count( section.d_snote ) == 0 )
				{
					comments[ section.d_snote ] = section.d_snote;
				}
			}
			XBT_INFO( "section at line(%d) cost (%f) milisec about (%d) count , avg(%f)" ,
					  section_line.first , total_section_duration , count , total_section_duration / count );
		}
		for ( auto & comment : comments )
		{
			XBT_INFO( "%s" , comment.second.c_str() );
		}
	}
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	*d_ppGlobalKeeper = NULL;
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
}

void SectionTimeKeeper::add_section( SectionTime * pSection )
{
	pSection->d_pKeeper = NULL;
	d_section_benchmark[ pSection->d_line_number ].push_back( *pSection );
}

SectionTime::SectionTime( SectionTimeKeeper * pKeeper , int line , string snote )
{
	d_line_number = line;
	d_snote = snote;
	d_pKeeper = pKeeper;
	d_t1 = high_resolution_clock::now();
}
SectionTime::~SectionTime()
{
	d_t2 = high_resolution_clock::now();
	if ( d_pKeeper != NULL )
	{
		d_pKeeper->add_section( this );
	}
}

/////////////////////////////////////////////

SectionAliveCheck::SectionAliveCheck( int line_number , string scomment )
{
	static int _id = -1;
	d_section_id = ++_id;
	d_line_number = line_number;
	d_comment = scomment;
}
SectionAliveCheck::~SectionAliveCheck()
{
	XBT_INFO_ALIVE_CHECK( "#(%d) ~section ↑(%d)" , d_line_number , d_section_id );
}
