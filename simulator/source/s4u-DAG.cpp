#define uses_pow
#define uses_mat
#define uses_DAG
#define uses_string_format
#define uses_Deadline_Quota
#define uses_random
#define uses_LOG
#define uses_getline
#define uses_CNFG
#define uses_thread
#define uses_CBenchmark

#include "../inc/dep"

XBT_LOG_NEW_DEFAULT_CATEGORY( DAG , "Messages specific for this s4u example" );

#pragma region DAGEdge

DAGEdge::DAGEdge( DAGNode * srcSource , DAGNode * srcDest )
{
	static int _edge_id = 0;
	d_edge_id = ++_edge_id;

	d_pSource = NULL;
	d_pDestination = NULL;

	d_p_network_usage_byte = NULL;
	d_pSource = srcSource;
	d_pDestination = srcDest;
	
	////d_edge_name = string_format( "(%s)->(%s)" , (d_pSource)->d_node_name.c_str() , (d_pDestination)->d_node_name.c_str() );
}

void DAGEdge::init_edge() // make link and parent both
{
	BEGIN_TRY
	(d_pDestination)->d_parents.push_back( d_pSource );

	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) d_destination->d_parents.size %d" , __LINE__ , (int)d_destination->d_parents.size() );

	(d_pSource)->d_links.push_back( this );

	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) d_source->d_links.size %d" , __LINE__ , ( int )d_source->d_links.size() );

	assert( (d_pDestination)->d_node_dax_id != (d_pSource)->d_node_dax_id );
	CATCH_TRY
}

void DAGEdge::dump( bool inside_another )
{
	if ( !inside_another ) XBT_INFO_EMPTY_LINE();
	//XBT_INFO( "dump EDGE_%s" , d_edge_name.empty() ? "N//P" : d_edge_name.c_str() );
	
	if ( d_p_network_usage_byte != NULL )
	{
		XBT_INFO( "network_usage_byte(%s)" , trim_floatnumber( *d_p_network_usage_byte ).c_str() );
	}
	XBT_INFO( "normalized_inverse_network_usage(%s)" , trim_floatnumber( normalized_inverse_network_usage ).c_str() );
}

#pragma endregion

#pragma region DAGNode

DAGNode::DAGNode( const char * pName )
{
	static int _id = eNodeID::first_node_id;
	d_node_uniq_id = ( eNodeID )++_id;

	d_node_dax_id = -1;

	d_allocated_node_name = new char[ strlen(pName) + 1];
	strcpy( d_allocated_node_name , pName );

	d_pOwnerDAG = NULL;

	d_task_cpu_usage_flops = 0;
	d_task_size_byte = 0;
	d_task_output_size_byte = 0;

	d_Katz_value = 0;

	d_pRuntime = NULL;

	d_ended_parent = 0;

	////d_task_deadline = NULL;
}

DAGNode::runtime_neccessary::runtime_neccessary()
{
	d_taskProccessed = false;
	d_taskInSchedulList = false;
	d_taskScheduled_in_vm = false;
	d_sort_iteration = -1;
}

DAGNode::runtime_neccessary::~runtime_neccessary()
{
	cleanup();
}
void DAGNode::runtime_neccessary::cleanup()
{
}

DAGNode::~DAGNode()
{
	cleanup();
}
void DAGNode::cleanup()
{
	for ( auto & link : d_links )
	{
		__DELETE( link );
	}
	d_links.clear();
	if ( d_allocated_node_name != NULL )
	{
		delete[] d_allocated_node_name;
		d_allocated_node_name = NULL;
	}
	if ( d_pRuntime != NULL )
	{
		__DELETE( d_pRuntime );
		d_pRuntime = NULL;
	}
}

AbsoluteTime * DAGNode::get_actual_triggered_time()
{
	if ( d_task_deadline.d_task_actual_triggered_time >= 0 )
	{
		return &d_task_deadline.d_task_actual_triggered_time;
	}
	return NULL;
}

AbsoluteTime * DAGNode::get_actual_complete_time()
{
	if ( d_task_deadline.d_task_actual_complete_execution_time >= 0 )
	{
		return &d_task_deadline.d_task_actual_complete_execution_time;
	}
	return NULL;
}

void DAGNode::dump( bool inside_another , bool dump_links )
{
	if ( !inside_another ) XBT_INFO_EMPTY_LINE();
	XBT_INFO( "dump task_%s@%s , cpu(%s flops) , size(%s b) , output(%s b) , ktz(%s)" ,
			  d_allocated_node_name , d_pOwnerDAG->get_name().c_str() ,
			  trim_floatnumber( d_task_cpu_usage_flops ).c_str() , trim_floatnumber( d_task_size_byte ).c_str() , trim_floatnumber( d_task_output_size_byte ).c_str() , trim_floatnumber( d_Katz_value ).c_str() );

	if ( d_pRuntime != NULL )
	{
		if ( /*d_task_deadline != NULL &&*/ d_task_deadline.d_task_actual_triggered_time >= 0 )
		{
			XBT_INFO( "task_triggered_time(at %f s) %s %s" , d_task_deadline.d_task_actual_triggered_time ,
					  string_format( ( d_task_deadline.d_task_actual_complete_execution_time >= 0 ? ", task_complete_time(at %f s) " : "" ) , d_task_deadline.d_task_actual_complete_execution_time ).c_str() ,
					  string_format( ( d_task_deadline.d_task_actual_complete_execution_time >= 0 ? ", diff(%f s) " : "" ) , d_task_deadline.d_task_actual_complete_execution_time - d_task_deadline.d_task_actual_triggered_time ).c_str() );
		}
		//if ( d_task_deadline != NULL ) 
		d_task_deadline.dump( true );
	}

	if ( dump_links )
	{
		for ( auto & ref_link : d_links )
		{
			ref_link->dump( false );
		}
	}
}

#pragma endregion

#pragma region DAG

DAG::DAG()
{
	d_dag_uniq_id = eDAG_ID::INVALID_DAG_ID;
	d_nodes_count = -99999;
	d_ccr = -1;
	d_least_price_vmp_type = eVM_Type::invalid_vm_type;
	d_MaximumPossibleParallelism = -1;
	d_pStatistic = NULL;
	d_pRuntime = NULL;
}

DAG::runtime_neccessary::runtime_neccessary()
{
	d_shuffled_nodes = NULL;
	d_ordered_nodes = NULL;
	d_swap_shuffled_nodes = NULL;
}
DAG::runtime_neccessary::~runtime_neccessary()
{
	cleanup();
}
void DAG::runtime_neccessary::cleanup()
{ 
	if ( d_shuffled_nodes != NULL )
	{
		for ( auto & node : *d_shuffled_nodes )
		{
			__DELETE( node.second );
		}
		d_shuffled_nodes->clear();
		__DELETE( d_shuffled_nodes );
		d_shuffled_nodes = NULL;
	}
	if ( d_ordered_nodes != NULL )
	{
		d_ordered_nodes->clear();
		__DELETE( d_ordered_nodes );
		d_ordered_nodes = NULL;
	}
}

DAG::statistic_neccessary::statistic_neccessary()
{
	d_dag_communication_byte_need = 0;
	d_dag_communication_duration = 0;

	d_dag_expectation_communication_byte_on_slow_link = 0;
	d_dag_reality_communication_byte_on_slow_link = 0;

	d_dag_computation_time_on_avg_machine = 0;
	d_dag_reality_computation_time = 0;
}

DAG::~DAG()
{
	cleanup();
}
void DAG::cleanup()
{
	if ( d_pRuntime != NULL )
	{
		__DELETE( d_pRuntime );
		d_pRuntime = NULL;
	}
	if ( d_pStatistic != NULL )
	{
		__DELETE( d_pStatistic );
		d_pStatistic = NULL;
	}
}

bool DAG::hasUnprocessedTask()
{
	BEGIN_TRY
	if ( d_pRuntime->d_shuffled_nodes == NULL )
	{
		make_shuffled_nodes();
	}

	//bool processed = true;
	for ( auto & node : *d_pRuntime->d_ordered_nodes )
	{
		if ( !node->d_pRuntime->d_taskProccessed )
		{
			return true;
		}
	}
	CATCH_TRY
	return false;
}

void DAG::make_ordered_nodes()
{
	BEGIN_TRY
	if ( d_pRuntime->d_ordered_nodes == NULL )
	{
		d_pRuntime->d_ordered_nodes = __NEW( vector< DAGNode * >() );
		// hierarchical assignment
		vector< DAGNode * > ret_temp;
		map< eNodeID , eNodeID > seen_node;
		for ( auto & node : d_pRuntime->d_roots )
		{
			ret_temp.push_back( node );
			seen_node[ node->d_node_uniq_id ] = node->d_node_uniq_id;
		}

		do
		{
			vector< DAGNode * > last_ret_temp( ret_temp );
			ret_temp.clear();

			for ( auto & ref_node : last_ret_temp )
			{
				for ( auto & ref_childs : ref_node->d_links )
				{
					bool bAllParentSeen = true;
					for ( const auto & ref_parent : (ref_childs->d_pDestination)->d_parents ) // این چک را هر جایی که از نود به فرزند ها می خواهیم برسیم باید انجام دهم
					{
						if ( seen_node.count( ref_parent->d_node_uniq_id ) == 0 )
						{
							bAllParentSeen = false;
							break;
						}
					}
					if ( bAllParentSeen ) // اگر همه پدر ها دبده شده
					{
						if ( seen_node.count( (ref_childs->d_pDestination)->d_node_uniq_id ) == 0 ) // و خود نود بکیار دیده شود
						{
							seen_node[ (ref_childs->d_pDestination)->d_node_uniq_id ] = (ref_childs->d_pDestination)->d_node_uniq_id;
							ret_temp.push_back( ref_childs->d_pDestination );
						}
					}
				}
			}
			for ( auto & ref_node : last_ret_temp )
			{
				d_pRuntime->d_ordered_nodes->push_back( ref_node );
			}

		} while ( !ret_temp.empty() );
		d_nodes_count = d_pRuntime->d_ordered_nodes->size();
	}
	CATCH_TRY
}

void DAG::make_shuffled_nodes()
{
	BEGIN_TRY
	if ( d_pRuntime->d_shuffled_nodes == NULL )
	{
		d_pRuntime->d_shuffled_nodes = __NEW( _map_nodes );
		if ( d_pRuntime->d_ordered_nodes == NULL )
		{
			make_ordered_nodes();
		}
		for ( const auto & ref_node : *d_pRuntime->d_ordered_nodes )
		{
			( *d_pRuntime->d_shuffled_nodes )[ ref_node->d_node_uniq_id ] = ref_node;
		}
		////this->d_swap_shuffled_nodes = NULL;
	}
	CATCH_TRY
}

class VMachine_Executer : public VMachine
{
public:
	VMachine_Executer( VMachine vm_type ) : VMachine(vm_type)
	{
		assign_task_time = -1;
		execute_task_duration = -1;
		assigned_task = NULL;
	}

	double assign_task_time;
	double execute_task_duration;
	DAGNode * assigned_task;

	virtual bool is_scheduler()
	{
		return false;
	}
	virtual bool is_idle( double now , int * pNumIdleVM = NULL )
	{
		if ( now >= assign_task_time + execute_task_duration )
		{
			if ( pNumIdleVM != NULL )
			{
				(*pNumIdleVM)++;
			}
			return true;
		}
		return false;
	}
	virtual tuple<double/*pure execution*/ , double/*execution plus provisioning*/> schedule_task( double now , DAGNode * node )
	{
		bool schedule_next_cycle = assign_task_time >= 0;

		assigned_task = node;

		execute_task_duration = schedule_next_cycle ? 0 : SETTINGS_FAST()->d_VIRTUALMACHINE_WARMUP_TIME_SEC + SETTINGS_FAST()->d_CONTAINER_INITIALIZATION_DALEY_TIME;
		assign_task_time = now;

		double pure_execute_task_duration = 0;

		// read task from gs
		pure_execute_task_duration += node->d_task_size_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;

		// transfer to vm
		pure_execute_task_duration += this->get_duration_datatransfer_on_wan( node->d_task_size_byte );

		// write task on vm
		pure_execute_task_duration += this->get_duration_datatransfer_on_mem( node->d_task_size_byte );

		// read task input from gs
		// transfer task input to vm
		for ( const auto & ref_node_parent : node->d_parents )
		{
			pure_execute_task_duration += ref_node_parent->d_task_output_size_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;
			pure_execute_task_duration += this->get_duration_datatransfer_on_wan( ref_node_parent->d_task_output_size_byte );
			pure_execute_task_duration += this->get_duration_datatransfer_on_mem( ref_node_parent->d_task_output_size_byte );
		}

		// exec time
		pure_execute_task_duration += this->get_execution_duration( node->d_task_cpu_usage_flops );

		// write output to vm
		pure_execute_task_duration += this->get_duration_datatransfer_on_mem( node->d_task_output_size_byte );

		// transfer output to gs
		pure_execute_task_duration += this->get_duration_datatransfer_on_wan( node->d_task_output_size_byte );

		// write output to gs
		pure_execute_task_duration += node->d_task_output_size_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;

		execute_task_duration += pure_execute_task_duration;

		return make_tuple( pure_execute_task_duration , execute_task_duration );
	}
};

class VMachine_Executer_Scheduler : public VMachine_Executer
{
public:
	VMachine_Executer_Scheduler( VMachine vm_type ) : VMachine_Executer(vm_type)
	{
		d_scheduleIteration = 0;
		execute_task_duration = SETTINGS_FAST()->d_schedule_periode_delay_time; // schedule interval
	}

	int d_scheduleIteration;

	virtual bool is_scheduler()
	{
		return true;
	}
	virtual bool is_idle( double now , int * pNumIdleVM = NULL )
	{
		while( assign_task_time < 0 || now >= assign_task_time + execute_task_duration )
		{
			if ( assign_task_time < 0 )
			{
				assign_task_time = now;
			}
			else
			{
				assign_task_time = assign_task_time + execute_task_duration;
			}
			d_scheduleIteration++;
		}
		return false;
	}
	virtual tuple<double/*pure execution*/ , double/*execution plus provisioning*/> schedule_task( double now , DAGNode * node )
	{
		////assign_task_time = now;
		is_idle(now);
		return make_tuple( execute_task_duration , execute_task_duration );
	}
};

class VMachine_Executer_Mgr
{
public:
	VMachine_Executer_Mgr(VMachine src_vm_type)
	{
		vm_type = src_vm_type;
		d_now = 0;
	}
	~VMachine_Executer_Mgr()
	{
		cleanup();
	}
	void cleanup()
	{
		for ( auto & vm : lst_vms )
		{
			__DELETE( vm );
		}
		lst_vms.clear();
		pScheduler = NULL;
	}

	VMachine vm_type;
	vector< VMachine_Executer * > lst_vms;
	VMachine_Executer_Scheduler * pScheduler;
	map< eNodeID , bool > node_endded;
	double d_now;

	tuple<double/*pure execution*/ , double/*execution plus provisioning*/> map_task_to_vm(double now , DAGNode * node , int max_vm = -1) // return execute duration
	{
		if ( lst_vms.empty() )
		{
			pScheduler = __NEW( VMachine_Executer_Scheduler( vm_type ) );
			lst_vms.push_back( pScheduler ); // start scheduler
			pScheduler->schedule_task( now , node );
		}
		assert(node_endded.count(node->d_node_uniq_id) == 0);
		for ( auto & vm : lst_vms )
		{
			if ( vm->is_idle( now ) )
			{
				node_endded[ node->d_node_uniq_id ] = false;
				return vm->schedule_task( now , node );
			}
		}
		if ( max_vm > 0 )
		{
			if ( lst_vms.size() < max_vm + 1 )
			{
				node_endded[ node->d_node_uniq_id ] = false;
				lst_vms.push_back( __NEW( VMachine_Executer(vm_type) ) );
				return lst_vms.back()->schedule_task( now , node );
			}
			return make_tuple( -1 , -1 );
		}
		node_endded[ node->d_node_uniq_id ] = false;
		lst_vms.push_back( __NEW( VMachine_Executer(vm_type) ) );
		return lst_vms.back()->schedule_task( now , node );
	}
	double next_discret_time_simulate( int * pNumIdleVM = NULL ) // step time forward
	{
		VMachine_Executer * pvm = NULL;
		VMachine_Executer * pvm_nonscheduler = NULL;
		double min_exe_time_all_vm = -1;
		double min_exe_time_non_scheduler = -1;

		//VMachine_Executer * pvm = NULL;
		//double min_exe_time = -1;
		if ( lst_vms.size() == 0 && pNumIdleVM != NULL )
		{
			(*pNumIdleVM) = -1;
		}
		for ( auto & vm : lst_vms )
		{
			if ( !vm->is_idle( d_now ) )
			{
				if ( min_exe_time_all_vm < 0 )
				{
					min_exe_time_all_vm = vm->assign_task_time + vm->execute_task_duration;
					pvm = vm;
				}
				else if ( vm->assign_task_time + vm->execute_task_duration < min_exe_time_all_vm )
				{
					min_exe_time_all_vm = vm->assign_task_time + vm->execute_task_duration;
					pvm = vm;
				}
				if ( !vm->is_scheduler() )
				{
					if ( min_exe_time_non_scheduler < 0 )
					{
						min_exe_time_non_scheduler = vm->assign_task_time + vm->execute_task_duration;
						pvm_nonscheduler = vm;
					}
					else if ( vm->assign_task_time + vm->execute_task_duration < min_exe_time_non_scheduler )
					{
						min_exe_time_non_scheduler = vm->assign_task_time + vm->execute_task_duration;
						pvm_nonscheduler = vm;
					}
				}
			}
		}

		if ( pvm != NULL )
		{
		    if ( pvm->is_scheduler() )
		    {
		        if ( pvm_nonscheduler != NULL )
		        {
		            if ( pvm != pvm_nonscheduler )
		            {
		                if ( ( ( pvm_nonscheduler->assign_task_time + pvm_nonscheduler->execute_task_duration ) - d_now ) > 5 * SETTINGS_FAST()->d_schedule_periode_delay_time )
		                {
		                    pvm->is_idle( d_now + ( 
		                        (
		                        ( ( int64_t )( ( pvm_nonscheduler->assign_task_time + pvm_nonscheduler->execute_task_duration ) - ( pvm->assign_task_time + pvm->execute_task_duration ) ) ) /
		                                              ( ( int64_t )SETTINGS_FAST()->d_schedule_periode_delay_time ) 
		                        ) - 3 ) * SETTINGS_FAST()->d_schedule_periode_delay_time );
		                }
		            }
		        }
		    }
		}

		if ( pvm != NULL )
		{
			d_now = pvm->assign_task_time + pvm->execute_task_duration;
			for ( auto & vm : lst_vms )
			{
				if ( vm->is_idle( d_now , pNumIdleVM ) )
				{
					assert( vm->assigned_task != NULL );
					assert( node_endded.count(vm->assigned_task->d_node_uniq_id) != 0 );
					if ( !node_endded[ vm->assigned_task->d_node_uniq_id ] )
					{
						node_endded[ vm->assigned_task->d_node_uniq_id ] = true;

						for ( auto & link : vm->assigned_task->d_links )
						{
							link->d_pDestination->d_ended_parent++;
						}
					}
				}
			}
		}
		else
		{
			//assert(lst_vms.size() > 0);
			//double prev_now = d_now;
			for ( auto & vm : lst_vms )
			{
				if ( vm->assign_task_time + vm->execute_task_duration > d_now )
				{
					d_now = vm->assign_task_time + vm->execute_task_duration;
				}
			}
			//assert( d_now > prev_now );
		}
		return d_now;
	}
	bool is_task_ended( eNodeID task_id )
	{
		if ( node_endded.count( task_id ) != 0 )
		{
			return node_endded[task_id];
		}
		return false;
	}
};

//DurationTime DAG::execute_serially_on_one_machine_time( VMachine machine ) // royeh yek machine fast seri execute shavad
//{
//	BEGIN_TRY
//
//	if ( d_pRuntime->d_ordered_nodes == NULL )
//	{
//		make_ordered_nodes();
//	}
//	
//	for ( auto & node : *this->d_pRuntime->d_ordered_nodes )
//	{
//		node->d_ended_parent = 0;
//	}
//	////auto & slowest_vm = VMachine::().front(); // slowest
//	// up down methode
//	// hierarchical assignment
//	// infinite loop
//
//	vector< DAGNode * > inlist_node;
//	map< eNodeID , int /*schedule iteration*/ > control;
//	VMachine_Executer_Mgr mgr( machine );
//	map< eNodeID , int > node_link_in_list; // میگه هر نود کدام لینکاش اسکجول شده و اگر همش اسکجول شده دیگه کنترل کردن برای اسکجول یکیش بی مورده
//	for ( auto & node : d_pRuntime->d_roots )
//	{
//		inlist_node.push_back( node );
//		////mgr.map_task_to_vm( 0 , node );
//		control[ node->d_node_uniq_id ] = 0;
//	}
//	while( 1 )
//	{
//		int NumIdleVM = 0;
//		double now = mgr.next_discret_time_simulate( &NumIdleVM );
//		assert( this->d_pRuntime->d_ordered_nodes != NULL );
//		if ( mgr.node_endded.size() == this->d_pRuntime->d_ordered_nodes->size() )
//		{
//			bool break_loop = true;
//			for ( auto & node : *this->d_pRuntime->d_ordered_nodes )
//			{
//				if ( !mgr.is_task_ended( node->d_node_uniq_id ) )
//				{
//					break_loop = false;
//					break;
//				}
//			}
//			if ( break_loop )
//			{
//				break;
//			}
//		}
//		if ( NumIdleVM == 0 )
//		{
//			continue; // اگر اون ماشین تکی خالی نیست یعنی هیچ تسکی تمام نشده و یعنی بعدی ها نمیتونه بره برای اجرا و در لیست بره
//		}
//
//		vector< DAGNode * > tmp_inlist_node( inlist_node );
//		bool b_one_task_scheduled = false;
//		for ( auto & node : tmp_inlist_node )
//		{
//			if ( control[ node->d_node_uniq_id ] == 0 )
//			{
//				if ( !b_one_task_scheduled && std::get<0>( mgr.map_task_to_vm( now , node , 1 ) ) > 0 )
//				{
//					b_one_task_scheduled = true;
//					control[ node->d_node_uniq_id ] = mgr.pScheduler->d_scheduleIteration;
//					////node_link_scheduled[ node->d_node_uniq_id ] += 1;
//				}
//			}
//			else if ( mgr.is_task_ended( node->d_node_uniq_id )  && node_link_in_list[node->d_node_uniq_id] != node->d_links.size() )
//			{
//				for ( auto & link : node->d_links )
//				{
//					if ( control.count( link->d_pDestination->d_node_uniq_id ) != 0 )
//					{
//						// یعنی لینک اضافه شده دیگه اضافش نکن
//						continue;
//					}
//
//					if ( link->d_pDestination->d_ended_parent < ( link->d_pDestination )->d_parents.size() )
//					{
//						// این لینک پدر هاش تعدادشون به تعداد پدر های نود نیست پس گمترل گردن دیگه لازم نیست
//						continue;
//					}
//
//					bool baddchild = true;
//					for ( auto & parent : ( link->d_pDestination )->d_parents ) // مطمئن می شویم نود پدر در گراف جدید ساخته شده باشد
//					{
//						////if ( control.count( parent->d_node_uniq_id ) == 0 ) // پدرش دیده نشده بود
//						////{
//						////    baddchild = false;
//						////    break;
//						////}
//						if ( control.count( parent->d_node_uniq_id ) != 0 && control[ parent->d_node_uniq_id ] >= mgr.pScheduler->d_scheduleIteration )
//						{
//							// فرزند در همان دوره پدر زمانبندی نمی شود
//							baddchild = false; // اگر هنوز در همان اسکجولی هستیم که پدر درش زمانبندی شده فرزند که نمی تواند در همان زمانبندی شود
//							break;
//						}
//						////if ( !mgr.is_task_ended( parent->d_node_uniq_id ) )
//						////{
//						////    baddchild = false;
//						////    break;
//						////}
//					}
//					if ( baddchild )
//					{
//						if ( control.count( ( link->d_pDestination )->d_node_uniq_id ) == 0 )
//						{
//							inlist_node.push_back( link->d_pDestination );
//							control[ link->d_pDestination->d_node_uniq_id ] = 0;
//							////mgr.map_task_to_vm( now , link->d_pDestination );
//							node_link_in_list[ node->d_node_uniq_id ] += 1;
//						}
//					}
//				}
//			}
//		}
//	};
//	double time_consume_to_exec = mgr.d_now;
//
//	for ( auto & node : *this->d_pRuntime->d_ordered_nodes )
//	{
//		node->d_ended_parent = 0;
//	}
//
//	if ( SETTINGS_FAST()->d_echo_minimum_maximum_calc_proc_dag_on_vm )
//	{
//		XBT_INFO( " serially time %s (%f) " , machine.get_vmtypeName() , time_consume_to_exec );
//	}
//
//	return time_consume_to_exec;
//	CATCH_TRY
//	return 0;
//}

DurationTime DAG::execute_parallel_time( VMachine machine , int extra_vm_leased , int * pMaximumExtraParallelVM , int * pMaximumPossibleParallelVM ,
										 map< eNodeID , DurationTime /*each node time duration*/> * pStoreDuration )
{
	BEGIN_TRY

	if ( d_pRuntime->d_ordered_nodes == NULL )
	{
		make_ordered_nodes();
	}
	////auto & slowest_vm = VMachine::().front(); // slowest
	for ( auto & node : *this->d_pRuntime->d_ordered_nodes )
	{
		node->d_ended_parent = 0;
	}
	// up down methode
	// hierarchical assignment
	// infinite loop
	vector< DAGNode * > inlist_node;
	map< eNodeID , int /*schedule iteration*/ > control;
	VMachine_Executer_Mgr mgr( machine );
	map< eNodeID , int > node_link_scheduled; // میگه هر نود کدام لینکاش اسکجول شده و اگر همش اسکجول شده دیگه کنترل کردن برای اسکجول یکیش بی مورده
	for ( auto & node : d_pRuntime->d_roots )
	{
		inlist_node.push_back( node );
		double dduration = std::get<1>(mgr.map_task_to_vm( 0 , node ));
		assert( dduration > 0 );
		if ( pStoreDuration != NULL )
		{
			( *pStoreDuration )[ node->d_node_uniq_id ] = dduration; // caution . index is very important
		}
		control[ node->d_node_uniq_id ] = mgr.pScheduler->d_scheduleIteration;
	}
	while( 1 )
	{
		double now = mgr.next_discret_time_simulate();

		if ( mgr.node_endded.size() == this->d_pRuntime->d_ordered_nodes->size() )
		{
			bool break_loop = true;
			for ( auto & node : *this->d_pRuntime->d_ordered_nodes )
			{
				if ( !mgr.is_task_ended( node->d_node_uniq_id ) )
				{
					break_loop = false;
					break;
				}
			}
			if ( break_loop )
			{
				break;
			}
		}

		vector< DAGNode * > tmp_inlist_node( inlist_node );
		for ( auto & node : tmp_inlist_node )
		{
			if ( mgr.is_task_ended( node->d_node_uniq_id ) && node_link_scheduled[node->d_node_uniq_id] != node->d_links.size() )
			{
				for ( auto & link : node->d_links )
				{
					if ( control.count( link->d_pDestination->d_node_uniq_id ) != 0 && control[ link->d_pDestination->d_node_uniq_id ] )
					{
						// این لینک ایکجول شده
						continue;
					}

					if ( link->d_pDestination->d_ended_parent < ( link->d_pDestination )->d_parents.size() )
					{
						// این لینک پدر هاش تعدادشون به تعداد پدر های نود نیست پس گمترل گردن دیگه لازم نیست
						continue;
					}

					bool baddchild = true;
					for ( auto & parent : ( link->d_pDestination )->d_parents ) // مطمئن می شویم نود پدر در گراف جدید ساخته شده باشد
					{
						if ( control.count( parent->d_node_uniq_id ) == 0 )
						{
							baddchild = false;
							break;
						}
						if ( control.count( parent->d_node_uniq_id ) != 0 && control[ parent->d_node_uniq_id ] >= mgr.pScheduler->d_scheduleIteration )
						{
							baddchild = false; // اگر هنوز در همان اسکجولی هستیم که پدر درش زمانبندی شده فرزند که نمی تواند در همان زمانبندی شود
							break;
						}
						if ( !mgr.is_task_ended( parent->d_node_uniq_id ) )
						{
							baddchild = false;
							break;
						}
					}
					if ( baddchild )
					{
						if ( control.count( ( link->d_pDestination )->d_node_uniq_id ) == 0 )
						{
							double dduration = -1;
							if ( ( dduration = std::get<1>( mgr.map_task_to_vm( now , link->d_pDestination , extra_vm_leased < 0 ? -1 : (int)d_pRuntime->d_roots.size() + extra_vm_leased ) ) ) > 0 )
							{
								if ( pStoreDuration != NULL )
								{
									( *pStoreDuration )[ link->d_pDestination->d_node_uniq_id ] = dduration; // link destination value is set
								}

								node_link_scheduled[ node->d_node_uniq_id ] += 1; // parent link is increase
								inlist_node.push_back( link->d_pDestination ); // child add to list
								control[ ( link->d_pDestination )->d_node_uniq_id ] = mgr.pScheduler->d_scheduleIteration; // child schedule iteration is set
							}
						}
					}
				}
			}
		}
	};
	if ( pMaximumPossibleParallelVM != NULL )
	{
		*pMaximumPossibleParallelVM = mgr.lst_vms.size() - 1;
	}
	if ( pMaximumExtraParallelVM != NULL )
	{
		assert( mgr.pScheduler != NULL );
		assert( mgr.lst_vms.size() >= 2 );
		assert(d_pRuntime->d_roots.size() > 0);
		if ( mgr.lst_vms.size() - 1 < d_pRuntime->d_roots.size() )
		{
			assert(0);
			XBT_INFO( "#(%d) warning" , __LINE__ ); // donot comment this
		}
		//XBT_INFO_ALIVE_CHECK( "#(%d) mgr.lst_vms.size()=%d d_roots.size()=%d" , __LINE__ , (int)mgr.lst_vms.size() , (int)d_roots.size() );
		( *pMaximumExtraParallelVM ) = mgr.lst_vms.size() - 1 - d_pRuntime->d_roots.size();
	}

	double time_consume_to_exec = mgr.d_now;

	for ( auto & node : *this->d_pRuntime->d_ordered_nodes )
	{
		node->d_ended_parent = 0;
	}
	if ( SETTINGS_FAST()->d_echo_minimum_maximum_calc_proc_dag_on_vm )
	{
		XBT_INFO( " longest_time(%f) " , time_consume_to_exec );
	}

	return time_consume_to_exec;
	CATCH_TRY
	return 0;
}

DurationTime DAG::shortest_time()
{
	BEGIN_TRY

		if ( d_pRuntime->d_ordered_nodes == NULL )
		{
			make_ordered_nodes();
		}
	auto & slowest_vm = VMachine::get_type_instances().back(); // fastest
	double time_consume_to_exec = 0;// SETTINGS_FAST()->d_VIRTUALMACHINE_WARMUP_TIME_SEC + SETTINGS_FAST()->d_CONTAINER_INITIALIZATION_DALEY_TIME; // در بلند مدت ماشین گیر میاید

	// اینجا می خواهم هر حجم پروسس را در نظر بگیرم و هم حجم تسک را و هم حجم دیتای انتقالی را
	for ( const auto & ref_node : *d_pRuntime->d_ordered_nodes )
	{
		// read task from gs
		time_consume_to_exec += ref_node->d_task_size_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;

		// transfer to vm
		time_consume_to_exec += ref_node->d_task_size_byte / slowest_vm.d_wan_network_speed;

		// write task on vm
		time_consume_to_exec += ref_node->d_task_size_byte / slowest_vm.d_memory_access_speed;

		// read task input from gs
		// transfer task input to vm
		for ( const auto & ref_node_parent : ref_node->d_parents )
		{
			time_consume_to_exec += ref_node_parent->d_task_output_size_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;
			time_consume_to_exec += ref_node_parent->d_task_output_size_byte / slowest_vm.d_wan_network_speed;
			time_consume_to_exec += ref_node_parent->d_task_output_size_byte / slowest_vm.d_memory_access_speed;
		}

		// exec time
		time_consume_to_exec += ref_node->d_task_cpu_usage_flops / slowest_vm.d_cpu_speed;

		// write output to vm
		time_consume_to_exec += ref_node->d_task_output_size_byte / slowest_vm.d_memory_access_speed;

		// transfer output to gs
		time_consume_to_exec += ref_node->d_task_output_size_byte / slowest_vm.d_wan_network_speed;

		// write output to gs
		time_consume_to_exec += ref_node->d_task_output_size_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;
	}

	if ( SETTINGS_FAST()->d_echo_minimum_maximum_calc_proc_dag_on_vm )
	{
		XBT_INFO( " longest_time(%f) " , time_consume_to_exec );
	}

	return time_consume_to_exec;
	CATCH_TRY
		return 0;
}

DurationTime DAG::longest_time()
{
	BEGIN_TRY

		if ( d_pRuntime->d_ordered_nodes == NULL )
		{
			make_ordered_nodes();
		}
	auto & slowest_vm = VMachine::get_type_instances().front(); // slowest
	double time_consume_to_exec = 0;// SETTINGS_FAST()->d_VIRTUALMACHINE_WARMUP_TIME_SEC + SETTINGS_FAST()->d_CONTAINER_INITIALIZATION_DALEY_TIME;

	// اینجا می خواهم هر حجم پروسس را در نظر بگیرم و هم حجم تسک را و هم حجم دیتای انتقالی را
	for ( const auto & ref_node : *d_pRuntime->d_ordered_nodes )
	{
		// read task from gs
		time_consume_to_exec += ref_node->d_task_size_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;

		// transfer to vm
		time_consume_to_exec += ref_node->d_task_size_byte / slowest_vm.d_wan_network_speed;

		// write task on vm
		time_consume_to_exec += ref_node->d_task_size_byte / slowest_vm.d_memory_access_speed;

		// read task input from gs
		// transfer task input to vm
		for ( const auto & ref_node_parent : ref_node->d_parents )
		{
			time_consume_to_exec += ref_node_parent->d_task_output_size_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;
			time_consume_to_exec += ref_node_parent->d_task_output_size_byte / slowest_vm.d_wan_network_speed;
			time_consume_to_exec += ref_node_parent->d_task_output_size_byte / slowest_vm.d_memory_access_speed;
		}

		// exec time
		time_consume_to_exec += ref_node->d_task_cpu_usage_flops / slowest_vm.d_cpu_speed;

		// write output to vm
		time_consume_to_exec += ref_node->d_task_output_size_byte / slowest_vm.d_memory_access_speed;

		// transfer output to gs
		time_consume_to_exec += ref_node->d_task_output_size_byte / slowest_vm.d_wan_network_speed;

		// write output to gs
		time_consume_to_exec += ref_node->d_task_output_size_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;
	}

	if ( SETTINGS_FAST()->d_echo_minimum_maximum_calc_proc_dag_on_vm )
	{
		XBT_INFO( " longest_time(%f) " , time_consume_to_exec );
	}

	return time_consume_to_exec;
	CATCH_TRY
	return 0;
}

//DurationTime DAG::time_comsume_to_process_on( int * pMaximumExtraParallelVM )
//{
//	BEGIN_TRY
//	////VMachine::().front().dump( true ); // first . weakest . cheapest
//	////VMachine::().back().dump( true ); // last . strongest . expensive
//	DurationTime min_time_consume = -1;
//	DurationTime max_time_consume = -1;
//
//	if ( SETTINGS_FAST()->d_reset_minimum_estimate_time_consumption != -1 )
//	{
//		XBT_INFO( "#(%d) warning" , __LINE__ ); // donot comment this
//		min_time_consume = SETTINGS_FAST()->d_reset_minimum_estimate_time_consumption;
//	}
//	if ( SETTINGS_FAST()->d_reset_maximum_estimate_time_consumption != -1 )
//	{
//		XBT_INFO( "#(%d) warning" , __LINE__ ); // donot comment this
//		max_time_consume = SETTINGS_FAST()->d_reset_maximum_estimate_time_consumption;
//	}
//
//	if ( min_time_consume < 0 )
//	{
//		min_time_consume = execute_parallel_time( VMachine::strongest_machine() , -1 , pMaximumExtraParallelVM , &d_MaximumPossibleParallelism );
//	}
//
//	if ( max_time_consume < 0 )
//	{
//		max_time_consume = execute_serially_on_one_machine_time( VMachine::weakest_machine() );
//	}
//
//	////if ( min_time_consume > max_time_consume )
//	////{
//	////	XBT_INFO_ALIVE_CHECK( "#(%d) min_time_consume > max_time_consume %s" , __LINE__ , this->get_name().c_str() );
//	////}
//
//	//assert( min_time_consume <= max_time_consume );
//
//	////if ( min_time_consume < 0 || max_time_consume < 0 )
//	//{
//		vector< double > time_need;
//	//	// در دگ های چاق اجرای سری مینیمم و ماکزیمم را درست می کند و در دگ های دراز اجرای موازی
//
//		time_need.push_back( min_time_consume );
//		time_need.push_back( max_time_consume );
//		double minn = *std::min_element(time_need.begin(), time_need.end());
//		double maxx = *std::max_element(time_need.begin(), time_need.end());
//		min_time_consume = minn;
//		max_time_consume = maxx;
//	
//	double deadline = 0;
//	{
//		////double minn = std::min(min_time_consume,max_time_consume);
//		////double maxx = std::max(min_time_consume,max_time_consume);
//
//		deadline = simgrid::xbt::random::uniform_real( min_time_consume , max_time_consume + 1 );
//
//		if ( SETTINGS_FAST()->d_echo__time_comsume_to_process_on__deadline )
//		{
//			XBT_INFO( "simgrid::xbt::random::uniform_real( %f , %f )==(%f) (%f%%)" , min_time_consume , max_time_consume , deadline ,
//					  ( deadline - min_time_consume ) * 100 / ( max_time_consume - min_time_consume ) );
//		}
//	}
//
//	if ( SETTINGS_FAST()->d_echo_average_deadline_calced_for_dag )
//	{
//		XBT_INFO( "%s , min_total_time_consume(%f) , max_total_time_consume(%f) , avg_deadline(%f)" , this->get_name().c_str() , min_time_consume , max_time_consume , deadline );
//	}
//	return deadline;
//	CATCH_TRY
//	return 0;
//}

DurationTime DAG::time_comsume_to_process_on()
{
	BEGIN_TRY
		//VMachine::get_type_instances().front().dump( true ); // first . weakest . cheapest
		//VMachine::get_type_instances().back().dump( true ); // last . strongest . expensive
		DurationTime min_time_consume = -1;
	if ( SETTINGS_FAST()->d_reset_minimum_estimate_time_consumption != -1 )
	{
		XBT_INFO( "#(%d) warning" , __LINE__ ); // donot comment this
		min_time_consume = SETTINGS_FAST()->d_reset_minimum_estimate_time_consumption;
	}
	if ( min_time_consume < 0 )
	{
		min_time_consume = shortest_time();
	}
	DurationTime max_time_consume = -1;
	if ( SETTINGS_FAST()->d_reset_maximum_estimate_time_consumption != -1 )
	{
		XBT_INFO( "#(%d) warning" , __LINE__ ); // donot comment this
		max_time_consume = SETTINGS_FAST()->d_reset_maximum_estimate_time_consumption;
	}
	if ( max_time_consume < 0 )
	{
		max_time_consume = longest_time();
	}

	double deadline = 0;
	{
		deadline = simgrid::xbt::random::uniform_real( min_time_consume , max_time_consume );

		if ( SETTINGS_FAST()->d_echo__time_comsume_to_process_on__deadline )
		{
			XBT_INFO( "simgrid::xbt::random::uniform_real( %f , %f )==(%f) (%f%%)" , min_time_consume , max_time_consume , deadline ,
					  ( deadline - min_time_consume ) * 100 / ( max_time_consume - min_time_consume ) );
		}
	}

	if ( SETTINGS_FAST()->d_echo_average_deadline_calced_for_dag )
	{
		XBT_INFO( "%s , min_total_time_consume(%f) , max_total_time_consume(%f) , avg_deadline(%f)" , this->d_pRuntime->d_dag_short_name.c_str() , min_time_consume , max_time_consume , deadline );
	}
	return deadline;
	CATCH_TRY
		return 0;
}

void DAG::dump( bool inside_another , bool b_short )
{
	BEGIN_TRY
	if ( d_pRuntime->d_ordered_nodes == NULL )
	{
		make_ordered_nodes();
	}

	if ( !inside_another ) XBT_INFO_EMPTY_LINE();
	XBT_INFO( "dump %s" , this->get_name().c_str() );

		d_dag_deadline.dump( true );

	for ( auto & node : *d_pRuntime->d_ordered_nodes )
	{
		node->dump(false , false);
	}

	////if ( !b_short )
	////{
	////	for ( auto & node : *d_ordered_nodes )
	////	{
	////		for ( auto & ref_link : node->d_links )
	////		{
	////			ref_link->dump( false );
	////		}
	////	}
	////}
	CATCH_TRY
}

void DAG::calc_edge_weight_normalize()
{
	if ( d_pRuntime->d_shuffled_nodes == NULL )
	{
		make_shuffled_nodes();
	}

	long double sumx2 = 0;
	long double sumx = 0;
	int count = 0;
	for ( const auto & ref_node : *d_pRuntime->d_shuffled_nodes )
	{
		for ( auto & ref_edge : ref_node.second->d_links )
		{
			ref_edge->d_p_network_usage_byte = &(ref_node.second->d_task_output_size_byte);

			sumx2 += pow( *ref_edge->d_p_network_usage_byte , 2 );
			sumx += *ref_edge->d_p_network_usage_byte;
			count++;
		}
	}
	// می خواهیم به هر یال عددی نسبت دهیم که هرچه وزن یال بیشتر باشد عدد کمتری پیدا کند
	// خب ببین از میانگین کنترا هارمونیک استفاده کردم بعد ضربدر تعداد کردم یعنی عدد بزرگتر بیشتر میشه و سپس تقسیم بر عدد هر یال میشه پس هرکی کوچکتر است مفدار بیشتر بر می دارد
	for ( const auto & ref_node : *d_pRuntime->d_shuffled_nodes )
	{
		for ( auto & edge : ref_node.second->d_links )
		{
			edge->normalized_inverse_network_usage = ( sumx2 * count ) / ( *edge->d_p_network_usage_byte * sumx );
		}
	}
}

bool calculate_nodes_sort_order_wrapper_fxn( DAG * pDAG )
{
	map< eNodeID , int/*index*/ > node_index;
	map< int/*index*/ , eNodeID > index_node;

	for ( int i = 0 ; i < pDAG->d_nodes_count ; i++ )
	{
		node_index[ pDAG->d_pRuntime->d_ordered_nodes->at( i )->d_node_uniq_id ] = i;
		index_node[ i ] = pDAG->d_pRuntime->d_ordered_nodes->at( i )->d_node_uniq_id;
	}
	////XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	mat mat_adjacent = zeros<mat>( pDAG->d_nodes_count , pDAG->d_nodes_count );
	////XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	for ( auto & ref_node : *(pDAG->d_pRuntime->d_ordered_nodes) )
	{
		for ( auto & edge : ref_node->d_links )
		{
			if ( *edge->d_p_network_usage_byte <= 0.0 )
			{
				////XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d)  %s" , __LINE__ , this->get_name().c_str() );
				return false;
			}
			mat_adjacent.at( node_index[ ref_node->d_node_uniq_id ] , node_index[ (edge->d_pDestination)->d_node_uniq_id ] ) = *edge->d_p_network_usage_byte;
			////mat_adjacent.at( node_index[ ref_node->d_node_uniq_id ] , node_index[ edge->d_destination->d_node_uniq_id ] ) = 1;
		}
	}
	////XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	mat_adjacent = mat_adjacent + trans( mat_adjacent );
	////XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	mat I = ones<mat>( pDAG->d_nodes_count , pDAG->d_nodes_count );
	mat dI = eye<mat>( pDAG->d_nodes_count , pDAG->d_nodes_count );
	////XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	vec eigval;
	mat eigvec;
	////time_t t1;
	////time( &t1 );

	if ( eig_sym( eigval , eigvec , mat_adjacent ) )
	{
		////time_t t2;
		////time( &t2 );
		////if ( difftime( t2 , t1 ) > 5 )
		////{
		////	return false;
		////}

		////XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
		double mev = *max_element( eigval.begin() , eigval.begin() + eigval.size() );
		if ( mev <= 0.0 )
		{
			////XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d)  %s" , __LINE__ , this->get_name().c_str() );
			return false;
		}
		////XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) mev(%f)" , __LINE__ , mev );
		////mat katz;

		//	//mat B;
		//	//bool fail = inv( B , dI - ( 0.99 / mev ) * mat_adjacent );
		////XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d)  " , __LINE__ );
		////if ( !fail ) return false;
		mat katz = ( inv( dI - ( 0.99 / mev ) * mat_adjacent ) - dI ) * I;

		////XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
		for ( int i = 0 ; i < pDAG->d_nodes_count ; i++ )
		{
			( *pDAG->d_pRuntime->d_shuffled_nodes )[ index_node[ i ] ]->d_Katz_value = katz.at( i , 0 );
			////XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d)   katz=%f" , __LINE__  , katz.at( i , 0 )  );
		}
		////XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
		return true;
	}

	return false;
}

////bool calculate_nodes_sort_order__wrapper( DAG * pDAG , string & sresult )
////{
////	std::mutex m;
////	std::condition_variable cv;
////	bool retValue = false;
////	sresult = "";
////
////	std::thread t( [ &cv , &retValue ]( DAG * pDAG )
////	{
////		retValue = calculate_nodes_sort_order_wrapper_fxn( pDAG );
////		cv.notify_one();
////	} , pDAG );
////
////	t.detach();
////
////	{
////		std::unique_lock<std::mutex> lock( m );
////		if ( cv.wait_for( lock , 30s ) == std::cv_status::timeout )
////		{
////			sresult = "timeout";
////			retValue = false;
////		}
////	}
////
////	return retValue;
////}

bool DAG::calculate_nodes_sort_order()
{
	BEGIN_TRY
		if ( d_pRuntime->d_ordered_nodes == NULL )
		{
			make_ordered_nodes();
		}

		//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d)  %s" , __LINE__ , this->get_name().c_str() );
		if ( SETTINGS_FAST()->d_calculate_katz )
		{
			if ( !calculate_nodes_sort_order_wrapper_fxn( this ) )
			{
				XBT_INFO( "#(%d) calculate_nodes_sort_order_wrapper_fxn fault %s" , __LINE__ , get_name().c_str() );
				return false;
			}

			//string sresult;
			////if ( !calculate_nodes_sort_order__wrapper( this , sresult ) )
			////{
			////	XBT_INFO( "#(%d) wrapper fault(%s) %s" , __LINE__ , sresult.c_str() , get_name().c_str() );
			////	return false;
			////}
		}
		else
		{
			_ONE_TIME_VAR();
			_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
		}
		//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
		return true;
	CATCH_TRY
	return false;
}

bool DAG::complete_dag_structure( maketime_data * pmaketime_data , tuple_dag_configure * pcache )
{
	BEGIN_TRY
	if ( d_pRuntime->d_shuffled_nodes == NULL )
	{
		make_shuffled_nodes();
	}
	this->d_pRuntime->d_dag_short_name = string_format( "dag_dgdxid%d_cnt%d" , this->d_dag_dax_id , this->d_nodes_count );
	
	//XBT_INFO( "#(%d) " , __LINE__ );

	//_GUID dag_guid = this->get_tag( "GUID" );
	//assert( dag_guid.length() > 0 );

	////if ( this->d_dag_type.d_dag_type != eDAGType::temp_dag )
	{
		check_dag();
	}
	
	for ( auto & ref_node : *d_pRuntime->d_shuffled_nodes )
	{
		ref_node.second->d_pOwnerDAG = this;
	}

	// calc edge weight normalize
	////if ( this->d_dag_type.d_dag_type != eDAGType::temp_dag )
	{
		calc_edge_weight_normalize();
	}

	//XBT_INFO_ALIVE_CHECK( "#(%d)" , __LINE__ );

	////tuple< vector< tuple< VMachine , double/*deadline*/ , double /*TimeConsume*/ , int/*parallism*/ , int /*maxParallism*/ ,
	////	map< int/*nodedaxid*/ , DurationTime /*each node time duration*/ > > > , map< int/*nodedaxid*/ , double/*katz*/ > > cached_config;

	// calc deadline quota
	{
		this->d_dag_deadline.clear();
		this->d_dag_deadline.d_paired_dag = this;

		////if ( !SETTINGS_FAST()->d_make_dag_guid_config && SETTINGS_FAST()->d_use_dag_guid_config_cache )
		////{
		////	cached_config = get_xml_dag_guid_config( dag_guid );
		////	std::random_device rd;
		////	std::mt19937 generator( rd() );
		////	std::shuffle( std::get<0>( cached_config ).begin() , std::get<0>( cached_config ).end() , generator );
		////}

		// find suitable vm type
		maketime_data default_instance; // try use outside instance but if its not available use inside instead
		////DurationTime TotalTimeConsume = 0;
		////map< eNodeID , DurationTime /*each node time duration*/> node_exec_time_duration;
		////VMachine  base_least_price_vm;
		////VMachine * p_least_price_vm = NULL;
		////double zarib_afzayesh = SETTINGS_FAST()->d_increase_deadline_after_no_machine_match; // گاهی زمان دستی ست می شود و زمان کن باعق می شود که هیچ ماشینی هیچ وقت نتواند کار را انجام دهد فلذا هر دور زمان را کمی زیادتر می کنیم
		
		if ( pmaketime_data == NULL )
		{
			pmaketime_data = &default_instance;
		}
		
		int loop_counter = 0;
		// infinite loop
		do
		{
			pmaketime_data->cleanup();

			pmaketime_data->TotalTimeConsume = std::numeric_limits<double>::max();

			loop_counter++;
			//if ( pTryCount != NULL )
			//{
			//	*pTryCount++;
			//}
			if ( loop_counter > 50 ) return false;
			
			//XBT_INFO( "#(%d) " , __LINE__ );

			//d_MaximumPossibleParallelism = 0;
			this->d_dag_deadline.d_dag_requestor_deadline_duration = 0;

			// try use cached config
			//if ( !SETTINGS_FAST()->d_make_dag_guid_config && std::get<0>( cached_config ).size() > 0 )
			//{
			//	this->d_dag_deadline.d_dag_requestor_deadline_duration = std::get<1>( std::get<0>( cached_config ).front() );
			//}
			if ( pcache != NULL )
			{
				this->d_dag_deadline.d_dag_requestor_deadline_duration = std::get<1>( std::get<2>( *pcache ) );
				assert( this->d_dag_deadline.d_dag_requestor_deadline_duration > 0 );
			}

			// if deadline empty try calc random one
			////this->d_dag_deadline.d_dag_requestor_deadline_duration = 10000;
			if ( this->d_dag_deadline.d_dag_requestor_deadline_duration <= 0 )
			{
				this->d_dag_deadline.d_dag_requestor_deadline_duration = time_comsume_to_process_on();
				////assert( d_MaximumPossibleParallelism > 0 );
				////pmaketime_data->ExtraParallelVM = simgrid::xbt::random::uniform_int( 0 , d_MaximumPossibleParallelism );
			}
			////string sresult;
			////if ( !time_comsume_to_process_on__wrapper( this , this->d_dag_deadline.d_dag_requestor_deadline_duration , &MaximumExtraParallelVM , sresult ) )
			////{
			////	XBT_INFO( "#(%d) wrapper fault(%s) %s" , __LINE__ , sresult.c_str() , get_name().c_str() );
			////	continue;
			////}

			////this->d_dag_deadline.d_dag_requestor_deadline_duration = this->time_comsume_to_process_on( &MaximumExtraParallelVM );
			
			//XBT_INFO_ALIVE_CHECK( "#(%d) MaximumExtraParallelVM %d" , __LINE__ , MaximumExtraParallelVM );

			// use cached parallism
			//////if ( !SETTINGS_FAST()->d_make_dag_guid_config && std::get<0>( cached_config ).size() > 0 )
			//////{
			//////	auto & random_vector_item = std::get<0>( cached_config ).front();
			//////	d_MaximumPossibleParallelism = std::get<4>( random_vector_item );
			//////	ExtraParallelVM = std::get<3>( random_vector_item );
			//////	base_least_price_vm = std::get<0>( random_vector_item );
			//////	p_least_price_vm = &base_least_price_vm;
			//////	TotalTimeConsume = std::get<2>( random_vector_item );
			//////	for ( auto & qouta : std::get<5>( random_vector_item ) )
			//////	{
			//////		for ( auto & node : *d_pRuntime->d_ordered_nodes )
			//////		{
			//////			if ( node->d_node_dax_id == qouta.first )
			//////			{
			//////				node_exec_time_duration[ node->d_node_uniq_id ] = qouta.second;
			//////				break;
			//////			}
			//////		}
			//////	}
			//////}
			if ( pcache != NULL )
			{
				//d_MaximumPossibleParallelism = std::get<4>( std::get<2>( *pcache ) );
				//pmaketime_data->ExtraParallelVM = std::get<3>( std::get<2>( *pcache ) );
				pmaketime_data->base_least_price_vm = std::get<0>( std::get<2>( *pcache ) );
				pmaketime_data->p_least_price_vm = &pmaketime_data->base_least_price_vm;
				pmaketime_data->TotalTimeConsume = std::get<2>( std::get<2>( *pcache ) );
				for ( auto & qouta : std::get<5>( std::get<2>( *pcache ) ) )
				{
					for ( auto & node : *d_pRuntime->d_ordered_nodes )
					{
						if ( node->d_node_dax_id == qouta.first )
						{
							pmaketime_data->node_exec_time_duration[ node->d_node_uniq_id ] = qouta.second;
							break;
						}
					}
				}
				assert( pmaketime_data->node_exec_time_duration.size() == d_pRuntime->d_ordered_nodes->size() );
				execute_parallel_time( pmaketime_data->base_least_price_vm , -1 , NULL , &d_MaximumPossibleParallelism );
			}


			if ( pmaketime_data->p_least_price_vm == NULL )
			{
				//XBT_INFO( "#(%d) " , __LINE__ );
				for ( const VMachine & ordered_vm : VMachine::get_type_instances() )
				{
					pmaketime_data->node_exec_time_duration.clear();

					switch ( SETTINGS_FAST()->d_calculate_dag_execution_duration_ver )
					{
						case 3:
						case 1:
						{
							//_ONE_TIME_VAR();
							//_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
							pmaketime_data->TotalTimeConsume = 0;
							//if ( SETTINGS_FAST()->d_calculate_dag_execution_duration_ver == 1 )
							//{
							//	pmaketime_data->TotalTimeConsume += SETTINGS_FAST()->d_VIRTUALMACHINE_WARMUP_TIME_SEC + SETTINGS_FAST()->d_CONTAINER_INITIALIZATION_DALEY_TIME;
							//}
							for ( const auto & ref_node : *d_pRuntime->d_ordered_nodes )
							{
								DurationTime TaskTimeConsume = 0; // for each node
								double total_input_size = 0;
								////if ( ref_node->d_parents.size() == 0 )
								////{
								////	TaskTimeConsume += SETTINGS_FAST()->d_VIRTUALMACHINE_WARMUP_TIME_SEC + SETTINGS_FAST()->d_CONTAINER_INITIALIZATION_DALEY_TIME;
								////}
								////else
								{
									for ( const auto & ref_parent : ref_node->d_parents )
									{
										total_input_size += ref_parent->d_task_output_size_byte; // چرا باید بعضی از یالها زمانش حساب بشه بزار همش حساب بشه
									}
								}
								TaskTimeConsume += ( ref_node->d_task_size_byte + total_input_size + ref_node->d_task_output_size_byte ) / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED; // read task . input & write output
								TaskTimeConsume += ordered_vm.get_duration_datatransfer_on_wan( ref_node->d_task_size_byte + total_input_size + ref_node->d_task_output_size_byte ); // transfer task . input & output
								TaskTimeConsume += ordered_vm.get_duration_datatransfer_on_mem( ref_node->d_task_size_byte + total_input_size ); // write task input to vm mem
								TaskTimeConsume += ordered_vm.get_execution_duration( ref_node->d_task_cpu_usage_flops ); // exec task
								TaskTimeConsume += ordered_vm.get_duration_datatransfer_on_mem( ref_node->d_task_output_size_byte ); // write task output to vm
								pmaketime_data->node_exec_time_duration[ ref_node->d_node_uniq_id ] = TaskTimeConsume;
								pmaketime_data->TotalTimeConsume += TaskTimeConsume;
							}
							break;
						}
						case 2:
						{
							_ONE_TIME_VAR();
							_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
							//assert( pmaketime_data->ExtraParallelVM >= 0 );
							////string sresult;
							////if ( !execute_parallel_time__wrapper( this , ordered_vm , TotalTimeConsume , ExtraParallelVM , &node_exec_time_duration , sresult ) )
							////{
							////	XBT_INFO( "#(%d) wrapper (||%d) (rot%d) fault(%s) %s" , __LINE__ , ExtraParallelVM , (int)this->d_roots.size() , sresult.c_str() , get_name().c_str());
							////	continue;
							////}
							//pmaketime_data->TotalTimeConsume = execute_parallel_time( ordered_vm , pmaketime_data->ExtraParallelVM , NULL , NULL , &pmaketime_data->node_exec_time_duration );
							////for ( auto & node : *this->d_ordered_nodes )
							////{
							////	node_exec_time_duration[ node->d_node_uniq_id ] = 1000;
							////}
							////TotalTimeConsume = this->d_nodes_count * 1000;

							assert( pmaketime_data->TotalTimeConsume > 0 );
							//XBT_INFO( "#(%d) " , __LINE__ );
							break;
						}
						default:
						{
							_ONE_TIME_VAR();
							_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
							break;
						}
					}

					if ( pmaketime_data->TotalTimeConsume > 0 && pmaketime_data->TotalTimeConsume <= this->d_dag_deadline.d_dag_requestor_deadline_duration )
					{
						pmaketime_data->base_least_price_vm = ordered_vm;
						pmaketime_data->p_least_price_vm = &pmaketime_data->base_least_price_vm;
						// یک ماشینی هست که می تواند قبل از ددلاین دگ را بپایان برساند
						//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_high , "#(%d) winner vm is(%s) in total time(%f) , dag(%s)" , __LINE__ , ordered_vms.get_vmtypeName().c_str() , TotalTimeConsume , this->get_name().c_str() );
						break;
					}
					else
					{
						//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_medium , "#(%d) not winner vm is(%s) in total time(%f)" , __LINE__ , ordered_vms.get_vmtypeName().c_str() , TotalTimeConsume );
						continue;
					}
				}
			} // if ( p_least_price_vm == NULL )

			//////if ( p_least_price_vm != NULL && SETTINGS_FAST()->d_make_dag_guid_config )
			//////{
			//////	//XBT_INFO( "#(%d) " , __LINE__ );
			//////	if ( std::get<0>( get_xml_dag_guid_config( dag_guid ) ).size() < SETTINGS_FAST()->d_make_dag_n_guid_config )
			//////	{
			//////		XBT_INFO( "#(%d) found one dag(%d) guid config %f %d" , __LINE__ , this->d_dag_dax_id , this->d_dag_deadline.d_dag_requestor_deadline_duration , ExtraParallelVM );
			//////		
			//////		map< int/*nodedaxid*/ , DurationTime /*each node time duration*/> quotalist;
			//////		for ( auto & dur : node_exec_time_duration )
			//////		{
			//////			quotalist[ ( *d_pRuntime->d_shuffled_nodes )[ dur.first ]->d_node_dax_id ] = dur.second;
			//////		}
			//////		
			//////		add_dag_guid_config( dag_guid , *p_least_price_vm , this->d_dag_deadline.d_dag_requestor_deadline_duration , TotalTimeConsume , ExtraParallelVM , d_MaximumPossibleParallelism , quotalist , true );
			//////		p_least_price_vm = NULL;
			//////		loop_counter = 0;
			//////	}
			//////}

		} while ( pmaketime_data->p_least_price_vm == NULL );

		d_least_price_vmp_type = pmaketime_data->p_least_price_vm->d_type;

		// balakhareh yek vm bayad betavanad workflow ra pish az deadline beh payan beresanad
		// i choose renegotiation strategy
		if ( pmaketime_data->TotalTimeConsume <= this->d_dag_deadline.d_dag_requestor_deadline_duration )
		{
			//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_medium , "#(%d) TotalTimeConsume <= this->d_dag_deadline->d_dag_requestor_deadline_duration" , __LINE__ );

			// نوع ماشینی هست که بتواند پیش از موعد دگ را بپایان برساند
			this->d_dag_deadline.d_dag_process_duration_in_cheapest_and_feasible_machine = pmaketime_data->TotalTimeConsume;

			// زمان اضافه بیت تسک ها به نسبت تقسیم می شود
			DurationTime total_spare_time =
				this->d_dag_deadline.d_dag_requestor_deadline_duration -
				this->d_dag_deadline.d_dag_process_duration_in_cheapest_and_feasible_machine;

			//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_medium , "#(%d) spare time(%f s)" , __LINE__ , total_spare_time );

			if ( total_spare_time < 0.0 )
			{
				XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
			}

			for ( const auto & ref_node : *d_pRuntime->d_ordered_nodes )
			{
				////ref_node->d_task_deadline = make_shared< Task_Deadline_Quota >();
				ref_node->d_task_deadline.clear();
				ref_node->d_task_deadline.d_paired_node = ref_node;

				ref_node->d_task_deadline.d_task_quota_duration_based_on_process_need = pmaketime_data->node_exec_time_duration[ ref_node->d_node_uniq_id ];
				ref_node->d_task_deadline.d_task_spare_duration_quota =
					total_spare_time * ref_node->d_task_deadline.d_task_quota_duration_based_on_process_need / pmaketime_data->TotalTimeConsume;

				////assert( ref_node->d_task_deadline.d_task_quota_duration_based_on_process_need > 0 );
			}
		}
		else
		{
			XBT_INFO( "#(%d) warning" , __LINE__ ); // donot comment this
		//	// اگر هیچ نوع ماشینی نتوانست که پیش از ددلاین به پایان برساند زمان بر اساس زمان ددلاین مشخص می شود
		//	//this->d_dag_deadline.d_dag_process_duration_in_cheapest_and_feasible_machine = this->d_dag_deadline.d_dag_requestor_deadline_duration;

		//	//for ( const auto & ref_node : *d_ordered_nodes )
		//	//{
		//	//	//ref_node->d_task_deadline = make_shared< Task_Deadline_Quota >();
		//	//	ref_node->d_task_deadline.clear();
		//	//	ref_node->d_task_deadline.d_paired_node = ref_node;

		//	//	ref_node->d_task_deadline.d_task_quota_duration_based_on_process_need =
		//	//		node_exec_time_duration[ ref_node->d_node_uniq_id ] * this->d_dag_deadline.d_dag_requestor_deadline_duration / TotalTimeConsume;
		//	//	ref_node->d_task_deadline.d_task_spare_duration_quota = 0;
		//	//}
		}

		//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_medium , "#(%d) d_dag_process_duration_in_cheapest_and_feasible_machine(%f s)" , __LINE__ , this->d_dag_deadline->d_dag_process_duration_in_cheapest_and_feasible_machine );
	}

	//XBT_INFO_ALIVE_CHECK( "#(%d)" , __LINE__ );

	////if ( this->d_dag_type.d_dag_type != eDAGType::temp_dag )
	{
		bool b_calcylate_katz = true;
		if ( pcache != NULL )
		{
			if ( std::get<3>( *pcache ).size() > 0 )
			{
				bool one_katz_is_positive = false;
				for ( auto & row : std::get<3>( *pcache ) )
				{
					for ( auto & node : *this->d_pRuntime->d_ordered_nodes )
					{
						if ( node->d_node_dax_id == row.first )
						{
							node->d_Katz_value = row.second;
							b_calcylate_katz = false;
							if ( node->d_Katz_value > 0 )
							{
								one_katz_is_positive = true;
							}
							break;
						}
					}
				}
				assert( one_katz_is_positive > 0 );
			}
		}

		if ( b_calcylate_katz )
		{
			if ( !calculate_nodes_sort_order() )
			{
				return false;
			}
		}

		//if ( SETTINGS_FAST()->d_make_dag_guid_config )
		//{
		//	map< int/*nodedaxid*/ , double/*katz*/ > guid_katz;
		//	for ( auto & node : *this->d_pRuntime->d_ordered_nodes )
		//	{
		//		guid_katz[ node->d_node_dax_id ] = node->d_Katz_value;
		//	}
		//	add_dag_guid_katz( dag_guid , guid_katz , true );
		//}
	}

	//XBT_INFO( "#(%d) " , __LINE__ );

	return true;
	CATCH_TRY
	return false;
}

//bool complete_dag_structure__wrapper( DAG * pDAG , string & sresult )
//{
//	std::mutex m;
//	std::condition_variable cv;
//	bool retValue = false;
//	sresult = "";
//
//	std::thread t( [ &cv , &retValue ]( DAG * pDAG )
//	{
//		retValue = pDAG->complete_dag_structure();
//		cv.notify_one();
//	} , pDAG );
//
//	t.detach();
//
//	{
//		std::unique_lock<std::mutex> lock( m );
//		if ( cv.wait_for( lock , 360s ) == std::cv_status::timeout ) // exception occure middle of complete_dag_structure somewhere randomly and say nothing but exception
//		{
//			sresult = "timeout";
//			retValue = false;
//		}
//	}
//
//	return retValue;
//}

void DAG::check_dag()
{
	BEGIN_TRY
	if ( d_pRuntime->d_shuffled_nodes == NULL )
	{
		make_shuffled_nodes();
	}

	for ( const auto & ref_node : *d_pRuntime->d_ordered_nodes )
	{
		for ( const auto & ref_parent_node : ref_node->d_parents )
		{
			bool b_parent_child_link_stable = false;
			for ( const auto & ref_edge : ref_parent_node->d_links )
			{
				if ( (ref_edge->d_pDestination)->d_node_uniq_id == ref_node->d_node_uniq_id )
				{
					b_parent_child_link_stable = true;
					break;
				}
			}
			if ( !b_parent_child_link_stable )
			{
				XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible . parent(%s) child(%s) link unstable" , __LINE__ , ref_parent_node->d_allocated_node_name , ref_node->d_allocated_node_name );
			}
		}
	}
	for ( const auto & ref_node : *d_pRuntime->d_ordered_nodes )
	{
		for ( const auto & ref_edge : ref_node->d_links )
		{
			bool b_parent_child_link_stable = false;
			for ( const auto & ref_parent_node : (ref_edge->d_pDestination)->d_parents )
			{
				if ( ref_parent_node->d_node_uniq_id == ref_node->d_node_uniq_id )
				{
					b_parent_child_link_stable = true;
					break;
				}
			}
			if ( !b_parent_child_link_stable )
			{
				XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible . parent(%s) child(%s) link unstable" , __LINE__ , ref_node->d_allocated_node_name , (ref_edge->d_pDestination)->d_allocated_node_name );
			}
		}
	}

	CATCH_TRY
}

double DAG::calculate_ccr()
{
	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	if ( d_pRuntime->d_shuffled_nodes == NULL )
	{
		make_shuffled_nodes();
	}
	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d)   %d" , __LINE__ , (int)d_nodes_count );
	// calc comp
	double sigma_computation = 0;
	double sigma_communication = 0;
	{
		map< eNodeID , eNodeID > test_node_duplicate;
		for ( auto & node : *d_pRuntime->d_ordered_nodes )
		{
			////if ( test_node_duplicate.count( node->d_node_uniq_id ) == 0 )
			////{
			////	test_node_duplicate[ node->d_node_uniq_id ] = node->d_node_uniq_id;
			////}
			////else
			////{
			////	assert( 0 );
			////}
			double avg_computation = 0;
			double avg_communication = 0;
			for ( auto & vm : VMachine::get_type_instances() )
			{
				avg_computation += vm.get_execution_cost( node->d_task_cpu_usage_flops );
			}
			for ( auto & vm : VMachine::get_type_instances() )
			{
				avg_communication += vm.get_cost_datatransfer_on_wan( node->d_task_size_byte );
			}
			sigma_computation += ( avg_computation / VMachine::get_type_instances().size() );
			sigma_communication += ( avg_communication / VMachine::get_type_instances().size() );
		}
	}
	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	// calc comm
	// hierarchical assignment
	map< int , DAGEdge * > edges;
	map< eNodeID , eNodeID > test_node_duplicate;
	for ( auto & node : *d_pRuntime->d_ordered_nodes )
	{
		if ( test_node_duplicate.count( node->d_node_uniq_id ) == 0 )
		{
			test_node_duplicate[ node->d_node_uniq_id ] = node->d_node_uniq_id;
		}
		else
		{
			assert( 0 );
		}
		for ( auto & link : node->d_links )
		{
			if ( edges.count( link->d_edge_id ) == 0 )
			{
				edges[ link->d_edge_id ] = link;
			}
			else
			{
				assert( false ); // impossible
			}
		}
	}
	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	for ( auto & edge : edges )
	{
		double avg_comm = 0;
		for ( auto & vm : VMachine::get_type_instances() )
		{
			avg_comm += vm.get_cost_datatransfer_on_wan( *edge.second->d_p_network_usage_byte );
		}
		sigma_communication += ( avg_comm / VMachine::get_type_instances().size() );
	}
	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	assert( sigma_computation > 0 );
	//assert( sigma_communication > 0 );
	return sigma_communication / sigma_computation;
}

////map< int /*edge id*/ , DAGEdge * > DAG::get_all_edges()
////{
////	if ( d_shuffled_nodes == NULL )
////	{
////		make_shuffled_nodes();
////	}
////	map< int /*edge id*/ , DAGEdge * > edges;
////	for ( auto & node : *d_ordered_nodes )
////	{
////		for ( auto & edge : node->d_links )
////		{
////			if ( edges.count( edge->d_edge_id ) == 0 )
////			{
////				edges[ edge->d_edge_id ] = edge;
////			}
////		}
////	}
////	return edges;
////}

void DAG::reset_values_with_ccr( bool b_append_ccr_into_name )
{
	BEGIN_TRY
	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) reset_values_with_ccr %s " , __LINE__ , this->get_name().c_str() );

	if ( d_pRuntime->d_shuffled_nodes == NULL )
	{
		make_shuffled_nodes();
	}
	if ( b_append_ccr_into_name )
	{
		d_pRuntime->d_dag_name += string_format( "_ccr(%s)" , trim_floatnumber( d_ccr ).c_str() );
	}

	for ( const auto & ref_node : *d_pRuntime->d_shuffled_nodes )
	{
		for ( auto & ref_edge : ref_node.second->d_links )
		{
			ref_edge->d_p_network_usage_byte = &ref_node.second->d_task_output_size_byte;
		}
	}

	////VMachine least_vm = VMachine::()[ d_least_price_vmp_type ];
	////int edge_count = get_all_edges().size();

	bool brevalue = false;
	revalue:
	assert( d_pRuntime->d_ordered_nodes != NULL );
	//assert( this->d_least_price_vmp_type >= (eVM_Type)0 && d_least_price_vmp_type <= (eVM_Type)4 );
	// init for the first time
	for ( auto & ref_node : *d_pRuntime->d_ordered_nodes )
	{
		switch ( SETTINGS_FAST()->d_reset_values_type )
		{
			case 2: // low fat
			{
				ref_node->d_task_cpu_usage_flops = simgrid::xbt::random::uniform_real( CPU_SPEED_POSTFIX( LOW_FAT_DAG_MIN_VALUE ) /*/ pow( 10 , 4 - this->d_least_price_vmp_type )*/ , CPU_SPEED_POSTFIX( LOW_FAT_DAG_MAX_VALUE )  );
				ref_node->d_task_size_byte = simgrid::xbt::random::uniform_real( WAN_NETWORK_ACCESS_SPEED_POSTFIX( LOW_FAT_DAG_MIN_VALUE )  , WAN_NETWORK_ACCESS_SPEED_POSTFIX( LOW_FAT_DAG_MAX_VALUE )  );
				ref_node->d_task_output_size_byte = simgrid::xbt::random::uniform_real( WAN_NETWORK_ACCESS_SPEED_POSTFIX( LOW_FAT_DAG_MIN_VALUE )  , WAN_NETWORK_ACCESS_SPEED_POSTFIX( LOW_FAT_DAG_MAX_VALUE )  );
				break;
			}
			case 4: // hi fat
			{
				_ONE_TIME_VAR();
				_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
				ref_node->d_task_cpu_usage_flops = simgrid::xbt::random::uniform_real( CPU_SPEED_POSTFIX( HI_FAT_DAG_MIN_VALUE ) , CPU_SPEED_POSTFIX( HI_FAT_DAG_MAX_VALUE )  );
				ref_node->d_task_size_byte = simgrid::xbt::random::uniform_real( WAN_NETWORK_ACCESS_SPEED_POSTFIX( HI_FAT_DAG_MIN_VALUE )  , WAN_NETWORK_ACCESS_SPEED_POSTFIX( HI_FAT_DAG_MAX_VALUE )  );
				ref_node->d_task_output_size_byte = simgrid::xbt::random::uniform_real( WAN_NETWORK_ACCESS_SPEED_POSTFIX( HI_FAT_DAG_MIN_VALUE )  , WAN_NETWORK_ACCESS_SPEED_POSTFIX( HI_FAT_DAG_MAX_VALUE )  );
				break;
			}
			case 3:
			{
				_ONE_TIME_VAR();
				_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
				ref_node->d_task_cpu_usage_flops = CPU_SPEED_POSTFIX( 1 );
				ref_node->d_task_size_byte = WAN_NETWORK_ACCESS_SPEED_POSTFIX( 1 );
				ref_node->d_task_output_size_byte = WAN_NETWORK_ACCESS_SPEED_POSTFIX( 1 );
				break;
			}
			default:
			{
				_ONE_TIME_VAR();
				_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
			}
		}
	}
	if ( SETTINGS_FAST()->d_reset_values_type == 3 )
	{
		return;
	}

	if ( brevalue == false )
	{
		//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_high , "#(%d) ccr(%f) , pre ccr(%f)" , __LINE__ , d_ccr , calculate_ccr() );
	}
	int timeout_counter = 0;
	////static int _timeout_counter = 0;
	////bool b_operator_on = simgrid::xbt::random::uniform_int( 0 , 1 );
	int timeout_counter_max = simgrid::xbt::random::uniform_int( 100 , 1000 );
	// infinite loop
	do
	{
		timeout_counter++;
		////_timeout_counter++;
		double calced_ccr = calculate_ccr();
		////if ( _timeout_counter % timeout_counter_max == 0 )
		////{
		////	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_high , "#(%d) ccr(%f) , pre ccr(%f) , timeout_counter(%d) , b_operator_on(%d) , timeout_counter_max(%d)" , __LINE__ , d_ccr , calced_ccr , timeout_counter , b_operator_on , timeout_counter_max );
		////}
		if ( calced_ccr <= 0.0 || timeout_counter > timeout_counter_max )
		{
			timeout_counter = 0;
			brevalue = true;
			//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_high , "#(%d) ccr(%f) , pre ccr(%f) , timeout_counter(%d)" , __LINE__ , d_ccr , calced_ccr , timeout_counter );
			goto revalue;
		}
		if ( fabs( calced_ccr - d_ccr ) < ( d_ccr / 10.0 ) )
		{
			break;
		}
		double inc_dec_percent = simgrid::xbt::random::uniform_real( 0.0001 , 0.2 );
		for ( auto & node : *d_pRuntime->d_ordered_nodes )
		{
			////if ( b_operator_on )
			{
				if ( d_ccr > calced_ccr )
				{
					node->d_task_output_size_byte *= ( 1 + inc_dec_percent );
					node->d_task_size_byte *= ( 1 + inc_dec_percent );
					node->d_task_cpu_usage_flops *= ( 1 - inc_dec_percent );
				}
				else
				{
					node->d_task_output_size_byte *= ( 1 - inc_dec_percent );
					node->d_task_size_byte *= ( 1 - inc_dec_percent );
					node->d_task_cpu_usage_flops *= ( 1 + inc_dec_percent );
				}
				node->d_task_output_size_byte = max( node->d_task_output_size_byte , 1.0 );
				node->d_task_size_byte = max( node->d_task_size_byte , 1.0 );
				node->d_task_cpu_usage_flops = max( node->d_task_cpu_usage_flops , 1.0 );
			}
			////else
			////{
			////	if ( d_ccr > calced_ccr )
			////	{
			////		node->d_task_cpu_usage_flops *= ( 1 - inc_dec_percent );
			////	}
			////	else
			////	{
			////		node->d_task_cpu_usage_flops *= ( 1 + inc_dec_percent );
			////	}
			////}
		}
	} while( 1 );

	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_high , "#(%d) ccr(%f) , post correct ccr(%f)" , __LINE__ , d_ccr , calculate_ccr() );
	CATCH_TRY
}

string DAG::get_tag( string name )
{
	for ( auto & row : this->d_pRuntime->d_dagtags )
	{
		if ( row.name == name )
		{
			return row.value;
		}
	}
	assert( 0 );
	return "";
}

string DAG::get_name()
{
	return SETTINGS_FAST()->d_dump_dag_with_long_name ? d_pRuntime->d_dag_name : d_pRuntime->d_dag_short_name;
}

void DAG::clear_unnecessary_data_after_dag_complete()
{
	////CBenchmark::d_current_benchmark->vmm->release_dag_data( this );

	if ( d_pRuntime != NULL )
	{
		__DELETE( d_pRuntime );
		d_pRuntime = NULL;
	}
}

#pragma region make dag type

eDAG_ID DAG::get_uniq_id()
{
	static int _DAGID = eDAG_ID::first_dag_id;
	return ( eDAG_ID )++_DAGID;
}

#pragma endregion

#pragma endregion

#pragma region DAX1Dags

DAX1Node::DAX1Node( int id )
{
	d_daxnode_nodeid = id;
}
DAX1Node::~DAX1Node()
{
	cleanup();
}
void DAX1Node::cleanup()
{
	for ( auto & link : d_links )
	{
		if ( link != NULL )
		{
			__DELETE( link );
			link = NULL;
		}
	}
	d_links.clear();
}

DAX1Dag::DAX1Dag()
{ }
DAX1Dag::~DAX1Dag()
{
	cleanup();
}
void DAX1Dag::cleanup()
{
	for ( auto & node : d_nodes )
	{
		if ( node.second != NULL )
		{
			node.second->cleanup();
			__DELETE( node.second );
			node.second = NULL;
		}
	}
	d_nodes.clear();
	d_roots.clear();
	d_daxtags.clear();
}

DAX1Dags::DAX1Dags()
{ }
DAX1Dags::~DAX1Dags()
{
	cleanup();
}
void DAX1Dags::cleanup()
{
	for ( auto & dag : d_dax_dags )
	{
		if ( dag != NULL )
		{
			dag->cleanup();
			__DELETE( dag );
			dag = NULL;
		}
	}
	d_dax_dags.clear();
}

void DAX1Dags::read_dax1_from_file()
{
	BEGIN_TRY
	cleanup();
	string line_token;
	DAX1Dag * new_dax_dag = NULL;
	cout << "#" << __LINE__ << " " << ")load dax1 file " << SETTINGS_FAST()->d_dax1_file_path << endl;
	std::ifstream file( SETTINGS_FAST()->d_dax1_file_path );
	int dax_dag_count = 0;
	while ( std::getline( file , line_token , '\n' ) )
	{
		stringstream line_token_stream( trim_copy( line_token ) );
		string token;
		while ( std::getline( line_token_stream , token , ']' ) && ( token = trim_copy( token ) ).length() > 0 )
		{
			if ( token.rfind( "[dags" , 0 ) == 0 )
			{
				try_read_token( line_token_stream , "[dagcount" , token );
				std::getline( line_token_stream , token , ']' ); // number
				dax_dag_count = atoi( ReplaceAll( token , "[" , "" ).c_str() );

				try_read_token( line_token_stream , "[pname" , token );
				std::getline( line_token_stream , token , ']' ); // string
				pstorage->d_pname = ReplaceAll( token , "[" , "" ).c_str();

				try_read_token( line_token_stream , "[DAX_VER" , token );
				std::getline( line_token_stream , token , ']' ); // ver
				d_daxver = ReplaceAll( token , "[" , "" );
				assert( d_daxver == "standard1.0" );
				continue;
			}
			if ( token.rfind( "[dag" , 0 ) == 0 )
			{
				new_dax_dag = __NEW( DAX1Dag() );
				
				try_read_token( line_token_stream , "[daxid" , token );
				std::getline( line_token_stream , token , ']' ); // number
				new_dax_dag->d_daxdag_dagid = atoi( ReplaceAll( token , "[" , "" ).c_str() );

				try_read_token( line_token_stream , "[dagtype" , token );
				std::getline( line_token_stream , token , ']' ); // number
				////new_dax_dag->d_daxdag_dagtype = atoi( ReplaceAll( token , "[" , "" ).c_str() );

				try_read_token( line_token_stream , "[dagname" , token );
				std::getline( line_token_stream , token , ']' ); // name
				new_dax_dag->d_daxdag_name = ReplaceAll( token , "[" , "" );

				try_read_token( line_token_stream , "[~dagname" , token );
				continue;
			}
			if ( token.rfind( "[tags" , 0 ) == 0 )
			{
				std::getline( line_token_stream , token , ']' );
				while ( token != "[~tags" )
				{
					string name_value = ReplaceAll( token , "[" , "" );
					stringstream name_value_stream( trim_copy( name_value ) );
					string tag_name;
					std::getline( name_value_stream , tag_name , '=' );
					string tag_value;
					std::getline( name_value_stream , tag_value , '=' );
					new_dax_dag->d_daxtags.push_back( DAGTag( tag_name , tag_value ) );
					std::getline( line_token_stream , token , ']' );
				}
				continue;
			}
			if ( token.rfind( "[node" , 0 ) == 0 )
			{
				try_read_token( line_token_stream , "[nodecount" , token );
				std::getline( line_token_stream , token , ']' ); // number
				int dag_node_count = atoi( ReplaceAll( token , "[" , "" ).c_str() );
				while ( try_read_token( line_token_stream , "[nodeid" , token , ']' , false ) )
				{
					std::getline( line_token_stream , token , ']' ); // number
					DAX1Node * src_node = __NEW( DAX1Node( atoi( ReplaceAll( token , "[" , "" ).c_str() ) ) );

					src_node->d_owner = new_dax_dag;
					new_dax_dag->d_nodes[ src_node->d_daxnode_nodeid ] = src_node;
				}
				assert( dag_node_count == new_dax_dag->d_nodes.size() );
				if ( token != "[~nodecount" )
				{
					cout << "assert" << endl;
					assert( 0 );
				}
				try_read_token( line_token_stream , "[~node" , token );
				continue;
			}
			if ( token.rfind( "[parents" , 0 ) == 0 )
			{
				while ( try_read_token( line_token_stream , "[childnodeid" , token , ']' , false ) )
				{
					std::getline( line_token_stream , token , ']' ); // number
					int child_node_id = atoi( ReplaceAll( token , "[" , "" ).c_str() );

					try_read_token( line_token_stream , "[parentnodeid" , token );

					std::getline( line_token_stream , token , ']' ); // number
					int parent_node_id = atoi( ReplaceAll( token , "[" , "" ).c_str() );

					new_dax_dag->d_nodes[ child_node_id ]->d_parents[ parent_node_id ] = new_dax_dag->d_nodes[ parent_node_id ];
					new_dax_dag->d_nodes[ parent_node_id ]->d_links.push_back( __NEW( DAX1Edge(
																				   new_dax_dag->d_nodes[ parent_node_id ] ,
																				   new_dax_dag->d_nodes[ child_node_id ]
																			   ) ) );
				}
				if ( token != "[~parents" )
				{
					cout << "assert" << endl;
					assert( 0 );
				}
				continue;
			}
			if ( token.rfind( "[rootcount" , 0 ) == 0 )
			{
				std::getline( line_token_stream , token , ']' ); // number
				int root_count = atoi( ReplaceAll( token , "[" , "" ).c_str() );

				while ( try_read_token( line_token_stream , "[noderootid" , token , ']' , false ) )
				{
					std::getline( line_token_stream , token , ']' ); // number
					int root_id = atoi( ReplaceAll( token , "[" , "" ).c_str() );

					new_dax_dag->d_roots[ root_id ] = new_dax_dag->d_nodes[ root_id ];
				}
				if ( token != "[~rootcount" )
				{
					cout << "assert" << endl;
					assert( 0 );
				}
				if ( try_read_token( line_token_stream , "[~dag" , token , ']' , true ) )
				{
					// TODO . comment this
					//if ( new_dax_dag->d_daxdag_dagid < 1640 )
					//{
					//	__DELETE( new_dax_dag );
					//}
					//else
					{

						d_dax_dags.push_back( new_dax_dag );
					}
				}
				new_dax_dag = NULL;
				continue;
			}
		}
	}
	cout << "#" << __LINE__ << " " << "finish loading dax1 " << d_dax_dags.size() << endl;
	assert( dax_dag_count == d_dax_dags.size() ); // uncomment this
	CATCH_TRY
}

void DAX1Dags::read_dax2_from_file()
{
	BEGIN_TRY
	cleanup();
	string line_token;
	DAX1Dag * new_dax_dag = NULL;
	cout << "(#" << __LINE__ << ")load dax2 file " << SETTINGS_FAST()->d_dax2_file_path << endl;
	std::ifstream file( SETTINGS_FAST()->d_dax2_file_path );
	int dax_dag_count = 0;
	while ( std::getline( file , line_token , '\n' ) )
	{
		stringstream line_token_stream( trim_copy( line_token ) );
		string token;
		while ( std::getline( line_token_stream , token , ']' ) && ( token = trim_copy( token ) ).length() > 0 )
		{
			if ( token.rfind( "[dags" , 0 ) == 0 )
			{
				try_read_token( line_token_stream , "[dagcount" , token );
				std::getline( line_token_stream , token , ']' ); // number
				dax_dag_count = atoi( ReplaceAll( token , "[" , "" ).c_str() );

				try_read_token( line_token_stream , "[pname" , token );
				std::getline( line_token_stream , token , ']' ); // string
				pstorage->d_pname = ReplaceAll( token , "[" , "" ).c_str();

				try_read_token( line_token_stream , "[DAX_VER" , token );
				std::getline( line_token_stream , token , ']' ); // ver
				d_daxver = ReplaceAll( token , "[" , "" );
				assert( d_daxver == "standard2.0" );

				continue;
			}
			if ( token.rfind( "[dag" , 0 ) == 0 )
			{
				new_dax_dag = __NEW( DAX1Dag() );
				
				try_read_token( line_token_stream , "[daxid" , token );
				std::getline( line_token_stream , token , ']' ); // number
				new_dax_dag->d_daxdag_dagid = atoi( ReplaceAll( token , "[" , "" ).c_str() );

				try_read_token( line_token_stream , "[dagtype" , token );
				std::getline( line_token_stream , token , ']' ); // number
				////new_dax_dag->d_daxdag_dagtype = atoi( ReplaceAll( token , "[" , "" ).c_str() );

				try_read_token( line_token_stream , "[dagname" , token );
				std::getline( line_token_stream , token , ']' ); // name
				new_dax_dag->d_daxdag_name = ReplaceAll( token , "[" , "" );

				try_read_token( line_token_stream , "[~dagname" , token );
				continue;
			}
			if ( token.rfind( "[tags" , 0 ) == 0 )
			{
				std::getline( line_token_stream , token , ']' );
				while ( token != "[~tags" )
				{
					string name_value = ReplaceAll( token , "[" , "" );

					stringstream name_value_stream( trim_copy( name_value ) );
					string tag_name;
					std::getline( name_value_stream , tag_name , '=' );
					string tag_value;
					std::getline( name_value_stream , tag_value , '=' );

					new_dax_dag->d_daxtags.push_back( DAGTag( tag_name , tag_value ) );

					std::getline( line_token_stream , token , ']' );
				}
				continue;
			}
			if ( token.rfind( "[node" , 0 ) == 0 )
			{
				try_read_token( line_token_stream , "[nodecount" , token );

				std::getline( line_token_stream , token , ']' ); // number
				int dag_node_count = atoi( ReplaceAll( token , "[" , "" ).c_str() );

				while ( try_read_token( line_token_stream , "[nodeid" , token , ']' , false ) )
				{
					std::getline( line_token_stream , token , ']' ); // number
					DAX1Node * src_node = __NEW( DAX1Node( atoi( ReplaceAll( token , "[" , "" ).c_str() ) ) );

					src_node->d_owner = new_dax_dag;

					try_read_token( line_token_stream , "[task_cpu_usage_flops" , token );
					std::getline( line_token_stream , token , ']' ); // number
					src_node->d_task_cpu_usage_flops = atof( ReplaceAll( token , "[" , "" ).c_str() );

					try_read_token( line_token_stream , "[task_size_byte" , token );
					std::getline( line_token_stream , token , ']' ); // number
					src_node->d_task_size_byte = atof( ReplaceAll( token , "[" , "" ).c_str() );

					try_read_token( line_token_stream , "[task_output_size_byte" , token );
					std::getline( line_token_stream , token , ']' ); // number
					src_node->d_task_output_size_byte = atof( ReplaceAll( token , "[" , "" ).c_str() );

					new_dax_dag->d_nodes[ src_node->d_daxnode_nodeid ] = src_node;
				}
				assert( dag_node_count == new_dax_dag->d_nodes.size() );
				if ( token != "[~nodecount" )
				{
					cout << "assert" << endl;
					assert( 0 );
				}
				try_read_token( line_token_stream , "[~node" , token );
				continue;
			}
			if ( token.rfind( "[parents" , 0 ) == 0 )
			{
				while ( try_read_token( line_token_stream , "[childnodeid" , token , ']' , false ) )
				{
					std::getline( line_token_stream , token , ']' ); // number
					int child_node_id = atoi( ReplaceAll( token , "[" , "" ).c_str() );

					try_read_token( line_token_stream , "[parentnodeid" , token );

					std::getline( line_token_stream , token , ']' ); // number
					int parent_node_id = atoi( ReplaceAll( token , "[" , "" ).c_str() );

					new_dax_dag->d_nodes[ child_node_id ]->d_parents[ parent_node_id ] = new_dax_dag->d_nodes[ parent_node_id ];
					new_dax_dag->d_nodes[ parent_node_id ]->d_links.push_back( __NEW( DAX1Edge(
																					   new_dax_dag->d_nodes[ parent_node_id ] ,
																					   new_dax_dag->d_nodes[ child_node_id ]
																				   ) ) );
				}
				if ( token != "[~parents" )
				{
					cout << "assert" << endl;
					assert( 0 );
				}
				continue;
			}
			if ( token.rfind( "[rootcount" , 0 ) == 0 )
			{
				std::getline( line_token_stream , token , ']' ); // number
				int root_count = atoi( ReplaceAll( token , "[" , "" ).c_str() );

				while ( try_read_token( line_token_stream , "[noderootid" , token , ']' , false ) )
				{
					std::getline( line_token_stream , token , ']' ); // number
					int root_id = atoi( ReplaceAll( token , "[" , "" ).c_str() );

					new_dax_dag->d_roots[ root_id ] = new_dax_dag->d_nodes[ root_id ];
				}
				if ( token != "[~rootcount" )
				{
					cout << "assert" << endl;
					assert( 0 );
				}
				if ( try_read_token( line_token_stream , "[~dag" , token , ']' , true ) )
				{
					d_dax_dags.push_back( new_dax_dag );
				}
				new_dax_dag = NULL;
				continue;
			}
		}
	}
	cout << "#" << __LINE__ << " " << "finish loading dax2 " << d_dax_dags.size() << endl;
	assert( dax_dag_count == d_dax_dags.size() ); // uncomment this
	CATCH_TRY
}

DAGstorage::DAGstorage()
{
	d_DAXdags.pstorage = this;
}
DAGstorage::~DAGstorage()
{
	cleanup();
}

void DAGstorage::cleanup()
{
	for ( auto & dag : d_dags )
	{
		__DELETE( dag );
	}
	d_dags.clear();
	d_DAXdags.cleanup();
}

void DAGstorage::read_and_convert_dax1_to_dags() // set ccr
{
	BEGIN_TRY

	////init_xml_dag_guid_config_to_write(); // به تابع بیرون منتقل شد

	map< int /*vm type*/ , int > _vm_type_dag_quota;

	d_DAXdags.read_dax1_from_file();

	XBT_INFO( "#(%d)" , __LINE__  );

	assert( d_DAXdags.d_daxver == "standard1.0" );

	XBT_INFO( "#(%d) pname(%s)" , __LINE__ , this->d_pname.c_str() );

	SystemTimeKeepr time_keeper;
	for ( auto & daxdag : d_DAXdags.d_dax_dags )
	{
		//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_critical_diagnose , "#(%d) dag(%d) with nodes(%d) " ,
		//					   __LINE__ , daxdag->d_dax_id , (int)daxdag->d_nodes.size() );

		DAG * new_dag = __NEW( DAG() );
		new_dag->d_pRuntime = __NEW( DAG::runtime_neccessary() );
		{ // collapse able
			new_dag->d_dag_uniq_id = DAG::get_uniq_id();
			new_dag->d_dag_dax_id = daxdag->d_daxdag_dagid;
			////new_dag->d_dag_type.init_dag_config( ( eDAGType )daxdag->d_daxdag_dagtype );
			new_dag->d_dag_config.init_dag_config( rand() % SETTINGS()->d_max_ranje_DAG_Type );
			new_dag->d_pRuntime->d_dag_name = string_format( "dag_%s_daxid%d" , daxdag->d_daxdag_name.c_str() , daxdag->d_daxdag_dagid );
			new_dag->d_pRuntime->d_dag_name = ReplaceAll( new_dag->d_pRuntime->d_dag_name , "dag_dag_" , "dag_" );
			new_dag->d_pRuntime->d_dagtags = daxdag->d_daxtags;

			int node_index = 0;
			map< int , eNodeID > map_dax_real;

			vector< DAX1Node * > next_iterate_daxnodes;
			map< int , int > check_duplicate_and_added;
			for ( auto & daxroot : daxdag->d_roots )
			{
				next_iterate_daxnodes.push_back( daxroot.second );
				check_duplicate_and_added[ daxroot.second->d_daxnode_nodeid ] = daxroot.second->d_daxnode_nodeid;
			}
			// MINIMIZE THIS
			while ( next_iterate_daxnodes.size() )
			{
				//XBT_INFO_OVERRIDEABLE__( "#(%d) next_iterate_daxnodes.size(%d)" , __LINE__ , ( int )next_iterate_daxnodes.size() );

				vector< DAX1Node * > current_iterate_daxnodes( next_iterate_daxnodes );
				next_iterate_daxnodes.clear();
				// حواست به فرزند دو پدر باشه
				// و در ضمن نودی یالش ساخته بشه که پدر هاش حتما اضافه شده باشند

				////for ( auto & currentdaxnode : current_iterate_nodes )
				////{
				////	check_duplicate_and_added[ currentdaxnode->d_nodeid ] = currentdaxnode->d_nodeid; // اونایی که قرار است پروسس شود به لیست اضافه می شود
				////}
				for ( auto & currentdaxnode : current_iterate_daxnodes )
				{
					for ( auto & daxedge : currentdaxnode->d_links )
					{
						if ( check_duplicate_and_added.count( daxedge->d_dst->d_daxnode_nodeid ) == 0 ) // از دو پدر به یک فرزند رسیدیم اون فرزند یکبار بیاید
						{
							bool badd = true;
							for ( auto & np : daxedge->d_dst->d_parents ) // این چک میکنه وقتی فرزند برود در لیست که همه پدر هاش پروسس شده باشد
							{
								if ( check_duplicate_and_added.count( np.first ) == 0 )
								{
									//XBT_INFO_OVERRIDEABLE__( "#(%d) np.first(%d)" , __LINE__ , np.first );

									badd = false;
									break;
								}
							}
							if ( badd )
							{
								next_iterate_daxnodes.push_back( daxedge->d_dst );
								check_duplicate_and_added[ daxedge->d_dst->d_daxnode_nodeid ] = daxedge->d_dst->d_daxnode_nodeid;

								//XBT_INFO_OVERRIDEABLE__( "#(%d) next_iterate_daxnodes.size(%d)" , __LINE__ , ( int )next_iterate_daxnodes.size() );
							}
						}
					}
				}
				//XBT_INFO_OVERRIDEABLE__( "#(%d) next_iterate_daxnodes.size(%d)" , __LINE__ , ( int )next_iterate_daxnodes.size() );
				for ( auto & tempdaxnode : current_iterate_daxnodes )
				{
					if ( daxdag->d_roots.count( tempdaxnode->d_daxnode_nodeid ) ) // in root fale dag
					{
						DAGNode * new_node = __NEW( DAGNode( string_format( "dxid%d" , tempdaxnode->d_daxnode_nodeid ).c_str() ) );
						new_node->d_node_dax_id = tempdaxnode->d_daxnode_nodeid; // نود هم ای دی منحصر بفرد دارد و هم یک آیدی موقت از فایل دکس می گیرد
						map_dax_real[ tempdaxnode->d_daxnode_nodeid ] = new_node->d_node_uniq_id; // مپ بین نود فیک و نود اصلی
						new_node->d_pOwnerDAG = new_dag;
						new_dag->d_pRuntime->d_roots.push_back( new_node );

						if ( new_dag->d_pRuntime->d_shuffled_nodes == NULL ) // اولین بار خالیست زیرا دگ جدید است
						{
							new_dag->d_pRuntime->d_shuffled_nodes = __NEW( _map_nodes );
						}

						//assert( new_dag->d_shuffled_nodes != NULL );

						( *new_dag->d_pRuntime->d_shuffled_nodes )[ new_node->d_node_uniq_id ] = new_node;
					}
					else // not in root dax dag
					{
						DAGNode * new_node = __NEW( DAGNode( string_format( "dxid%d" , tempdaxnode->d_daxnode_nodeid ).c_str() ) );
						new_node->d_node_dax_id = tempdaxnode->d_daxnode_nodeid;
						map_dax_real[ tempdaxnode->d_daxnode_nodeid ] = new_node->d_node_uniq_id; // مپ بین نود فیک و نود اصلی
						new_node->d_pOwnerDAG = new_dag;

						//assert( new_dag->d_shuffled_nodes != NULL );
						( *new_dag->d_pRuntime->d_shuffled_nodes )[ new_node->d_node_uniq_id ] = new_node;

						for ( auto & daxnodeparent : tempdaxnode->d_parents )
						{
							//assert( daxnodeparent.first < 2000 && daxnodeparent.first >= 0 );
							//assert( map_dax_real.count( daxnodeparent.first ) );

							DAGEdge * new_edge = __NEW( DAGEdge(
								( *new_dag->d_pRuntime->d_shuffled_nodes )[ map_dax_real[ daxnodeparent.first ] ] , new_node ) );

							new_edge->init_edge();
						}
					}
				}
			}
		}
		
		new_dag->d_pRuntime->d_swap_shuffled_nodes = __NEW( _map_nodes );
		*new_dag->d_pRuntime->d_swap_shuffled_nodes = *new_dag->d_pRuntime->d_shuffled_nodes; // important . 14020828

		_GUID dag_guid = new_dag->get_tag( "GUID" );
		assert( dag_guid.length() > 0 );

		int loop_count = 0;
		bool bret = false;
		do
		{
			if ( new_dag->d_pRuntime->d_shuffled_nodes != NULL )
			{
				new_dag->d_pRuntime->d_shuffled_nodes->clear();
				__DELETE( new_dag->d_pRuntime->d_shuffled_nodes );
				new_dag->d_pRuntime->d_shuffled_nodes = NULL; // important . 14020828
			}
			if ( new_dag->d_pRuntime->d_ordered_nodes != NULL )
			{
				new_dag->d_pRuntime->d_ordered_nodes->clear();
				__DELETE( new_dag->d_pRuntime->d_ordered_nodes );
				new_dag->d_pRuntime->d_ordered_nodes = NULL; // important . 14020828
			}
			if ( loop_count > 0 )
			{
				XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) add new dag(%s) loop_count(%d)" , __LINE__ , new_dag->get_name().c_str() , loop_count );
			}
			loop_count++;
			//cout << dag_guid << "  " << new_dag->d_dag_dax_id << endl;
			if ( SETTINGS_FAST()->d_use_dag_guid_config_cache == false )
			{
				//cout << dag_guid << "  " << new_dag->d_dag_dax_id << endl;
				if ( get_xml_dag_guid_config( dag_guid ).size() < SETTINGS_FAST()->d_make_dag_n_guid_config )
				{
					double ccrs[] = { 0.01 , 0.1 , 0.5 , 1 , 2 , 10 , 100 };
					new_dag->d_ccr = ccrs[ simgrid::xbt::random::uniform_int( 1 , sizeof( ccrs ) / sizeof( ccrs[ 0 ] ) ) - 1 ];
					bool bfound = false;
					for ( auto & tag : new_dag->d_pRuntime->d_dagtags )
					{
						if ( tag.name == "ccr" )
						{
							tag.value = trim_floatnumber( new_dag->d_ccr );
							bfound = true;
							break;
						}
					}
					if ( bfound == false )
					{
						new_dag->d_pRuntime->d_dagtags.push_back( DAGTag( "ccr" , trim_floatnumber( new_dag->d_ccr ) ) );
					}

					//XBT_INFO_ALIVE_CHECK( "#(%d) %d %f " , __LINE__ , inner_loop_count , new_dag->d_ccr );

					new_dag->reset_values_with_ccr( false );

					if ( SETTINGS_FAST()->d_make_dag_guid_config )
					{
						DAG::maketime_data data;
						bret = new_dag->complete_dag_structure( &data );

						if ( bret )
						{
							//XBT_INFO( "#(%d) " , __LINE__ );
							//if ( get_xml_dag_guid_config( dag_guid ).size() < SETTINGS_FAST()->d_make_dag_n_guid_config )
							{
								/*XBT_INFO( "#(%d) found one dag(%d) guid config dead:%f paral:%d max:%d vmtype(%d)" , __LINE__ , new_dag->d_dag_dax_id ,
										  new_dag->d_dag_deadline.d_dag_requestor_deadline_duration , data.ExtraParallelVM , new_dag->d_MaximumPossibleParallelism , new_dag->d_least_price_vmp_type );*/

								XBT_INFO( "#(%d) found one dag(%d) guid config dead:%f vmtype(%d)" , __LINE__ , new_dag->d_dag_dax_id ,
										  new_dag->d_dag_deadline.d_dag_requestor_deadline_duration , new_dag->d_least_price_vmp_type );

								map< int/*nodedaxid*/ , DurationTime /*each node time duration*/> quotalist;
								for ( auto & dur : data.node_exec_time_duration )
								{
									quotalist[ ( *new_dag->d_pRuntime->d_shuffled_nodes )[ dur.first ]->d_node_dax_id ] = dur.second;
								}

								t_map_nodes_vars node_vars;
								t_map_node_katz node_katz;
								for ( auto & node : *new_dag->d_pRuntime->d_ordered_nodes )
								{
									node_vars[ node->d_node_dax_id ] = make_tuple( node->d_task_cpu_usage_flops , node->d_task_size_byte , node->d_task_output_size_byte );
									node_katz[ node->d_node_dax_id ] = node->d_Katz_value;
								}
								t_map_nodes_duration node_durations;
								for ( auto & row : data.node_exec_time_duration )
								{
									node_durations[ ( *new_dag->d_pRuntime->d_shuffled_nodes )[ row.first ]->d_node_dax_id ] = row.second;
								}
								assert( new_dag->d_dag_deadline.d_dag_requestor_deadline_duration > 0 );
								//assert( new_dag->d_MaximumPossibleParallelism > 0 );
								tuple_handler handler = make_tuple( data.base_least_price_vm , new_dag->d_dag_deadline.d_dag_requestor_deadline_duration ,
																	data.TotalTimeConsume , -1/*data.ExtraParallelVM*/ , -1/*new_dag->d_MaximumPossibleParallelism*/ , node_durations);
								tuple_dag_configure dag_configure = make_tuple( get_time() , node_vars , handler , node_katz , new_dag->d_ccr );
								add_dag_guid_config( dag_guid , dag_configure , true );

								if ( get_xml_dag_guid_config( dag_guid ).size() < SETTINGS_FAST()->d_make_dag_n_guid_config )
								{
									bret = false;
									loop_count = 0;
								}
							}
						}
					}
				}
				else
				{
					//bret = true; // comment this
				}
			}
			else // use cache
			{
				auto list_config = get_xml_dag_guid_config( dag_guid );
				std::random_device rd;
				std::mt19937 generator( rd() );
				std::shuffle( list_config.begin() , list_config.end() , generator );
				auto choosen_configuration = list_config.front();

				new_dag->d_ccr = std::get<4>( choosen_configuration );

				if ( new_dag->d_pRuntime->d_shuffled_nodes == NULL )
				{
					new_dag->make_shuffled_nodes();
				}

				//DAG::maketime_data data;

				for ( auto & node : *new_dag->d_pRuntime->d_ordered_nodes )
				{
					node->d_task_cpu_usage_flops = std::get<0>( std::get<1>( choosen_configuration )[ node->d_node_dax_id ] );
					node->d_task_size_byte = std::get<1>( std::get<1>( choosen_configuration )[ node->d_node_dax_id ] );
					node->d_task_output_size_byte = std::get<2>( std::get<1>( choosen_configuration )[ node->d_node_dax_id ] );
					//node->d_Katz_value = std::get<3>( choosen_configuration )[ node->d_node_dax_id ];
					//data.node_exec_time_duration[ node->d_node_uniq_id ] = std::get<5>( std::get<2>( choosen_configuration ) )[ node->d_node_dax_id ];
				}

				//data.TotalTimeConsume =	std::get<2>( std::get<2>( choosen_configuration ) );
				//data.base_least_price_vm = std::get<0>( std::get<2>( choosen_configuration ) );
				//data.p_least_price_vm = &data.base_least_price_vm;
				//data.ExtraParallelVM = std::get<3>( std::get<2>( choosen_configuration ) );
				//new_dag->d_dag_deadline.d_dag_requestor_deadline_duration = std::get<1>( std::get<2>( choosen_configuration ) );
				//new_dag->d_MaximumPossibleParallelism = std::get<4>( std::get<2>( choosen_configuration ) );

				bret = new_dag->complete_dag_structure( NULL , &choosen_configuration );
			}
			//XBT_INFO_ALIVE_CHECK( "#(%d)" , __LINE__ );

			if ( bret )
			{
				_vm_type_dag_quota[ new_dag->d_least_price_vmp_type ] += 1;
			}


		} while ( bret == false && loop_count < 100 );
		
		__DELETE( new_dag->d_pRuntime->d_swap_shuffled_nodes );
		new_dag->d_pRuntime->d_swap_shuffled_nodes = NULL;

		if ( bret )
		{
			//__DELETE( new_dag ); // comment this
			d_dags.push_back( new_dag ); // uncomment this
		}
		else
		{
			XBT_INFO( "#(%d) error " , __LINE__ );
		}
		if ( time_keeper.peek_time() )
		{
			//XBT_INFO_ALIVE_CHECK( "#(%d) dags(%d) (#0-%d #1-%d #2-%d #3-%d)" , __LINE__ , ( int )d_dags.size() ,
			//					  _vm_type_dag_quota[0].used_count , _vm_type_dag_quota[1].used_count , _vm_type_dag_quota[2].used_count , _vm_type_dag_quota[3].used_count );
			XBT_INFO_ALIVE_CHECK( "#(%d) dags(%d) (#0-%d #1-%d #2-%d #3-%d)" , __LINE__ , ( int )d_dags.size() ,
								  _vm_type_dag_quota[0] , _vm_type_dag_quota[1] , _vm_type_dag_quota[2] , _vm_type_dag_quota[3] );

			//XBT_INFO_ALIVE_CHECK( "#(%d) dags(%d) " , __LINE__ , ( int )d_dags.size() );
		}
		if ( SETTINGS()->d_order_stop_loop_dag_storage_add )
		{
			return;
		}
	}

	////XBT_INFO( "least vm type for dag statistic" );
	////for ( auto & yy : _vm_type_dag_quota )
	////{
	////	XBT_INFO( "(%d)(%d)(%d)" , yy.first , yy.second.quota , yy.second.used_count );
	////}

	XBT_INFO( "#(%d) ver(%d) reset value" , __LINE__ , SETTINGS()->d_reset_values_type );

	////finish_xml_dag_guid_config_for_write();

	assert( d_dags.size() == d_DAXdags.d_dax_dags.size() ); // uncomment this
	d_DAXdags.cleanup();
	CATCH_TRY
}

void DAGstorage::read_and_convert_dax2_to_dags()
{
	BEGIN_TRY
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	
	{
		if ( SETTINGS_FAST()->d_use_dag_guid_config_cache )
		{
			//assert( 0 );
			SECTION_ALIVE_CHECK( "" );
			read_xml_dag_guid_config( false ); // uncomment this
		}
	}
	SECTION_ALIVE_CHECK("");

	cleanup();
	d_DAXdags.read_dax2_from_file();
	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	assert( d_DAXdags.d_daxver == "standard2.0" );

	map< int /*least vm price*/ , int /*count*/ > vm_price_count;
	map< int /*node count*/ , int /*count*/ > dag_node_count_each;

	SystemTimeKeepr time_keeper;
	for ( auto & daxdag : d_DAXdags.d_dax_dags )
	{
		DAG * new_dag = __NEW( DAG() );
		new_dag->d_pRuntime = __NEW( DAG::runtime_neccessary() );

		new_dag->d_dag_uniq_id = DAG::get_uniq_id();
		new_dag->d_dag_dax_id = daxdag->d_daxdag_dagid;
		new_dag->d_dag_config.init_dag_config( rand() % SETTINGS()->d_max_ranje_DAG_Type );
		new_dag->d_pRuntime->d_dag_name = string_format( "dag_%s_daxid%d" , daxdag->d_daxdag_name.c_str() , new_dag->d_dag_dax_id );
		new_dag->d_pRuntime->d_dag_name = ReplaceAll( new_dag->d_pRuntime->d_dag_name , "dag_dag_" , "dag_" );
		new_dag->d_pRuntime->d_dagtags = daxdag->d_daxtags;
		for ( auto & tag : new_dag->d_pRuntime->d_dagtags )
		{
			if ( tag.name == "ccr" )
			{
				new_dag->d_ccr = atof( tag.value.c_str() );
			}
		}

		//SECTION_ALIVE_CHECK(new_dag->get_name());

		int node_index = 0;
		map< int , eNodeID > map_dax_real;

		vector< DAX1Node * > next_iterate_daxnodes;
		map< int , int > check_duplicate_and_added;
		for ( auto & daxroot : daxdag->d_roots )
		{
			next_iterate_daxnodes.push_back( daxroot.second );
			check_duplicate_and_added[ daxroot.second->d_daxnode_nodeid ] = daxroot.second->d_daxnode_nodeid;
		}
		while ( next_iterate_daxnodes.size() )
		{ // collape able
			vector< DAX1Node * > current_iterate_daxnodes( next_iterate_daxnodes );
			next_iterate_daxnodes.clear();
			// حواست به فرزند دو پدر باشه
			// و در ضمن نودی یالش ساخته بشه که پدر هاش حتما اضافه شده باشند

			////for ( auto & currentdaxnode : current_iterate_nodes )
			////{
			////	check_duplicate_and_added[ currentdaxnode->d_nodeid ] = currentdaxnode->d_nodeid; // اونایی که قرار است پروسس شود به لیست اضافه می شود
			////}
			for ( auto & currentdaxnode : current_iterate_daxnodes )
			{
				for ( auto & daxedge : currentdaxnode->d_links )
				{
					if ( check_duplicate_and_added.count( daxedge->d_dst->d_daxnode_nodeid ) == 0 ) // از دو پدر به یک فرزند رسیدیم اون فرزند یکبار بیاید
					{
						bool badd = true;
						for ( auto & np : daxedge->d_dst->d_parents ) // این چک میکنه وقتی فرزند برود در لیست که همه پدر هاش پروسس شده باشد
						{
							if ( check_duplicate_and_added.count( np.first ) == 0 )
							{
								badd = false;
								break;
							}
						}
						if ( badd )
						{
							next_iterate_daxnodes.push_back( daxedge->d_dst );
							check_duplicate_and_added[ daxedge->d_dst->d_daxnode_nodeid ] = daxedge->d_dst->d_daxnode_nodeid;
						}
					}
				}
			}
			for ( auto & tempdaxnode : current_iterate_daxnodes )
			{
				if ( daxdag->d_roots.count( tempdaxnode->d_daxnode_nodeid ) ) // in root fale dag
				{
					DAGNode * new_node = __NEW( DAGNode( string_format( "dxid%d" , tempdaxnode->d_daxnode_nodeid ).c_str() ) );
					new_node->d_node_dax_id = tempdaxnode->d_daxnode_nodeid; // نود هم ای دی منحصر بفرد دارد و هم یک آیدی موقت از فایل دکس می گیرد
					map_dax_real[ tempdaxnode->d_daxnode_nodeid ] = new_node->d_node_uniq_id; // مپ بین نود فیک و نود اصلی
					new_node->d_pOwnerDAG = new_dag;
					
					new_node->d_task_cpu_usage_flops = tempdaxnode->d_task_cpu_usage_flops;
					new_node->d_task_size_byte = tempdaxnode->d_task_size_byte;
					new_node->d_task_output_size_byte = tempdaxnode->d_task_output_size_byte;

					new_dag->d_pRuntime->d_roots.push_back( new_node );

					if ( new_dag->d_pRuntime->d_shuffled_nodes == NULL ) // اولین بار خالیست زیرا دگ جدید است
					{
						new_dag->d_pRuntime->d_shuffled_nodes = __NEW( _map_nodes );
					}

					( *new_dag->d_pRuntime->d_shuffled_nodes )[ new_node->d_node_uniq_id ] = new_node;
				}
				else // not in root dax dag
				{
					DAGNode * new_node = __NEW( DAGNode( string_format( "dxid%d" , tempdaxnode->d_daxnode_nodeid ).c_str() ) );
					new_node->d_node_dax_id = tempdaxnode->d_daxnode_nodeid;
					map_dax_real[ tempdaxnode->d_daxnode_nodeid ] = new_node->d_node_uniq_id; // مپ بین نود فیک و نود اصلی
					new_node->d_pOwnerDAG = new_dag;

					new_node->d_task_cpu_usage_flops = tempdaxnode->d_task_cpu_usage_flops;
					new_node->d_task_size_byte = tempdaxnode->d_task_size_byte;
					new_node->d_task_output_size_byte = tempdaxnode->d_task_output_size_byte;

					( *new_dag->d_pRuntime->d_shuffled_nodes )[ new_node->d_node_uniq_id ] = new_node;

					for ( auto & daxnodeparent : tempdaxnode->d_parents )
					{
						DAGEdge * new_edge = __NEW( DAGEdge(
							( *new_dag->d_pRuntime->d_shuffled_nodes )[ map_dax_real[ daxnodeparent.first ] ] ,
							new_node ) );

						new_edge->init_edge();
					}
				}
			}
		}

		XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
		////new_dag->d_shuffled_nodes = NULL;
		////new_dag->d_ccr = simgrid::xbt::random::uniform_real( 0.001 , 100 );
		////if ( CNFG()->d_reset_value_after_load_dax2 )
		////{
		////	new_dag->reset_values_to_random();
		////}
		////XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
		new_dag->d_pRuntime->d_swap_shuffled_nodes = __NEW( _map_nodes );
		*new_dag->d_pRuntime->d_swap_shuffled_nodes = *new_dag->d_pRuntime->d_shuffled_nodes; // important . 14020828
		
		_GUID dag_guid = new_dag->get_tag( "GUID" );
		assert( dag_guid.length() > 0 );
		
		int loop_count = 0;
		bool bret = false;
		do
		{
			if ( new_dag->d_pRuntime->d_shuffled_nodes != NULL ) // روی پاک نمیشه و نود های هم در سوپ دخیره شده پس میشود مجدد لیست نود ها را ساخت
			{
				new_dag->d_pRuntime->d_shuffled_nodes->clear();
				__DELETE( new_dag->d_pRuntime->d_shuffled_nodes );
				new_dag->d_pRuntime->d_shuffled_nodes = NULL; // important . 14020828
			}
			if ( new_dag->d_pRuntime->d_ordered_nodes != NULL )
			{
				new_dag->d_pRuntime->d_ordered_nodes->clear();
				__DELETE( new_dag->d_pRuntime->d_ordered_nodes );
				new_dag->d_pRuntime->d_ordered_nodes = NULL; // important . 14020828
			}
			if ( loop_count > 0 ) XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) add new dag(%s) loop_count(%d)" , __LINE__ , new_dag->get_name().c_str() , loop_count );
			loop_count++;
			
			if ( !SETTINGS_FAST()->d_use_dag_guid_config_cache )
			{
				//cout << dag_guid << "  " << new_dag->d_dag_dax_id << endl;
				if ( get_xml_dag_guid_config( dag_guid ).size() < SETTINGS_FAST()->d_make_dag_n_guid_config )
				{
					new_dag->calc_edge_weight_normalize();
					
					new_dag->d_ccr = new_dag->calculate_ccr();
					bool bfound = false;
					for ( auto & tag : new_dag->d_pRuntime->d_dagtags )
					{
						if ( tag.name == "ccr" )
						{
							tag.value = trim_floatnumber( new_dag->d_ccr );
							bfound = true;
							break;
						}
					}
					if ( bfound == false )
					{
						new_dag->d_pRuntime->d_dagtags.push_back( DAGTag( "ccr" , trim_floatnumber( new_dag->d_ccr ) ) );
					}

					//XBT_INFO_ALIVE_CHECK( "#(%d) %d %f " , __LINE__ , inner_loop_count , new_dag->d_ccr );
					//new_dag->reset_values_with_ccr( false );

					if ( SETTINGS_FAST()->d_make_dag_guid_config )
					{
						DAG::maketime_data data;
						bret = new_dag->complete_dag_structure( &data );

						if ( bret )
						{
							//XBT_INFO( "#(%d) " , __LINE__ );
							//if ( get_xml_dag_guid_config( dag_guid ).size() < SETTINGS_FAST()->d_make_dag_n_guid_config )
							{
								//XBT_INFO( "#(%d) found one dag(%d) guid config dead:%f paral:%d max:%d vmtype(%d)" , __LINE__ , new_dag->d_dag_dax_id ,
								//		  new_dag->d_dag_deadline.d_dag_requestor_deadline_duration , data.ExtraParallelVM , new_dag->d_MaximumPossibleParallelism , new_dag->d_least_price_vmp_type );

								XBT_INFO( "#(%d) found one dag(%d) guid config dead:%f vmtype(%d)" , __LINE__ , new_dag->d_dag_dax_id ,
										  new_dag->d_dag_deadline.d_dag_requestor_deadline_duration , new_dag->d_least_price_vmp_type );

								map< int/*nodedaxid*/ , DurationTime /*each node time duration*/> quotalist;
								for ( auto & dur : data.node_exec_time_duration )
								{
									quotalist[ ( *new_dag->d_pRuntime->d_shuffled_nodes )[ dur.first ]->d_node_dax_id ] = dur.second;
								}

								t_map_nodes_vars node_vars;
								t_map_node_katz node_katz;
								for ( auto & node : *new_dag->d_pRuntime->d_ordered_nodes )
								{
									//node_vars[ node->d_node_dax_id ] = make_tuple( node->d_task_cpu_usage_flops , node->d_task_size_byte , node->d_task_output_size_byte );
									node_katz[ node->d_node_dax_id ] = node->d_Katz_value;
								}
								t_map_nodes_duration node_durations;
								for ( auto & row : data.node_exec_time_duration )
								{
									node_durations[ ( *new_dag->d_pRuntime->d_shuffled_nodes )[ row.first ]->d_node_dax_id ] = row.second;
								}
								assert( new_dag->d_dag_deadline.d_dag_requestor_deadline_duration > 0 );
								//assert( new_dag->d_MaximumPossibleParallelism > 0 );
								tuple_handler handler = make_tuple( data.base_least_price_vm , new_dag->d_dag_deadline.d_dag_requestor_deadline_duration ,
																	data.TotalTimeConsume , -1 , -1 , node_durations );
								tuple_dag_configure dag_configure = make_tuple( get_time() , node_vars , handler , node_katz , new_dag->d_ccr );
								add_dag_guid_config( dag_guid , dag_configure , true );

								if ( get_xml_dag_guid_config( dag_guid ).size() < SETTINGS_FAST()->d_make_dag_n_guid_config )
								{
									bret = false;
									loop_count = 0;
								}
							}
						}
					}
				}
				else
				{
					bret = true; // comment this
				}
			}
			else // use cache
			{
				auto list_config = get_xml_dag_guid_config( dag_guid );
				std::random_device rd;
				std::mt19937 generator( rd() );
				std::shuffle( list_config.begin() , list_config.end() , generator );
				auto choosen_configuration = list_config.front();

				new_dag->d_ccr = std::get<4>( choosen_configuration );

				if ( new_dag->d_pRuntime->d_shuffled_nodes == NULL )
				{
					new_dag->make_shuffled_nodes();
				}

				//DAG::maketime_data data;

				//for ( auto & node : *new_dag->d_pRuntime->d_ordered_nodes )
				//{
				//	node->d_task_cpu_usage_flops = std::get<0>( std::get<1>( choosen_configuration )[ node->d_node_dax_id ] );
				//	node->d_task_size_byte = std::get<1>( std::get<1>( choosen_configuration )[ node->d_node_dax_id ] );
				//	node->d_task_output_size_byte = std::get<2>( std::get<1>( choosen_configuration )[ node->d_node_dax_id ] );
				//	//node->d_Katz_value = std::get<3>( choosen_configuration )[ node->d_node_dax_id ];
				//	//data.node_exec_time_duration[ node->d_node_uniq_id ] = std::get<5>( std::get<2>( choosen_configuration ) )[ node->d_node_dax_id ];
				//}

				//data.TotalTimeConsume =	std::get<2>( std::get<2>( choosen_configuration ) );
				//data.base_least_price_vm = std::get<0>( std::get<2>( choosen_configuration ) );
				//data.p_least_price_vm = &data.base_least_price_vm;
				//data.ExtraParallelVM = std::get<3>( std::get<2>( choosen_configuration ) );
				//new_dag->d_dag_deadline.d_dag_requestor_deadline_duration = std::get<1>( std::get<2>( choosen_configuration ) );
				//new_dag->d_MaximumPossibleParallelism = std::get<4>( std::get<2>( choosen_configuration ) );

				bret = new_dag->complete_dag_structure( NULL , &choosen_configuration );
			}

			//string sresult;
			////if ( SETTINGS_FAST()->d_use_dag_guid_config_cache ) // uncomment this
			//{
			//	bret = new_dag->complete_dag_structure();
			//}
			////else // uncomment this
			//{
			//	//bret = complete_dag_structure__wrapper( new_dag , sresult ); // uncomment this
			//}


			if ( bret )
			{
				vm_price_count[ new_dag->d_least_price_vmp_type ] += 1;
				dag_node_count_each[ new_dag->d_nodes_count ] += 1;
			}

		} while ( bret == false && loop_count < 30 );

		__DELETE( new_dag->d_pRuntime->d_swap_shuffled_nodes );
		new_dag->d_pRuntime->d_swap_shuffled_nodes = NULL;

		if ( bret )
		{
			//__DELETE( new_dag ); // comment this
			d_dags.push_back( new_dag ); // uncomment this
		}

		if ( time_keeper.peek_time() )
		{
			XBT_INFO_ALIVE_CHECK( "#(%d) dags(%d)" , __LINE__ , (int)d_dags.size() );
		}
		if ( SETTINGS()->d_order_stop_loop_dag_storage_add )
		{
			return;
		}
	}
	////if ( SETTINGS()->d_reset_value_after_load_dax2 )
	////{
	////	XBT_INFO( "#(%d) ver(%d) reset value" , __LINE__ , SETTINGS()->d_reset_values_type );
	////}
	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "#(%d) least vm price" , __LINE__ );
	for ( auto & price : vm_price_count )
	{
		XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "vm type(%d): (%d)" , price.first , price.second );
	}

	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "#(%d) dag size" , __LINE__ );
	for ( auto & sizee : dag_node_count_each )
	{
		XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "dag size(%d): (%d)" , sizee.first , sizee.second );
	}

	try
	{
		finish_xml_dag_guid_config_read(); // uncomment this
	}
	catch ( ... )
	{
		cout << "exception " << __LINE__ << endl;
	}

	assert( d_dags.size() == d_DAXdags.d_dax_dags.size() ); // uncomment this
	d_DAXdags.cleanup();

	cout << "#" << __LINE__ << " end " << __FUNCTION__ << endl;

	CATCH_TRY
}

////void DAGstorage::convert_dags_to_dax2() // دکس2 از تبدیل محاسبه بدست نمی آید بلکه یا کش شده و یا از فابل میاید
////{
////	BEGIN_TRY
////	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
////	d_DAXdags.cleanup();
////	d_DAXdags.d_daxver = "standard2.0";
////
////	map< int /*least vm price*/ , int /*count*/ > vm_price_count;
////	map< int /*node count*/ , int /*count*/ > dag_node_count_each;
////
////	double min_task_cpu_usage_flops = -1;
////	double min_task_size_byte = -1;
////	double min_task_output_size_byte = -1;
////
////	double max_task_cpu_usage_flops = -1;
////	double max_task_size_byte = -1;
////	double max_task_output_size_byte = -1;
////
////	double avg_task_cpu_usage_flops = 0;
////	double avg_task_size_byte = 0;
////	double avg_task_output_size_byte = 0;
////
////	double node_count = 0;
////
////	for ( auto & dag : d_dags )
////	{
////		for ( auto & node : *dag->d_pRuntime->d_ordered_nodes )
////		{
////			node_count++;
////
////			if ( min_task_cpu_usage_flops < 0 || node->d_task_cpu_usage_flops < min_task_cpu_usage_flops )
////			{
////				min_task_cpu_usage_flops = node->d_task_cpu_usage_flops;
////			}
////			if ( min_task_size_byte < 0 || node->d_task_size_byte < min_task_size_byte )
////			{
////				min_task_size_byte = node->d_task_size_byte;
////			}
////			if ( min_task_output_size_byte < 0 || node->d_task_output_size_byte < min_task_output_size_byte )
////			{
////				min_task_output_size_byte = node->d_task_output_size_byte;
////			}
////
////			if ( max_task_cpu_usage_flops < 0 || node->d_task_cpu_usage_flops > max_task_cpu_usage_flops )
////			{
////				max_task_cpu_usage_flops = node->d_task_cpu_usage_flops;
////			}
////			if ( max_task_size_byte < 0 || node->d_task_size_byte > max_task_size_byte )
////			{
////				max_task_size_byte = node->d_task_size_byte;
////			}
////			if ( max_task_output_size_byte < 0 || node->d_task_output_size_byte > max_task_output_size_byte )
////			{
////				max_task_output_size_byte = node->d_task_output_size_byte;
////			}
////
////			avg_task_cpu_usage_flops += node->d_task_cpu_usage_flops;
////			avg_task_size_byte += node->d_task_size_byte;
////			avg_task_output_size_byte += node->d_task_output_size_byte;
////		}
////
////		//XBT_INFO_OVERRIDEABLE__( "#(%d) " , __LINE__ );
////		DAX1Dag * new_dax_dag = __NEW( DAX1Dag() );
////		new_dax_dag->d_daxdag_dagid = dag->d_dag_dax_id;
////		new_dax_dag->d_daxdag_name = dag->d_pRuntime->d_dag_name;
////		////new_dax_dag->d_daxdag_dagtype = dag->d_dag_type.d_dag_type;
////		new_dax_dag->d_daxtags = dag->d_pRuntime->d_dagtags;
////		new_dax_dag->d_daxtags.push_back( DAGTag( "nodecount" , std::to_string( dag->d_nodes_count ) ));
////
////		vm_price_count[ dag->d_least_price_vmp_type ] += 1;
////		dag_node_count_each[ dag->d_nodes_count ] += 1;
////
////		for ( auto & dagnode : *dag->d_pRuntime->d_ordered_nodes )
////		{
////			//XBT_INFO_OVERRIDEABLE__( "#(%d) " , __LINE__ );
////			DAX1Node * new_dax_node = __NEW( DAX1Node( dagnode->d_node_dax_id ) );
////			new_dax_node->d_owner = new_dax_dag;
////
////			new_dax_node->d_task_cpu_usage_flops = dagnode->d_task_cpu_usage_flops;
////			new_dax_node->d_task_size_byte = dagnode->d_task_size_byte;
////			new_dax_node->d_task_output_size_byte = dagnode->d_task_output_size_byte;
////
////			new_dax_dag->d_nodes[ new_dax_node->d_daxnode_nodeid ] = new_dax_node;
////		}
////		//XBT_INFO_OVERRIDEABLE__( "#(%d) " , __LINE__ );
////		for ( auto & root_node : dag->d_pRuntime->d_roots )
////		{
////			new_dax_dag->d_roots[ root_node->d_node_dax_id ] = new_dax_dag->d_nodes[ root_node->d_node_dax_id ];
////		}
////		//XBT_INFO_OVERRIDEABLE__( "#(%d) " , __LINE__ );
////		for ( auto & node : *dag->d_pRuntime->d_ordered_nodes )
////		{
////			for ( auto & link : node->d_links )
////			{
////				//XBT_INFO_OVERRIDEABLE__( "#(%d) " , __LINE__ );
////				DAX1Edge * new_dax_edge = __NEW( DAX1Edge(
////					new_dax_dag->d_nodes[ (link->d_pSource)->d_node_dax_id ] ,
////					new_dax_dag->d_nodes[ (link->d_pDestination)->d_node_dax_id ] ) );
////				new_dax_dag->d_nodes[ (link->d_pSource)->d_node_dax_id ]->d_links.push_back( new_dax_edge );
////
////				new_dax_dag->d_nodes[ (link->d_pDestination)->d_node_dax_id ]->d_parents[ (link->d_pSource)->d_node_dax_id ] =
////					new_dax_dag->d_nodes[ (link->d_pSource)->d_node_dax_id ];
////			}
////		}
////		//XBT_INFO_OVERRIDEABLE__( "#(%d) " , __LINE__ );
////		d_DAXdags.d_dax_dags.push_back( new_dax_dag );
////	}
////
////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "#(%d) node statistic" , __LINE__ );
////
////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "min_task_cpu_usage_flops (%f)" , min_task_cpu_usage_flops );
////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "min_task_size_byte (%f)" , min_task_size_byte );
////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "min_task_output_size_byte (%f)" , min_task_output_size_byte );
////
////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "max_task_cpu_usage_flops (%f)" , max_task_cpu_usage_flops );
////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "max_task_size_byte (%f)" , max_task_size_byte );
////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "max_task_output_size_byte (%f)" , max_task_output_size_byte );
////
////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "avg_task_cpu_usage_flops (%f)" , avg_task_cpu_usage_flops / node_count );
////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "avg_task_size_byte (%f)" , avg_task_size_byte / node_count );
////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "avg_task_output_size_byte (%f)" , avg_task_output_size_byte / node_count );
////
////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "#(%d) least vm price" , __LINE__ );
////	for ( auto & price : vm_price_count )
////	{
////		XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "vm type(%d): (%d)" , price.first , price.second );
////	}
////
////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "#(%d) dag size" , __LINE__ );
////	for ( auto & sizee : dag_node_count_each )
////	{
////		XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_always_dump , "dag size(%d): (%d)" , sizee.first , sizee.second );
////	}
////
////
////	std::fstream out( "/mnt/c/simgrid/simgrid-3.32/out/build/WSL-GCC-Debug/examples/c/actor-create/DAX2_fromDAGS.txt" , std::ios::out );
////	d_DAXdags.serializeDAX2( out , this->d_pname );
////	out.close();
////	d_DAXdags.cleanup();
////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
////	CATCH_TRY
////}
////void DAGstorage::convert_dags_to_dax1()
////{
////	BEGIN_TRY
//////	d_DAXdags.d_dax_dags.clear();
////	d_DAXdags.d_daxver = "standard2.0";
////
////	for ( auto & dag : d_dags )
////	{
////		shared_ptr< daxDAG > new_dax_dag = make_shared<daxDAG>();
////		new_dax_dag->d_dax_id = dag->d_dag_dax_id;
////		new_dax_dag->d_dax_dag_name = dag->d_dag_name;
////		new_dax_dag->d_dag_node_type = dag->d_dag_type.d_dag_type;
////		new_dax_dag->d_daxtags = dag->d_dagtags;
////
////		for ( shared_ptr< DAGNode > & dagnode : *dag->d_ordered_nodes )
////		{
////			shared_ptr< daxNODE > new_dax_node = make_shared< daxNODE >( dagnode->d_dax_id );
////			new_dax_node->d_owner = new_dax_dag;
////
////			//new_dax_node->d_task_cpu_usage_flops = dagnode->d_task_cpu_usage_flops;
////			//new_dax_node->d_task_size_byte = dagnode->d_task_size_byte;
////			//new_dax_node->d_task_output_size_byte = dagnode->d_task_output_size_byte;
////
////			new_dax_dag->d_nodes[ new_dax_node->d_dax_nodeid ] = new_dax_node;
////		}
////		for ( auto & root_node : dag->d_roots )
////		{
////			new_dax_dag->d_roots[ root_node->d_dax_id ] = new_dax_dag->d_nodes[ root_node->d_dax_id ];
////		}
////
////		for ( auto & node : *dag->d_ordered_nodes )
////		{
////			for ( auto & link : node->d_links )
////			{
////				shared_ptr< daxEDGE > new_dax_edge = make_shared<daxEDGE>(
////					new_dax_dag->d_nodes[ link->d_source->d_dax_id ] , new_dax_dag->d_nodes[ link->d_destination->d_dax_id ] );
////				new_dax_dag->d_nodes[ link->d_source->d_dax_id ]->d_links.push_back( new_dax_edge );
////
////				new_dax_dag->d_nodes[ link->d_destination->d_dax_id ]->d_parents[ link->d_source->d_dax_id ] =
////					new_dax_dag->d_nodes[ link->d_source->d_dax_id ];
////			}
////		}
////		d_DAXdags.d_dax_dags.push_back( new_dax_dag );
////	}
////
////	std::fstream out( "/mnt/c/simgrid/simgrid-3.32/out/build/WSL-GCC-Debug/examples/c/actor-create/DAX1_fromDAGS.txt" , std::ios::out );
////	d_DAXdags.serializeDAX1( out );
////	out.close();
////	CATCH_TRY
////}
////void DAGstorage::convert_dags_to_DOT1()
////{
////	BEGIN_TRY
////	std::fstream out( "/mnt/c/simgrid/simgrid-3.32/out/build/WSL-GCC-Debug/examples/c/actor-create/DOT1_fromDAGS.txt" , std::ios::out );
////	for ( auto & dag : d_dags )
////	{
////		out << "\ndigraph " << dag->d_dag_name << " {\n";
////		//int counter = 0;
////		map< int , eNodeID > map_dax_2_node;
////		map< eNodeID , int > map_node_2_dax;
////		for ( auto & node : *dag->d_shuffled_nodes )
////		{
////			//counter++;
////			map_dax_2_node[ node.second->d_dax_id ] = node.second->d_node_uniq_id;
////			map_node_2_dax[ node.second->d_node_uniq_id ] = node.second->d_dax_id;
////		}
////
////		for ( auto & node : *dag->d_shuffled_nodes )
////		{
////			out << "  " << map_node_2_dax[ node.second->d_node_uniq_id ] << "\n";
////			map< eNodeID , eNodeID> dest;
////			for ( auto & link : node.second->d_links )
////			{
////				dest[ link->d_destination->d_node_uniq_id ] = link->d_destination->d_node_uniq_id;
////			}
////			for ( auto & dst : dest )
////			{
////				out << "  " << map_node_2_dax[ node.second->d_node_uniq_id ] << " -> " << map_node_2_dax[ dst.first ] << "\n";
////			}
////		}
////
////		out << "}\n";
////	}
////	out.close();
////	CATCH_TRY
////}
////void DAGstorage::convert_dax1_to_DOT1()
////{
////	BEGIN_TRY
////	std::fstream out( "/mnt/c/simgrid/simgrid-3.32/out/build/WSL-GCC-Debug/examples/c/actor-create/DOT1_fromDAX1base.txt" , std::ios::out );
////	for ( auto & dax_dag : d_DAXdags.d_dax_dags )
////	{
////		out << "\ndigraph " << dax_dag->d_dax_dag_name << " {\n";
////		for ( auto & node : dax_dag->d_nodes )
////		{
////			out << "  " << node.first << "\n";
////			for ( auto & link : node.second->d_links )
////			{
////				out << "  " << link->d_src->d_dax_nodeid << " -> " << link->d_dst->d_dax_nodeid << "\n";
////			}
////		}
////		out << "}\n";
////	}
////	out.close();
////	CATCH_TRY
////}

#pragma endregion

