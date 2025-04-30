#define uses_priority_queue
#define uses_LOG
#define uses_string_format
#define uses_random
#define uses_CNFG
#define uses_VMs
//#define uses_DGG
#define uses_SectionTimeKeeper
#define uses_BenchmarkIteration

#include "../inc/dep"

XBT_LOG_NEW_DEFAULT_CATEGORY( Vmm , "Messages specific for this s4u example" );

// law.
// 1. vm warm up time and cost resposibility in on requestor
// 2. vm memory is replace for ram and hard and is infinit capacity

#pragma region VMQuality

string VMQuality::bundle_nodes_text() const
{
	BEGIN_TRY
	string dump_bundle_nodes;
	if ( SETTINGS()->d_low_memory_mode )
	{
		return dump_bundle_nodes;
	}
	if ( d_pAllocated_bundled_nodes_ordered != NULL )
	{
		dump_bundle_nodes += string_format( "dag(%s) bundled nodes" , d_pAllocated_bundled_nodes_ordered->front()->d_pOwnerDAG->get_name().c_str() );
		for ( auto & ref_node : *d_pAllocated_bundled_nodes_ordered )
		{
			dump_bundle_nodes += "(" + string_format( "%s" , ref_node->d_allocated_node_name ) + "),";
		}
	}
	return dump_bundle_nodes;
	CATCH_TRY
	return string( "" );
}

Vmm * VMQuality::get_vmm() const
{
	return this->d_pAliasVM->get_vmm();
}
DAG * VMQuality::get_DAG() const
{
	return this->d_pAllocated_bundled_nodes_ordered->front()->d_pOwnerDAG;
}

void VMQuality::dump( bool inside_another , bool bshort ) const
{
	BEGIN_TRY
	if ( !inside_another ) XBT_INFO_NOTIME( "" , "" );
	if ( pre_scheduling_quality() )
	{
		XBT_INFO( "dump_Quality , @%s" ,
				  this->d_pAlias_bundled_nodes_ordered->front()->d_pOwnerDAG->get_name().c_str() );
	}
	else
	{
		XBT_INFO( "dump_Quality , @%s" ,
				  this->d_pAllocated_bundled_nodes_ordered->front()->d_pOwnerDAG->get_name().c_str() );
	}

	d_pAliasVM->dump( true , bshort );

	if ( d_stablish_vm && d_stablish_container )
	{
		XBT_INFO_NOTIME( "Un_provisioned VM  ,  NON stablished container" , "" );
	}
	else if ( d_stablish_vm )
	{
		XBT_INFO_NOTIME( "Un_provisioned VM" , "" );
	}
	else if ( d_stablish_container )
	{
		XBT_INFO_NOTIME( "NON stablished container" , "" );
	}
	if ( d_occupied_vm )
	{
		XBT_INFO_NOTIME( "Occupied VM" , "" );
	}

	if ( !d_EPSM_algorithm )
	{
		XBT_INFO_NOTIME( "TotalResidentInputs(%s b)\nTotalInternalEdgeWeight(%s b)" ,
						 trim_floatnumber( d_estimate_kasr_TotalResidentInputs_size_byte ).c_str() ,
						 trim_floatnumber( d_estimate_kasr_TotalInternalEdgeWeight_size_byte ).c_str() );
		XBT_INFO_NOTIME( "TotalTransferedInput(%s b)\nTotalExternalEdgeWeight(%s b)\nTotalExportNodeOutputToGS(%s)" ,
						 trim_floatnumber( d_estimate_kasr_TotalTransferedInput_size_byte ).c_str() ,
						 trim_floatnumber( d_estimate_kasr_TotalExternalEdgeWeight_size_byte ).c_str() ,
						 trim_floatnumber( d_estimate_TotalExportNodeOutputToGS_size_byte ).c_str() );
		XBT_INFO_NOTIME( "kasr_ProcessCost(%s $)\nkasr_DataTransferCost(%s $)" ,
						 trim_floatnumber( d_estimate_kasr_ProcessCost ).c_str() ,
						 trim_floatnumber( d_estimate_kasr_DataTransferCost ).c_str() );
		XBT_INFO_NOTIME( "a^2+b^2/a+b(%s $)" ,
						 trim_floatnumber( ( pow( d_estimate_kasr_ProcessCost , 2.0 ) + pow( d_estimate_kasr_DataTransferCost , 2.0 ) ) / ( d_estimate_kasr_ProcessCost + d_estimate_kasr_DataTransferCost ) ).c_str() );

		XBT_INFO_NOTIME( "kasr_AllocationNewMachineCost(%s $)\nkasr_ContainerStablishmentCost(%s $)" ,
						 trim_floatnumber( d_estimate_kasr_AllocationNewMachineCost ).c_str() ,
						 trim_floatnumber( d_estimate_kasr_ContainerStablishmentCost ).c_str() );

		//XBT_INFO_NOTIME( "" , "" );
		XBT_INFO_NOTIME( "QQuality ratio(%s) QQuality ratio x10000(%s)" ,
						 trim_floatnumber( d_estimate_quality_ratio ).c_str() ,
						 trim_floatnumber( d_estimate_quality_ratio * 10000 ).c_str() );
		

		XBT_INFO_NOTIME( "TotalCost(%s $)\nuse_leased_cycle_cost(%s $)\nuse_new_leased_cycle_cost(%s $)" ,
						 trim_floatnumber( d_estimate_TotalCost ).c_str() ,
						 trim_floatnumber( d_estimate_use_leased_cycle_cost ).c_str() ,
						 trim_floatnumber( d_estimate_use_new_leased_cycle_cost ).c_str() );

		XBT_INFO_NOTIME( "TotalTransferedInputFromLocalRegionSize(%s b)\nTotalTransferedInputFromGlobalStorageSize(%s b)" ,
						 trim_floatnumber( d_estimate_TotalTransferedInputFromLocalRegionSize_byte ).c_str() ,
						 trim_floatnumber( d_estimate_TotalTransferedInputFromGlobalStorageSize_byte ).c_str() );
	}
	else
	{
		//XBT_INFO_NOTIME( "kasr_AllocationNewMachineCost(%s $)\nkasr_ContainerStablishmentCost(%s $)" ,
		//				 trim_floatnumber( d_estimate_kasr_AllocationNewMachineCost ).c_str() ,
		//				 trim_floatnumber( d_estimate_kasr_ContainerStablishmentCost ).c_str() );

		XBT_INFO_NOTIME( "TotalCost(%s $)\nuse_new_leased_cycle_cost(%s $)" ,
						 trim_floatnumber( d_estimate_TotalCost ).c_str() ,
						 trim_floatnumber( d_estimate_use_new_leased_cycle_cost ).c_str() );

		//XBT_INFO_NOTIME( "d_bundle_duration_need_on_cheapest_vm_plus_spare(%f s)" , d_bundle_duration_need_on_cheapest_vm_plus_spare );
	}

	if ( pre_scheduling_quality() )
	{
		assert( d_pAlias_bundled_nodes_ordered->size() );
		if ( d_pAlias_bundled_nodes_ordered->front()->get_actual_triggered_time() != NULL &&
			 d_pAlias_bundled_nodes_ordered->back()->get_actual_complete_time() != NULL )
		{
			//XBT_INFO_NOTIME( "" , "" );
			XBT_INFO_NOTIME( "start_scheduling_quality_time(at %f s) , end_exec_quality_time(at %f s) , diff(%f s)" ,
							 *d_pAlias_bundled_nodes_ordered->front()->get_actual_triggered_time() ,
							 *d_pAlias_bundled_nodes_ordered->back()->get_actual_complete_time() ,
							 *d_pAlias_bundled_nodes_ordered->back()->get_actual_complete_time() - *d_pAlias_bundled_nodes_ordered->front()->get_actual_triggered_time() );
		}
		else if ( d_pAlias_bundled_nodes_ordered->front()->get_actual_triggered_time() != NULL )
		{
			XBT_INFO_NOTIME( "start_scheduling_quality_time(at %f s)" , *d_pAlias_bundled_nodes_ordered->front()->get_actual_triggered_time() );
		}
		else if ( d_pAlias_bundled_nodes_ordered->back()->get_actual_complete_time() != NULL )
		{
			XBT_INFO_NOTIME( "end_exec_quality_time(at %f s)" , *d_pAlias_bundled_nodes_ordered->back()->get_actual_complete_time() );
		}
	}
	else
	{
		assert( d_pAllocated_bundled_nodes_ordered->size() );
		if ( d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() != NULL &&
			 d_pAllocated_bundled_nodes_ordered->back()->get_actual_complete_time() != NULL )
		{
			//XBT_INFO_NOTIME( "" , "" );
			XBT_INFO_NOTIME( "start_scheduling_quality_time(at %f s) , end_exec_quality_time(at %f s) , diff(%f s)" ,
							 *d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() ,
							 *d_pAllocated_bundled_nodes_ordered->back()->get_actual_complete_time() ,
							 *d_pAllocated_bundled_nodes_ordered->back()->get_actual_complete_time() - *d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() );
		}
		else if ( d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() != NULL )
		{
			XBT_INFO_NOTIME( "start_scheduling_quality_time(at %f s)" , *d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() );
		}
		else if ( d_pAllocated_bundled_nodes_ordered->back()->get_actual_complete_time() != NULL )
		{
			XBT_INFO_NOTIME( "end_exec_quality_time(at %f s)" , *d_pAllocated_bundled_nodes_ordered->back()->get_actual_complete_time() );
		}
	}

	if ( !SETTINGS()->d_low_memory_mode )
	{
		XBT_INFO_NOTIME( "%s" , bundle_nodes_text().c_str() );
	}

	string temp1;
	string sseperator = "\n";
	//if ( !bshort ) XBT_INFO_NOTIME( "" , "" );

	if ( !bshort ) temp1 += string_format( "d_estimate_VirtualMachine_Warmup_time_consume(%s s)" , trim_floatnumber( d_estimate_VirtualMachine_Warmup_time_consume ).c_str() ) + sseperator;
	if ( !bshort ) temp1 += string_format( "d_estimate_StablishContainer_time_consume(%s s)" , trim_floatnumber( d_estimate_StablishContainer_time_consume ).c_str() ) + sseperator;
	if ( !bshort ) temp1 += string_format( "d_estimate_read_tasks_from_global_storage_time_consume(%s s)" , trim_floatnumber( d_estimate_read_tasks_from_global_storage_time_consume ).c_str() ) + sseperator;
	if ( !bshort ) temp1 += string_format( "d_estimate_transfer_tasks_from_global_storage_to_vm_time_consume(%s s)" , trim_floatnumber( d_estimate_transfer_tasks_from_global_storage_to_vm_time_consume ).c_str() ) + sseperator;
	if ( !bshort ) temp1 += string_format( "d_estimate_write_tasks_to_vm_time_consume(%s s)" , trim_floatnumber( d_estimate_write_tasks_to_vm_time_consume ).c_str() ) + sseperator;
	if ( !bshort ) temp1 += string_format( "d_estimate_read_parent_output_from_global_storage_memory_time_consume(%s s)" , trim_floatnumber( d_estimate_read_parent_output_from_global_storage_memory_time_consume ).c_str() ) + sseperator;
	if ( !bshort ) temp1 += string_format( "d_estimate_TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume(%s s)" , trim_floatnumber( d_estimate_TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume ).c_str() ) + sseperator;
	if ( !bshort ) temp1 += string_format( "d_estimate_write_output_parent_from_global_storage_on_vm(%s s)" , trim_floatnumber( d_estimate_write_output_parent_from_global_storage_on_vm ).c_str() ) + sseperator;
	if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_estimate_TotalTransfered_ReadMemory_InputFromLocalRegionTimeConsume(%s s)" , trim_floatnumber( d_estimate_TotalTransfered_ReadMemory_InputFromLocalRegionTimeConsume ).c_str() ) + sseperator;
	if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_estimate_TotalTransfered_OnNetwork_InputFromLocalRegionTimeConsume(%s s)" , trim_floatnumber( d_estimate_TotalTransfered_OnNetwork_InputFromLocalRegionTimeConsume ).c_str() ) + sseperator;
	if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_estimate_write_output_parent_from_local_region_on_vm(%s s)" , trim_floatnumber( d_estimate_write_output_parent_from_local_region_on_vm ).c_str() ) + sseperator;
	if ( !bshort ) temp1 += string_format( "d_estimate_exec_tasks_time_consume(%s s)" , trim_floatnumber( d_estimate_exec_tasks_time_consume ).c_str() ) + sseperator;
	if ( !bshort ) temp1 += string_format( "d_estimate_write_output_to_vm(%s s)" , trim_floatnumber( d_estimate_write_output_to_vm_time_consume ).c_str() ) + sseperator;
	if ( !bshort ) temp1 += string_format( "d_estimate_transfer_output_to_global_storage(%s s)" , trim_floatnumber( d_estimate_transfer_output_to_global_storage_time_consume ).c_str() ) + sseperator;
	if ( !bshort ) temp1 += string_format( "d_estimate_write_output_to_global_storage(%s s)" , trim_floatnumber( d_estimate_write_output_to_global_storage_time_consume ).c_str() ) + sseperator;
	if ( !bshort ) temp1 += string_format( "d_estimate_TotalTimeConsume(%s s)" , trim_floatnumber( d_estimate_TotalTimeConsume ).c_str() ) + sseperator;
	if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_estimate_local_bundle_parent_ids%s" , d_estimate_local_bundle_parent_ids.c_str() ) + sseperator;
	if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_estimate_local_memory_parent_ids%s" , d_estimate_local_memory_parent_ids.c_str() ) + sseperator;
	if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_estimate_local_region_parent_ids%s" , d_estimate_local_region_parent_ids.c_str() ) + sseperator;
	if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_estimate_globally_parent_ids%s" , d_estimate_globally_parent_ids.c_str() ) + sseperator;
	if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_estimate_in_local_region_memory_resident_ids%s" , d_estimate_in_local_region_memory_resident_ids.c_str() ) + sseperator;

	//if ( !bshort ) XBT_INFO_NOTIME( "" , "" );
	//if ( !bshort ) temp1 += string_format( "d_actually_VirtualMachine_Warmup_time_consume(%s s)" , trim_floatnumber( d_actually_VirtualMachine_Warmup_time_consume ).c_str() ) + sseperator;
	//if ( !bshort ) temp1 += string_format( "d_actually_container_initialization_time(%s s)" , trim_floatnumber( d_actually_container_initialization_time_consume ).c_str() ) + sseperator;
	//if ( !bshort ) temp1 += string_format( "d_actually_read_task_from_global_storage_memory(%s s)" , trim_floatnumber( d_actually_read_task_from_global_storage_memory_time_consume ).c_str() ) + sseperator;
	//if ( !bshort ) temp1 += string_format( "d_actually_transfer_task_from_global_storage_to_vm(%s s)" , trim_floatnumber( d_actually_transfer_task_from_global_storage_to_vm_time_consume ).c_str() ) + sseperator;
	//if ( !bshort ) temp1 += string_format( "d_actually_store_task_to_vm_memory(%s s)" , trim_floatnumber( d_actually_store_task_to_vm_memory_time_consume ).c_str() ) + sseperator;
	//if ( !bshort ) temp1 += string_format( "d_actually_read_parent_output_from_global_storage_memory(%s s)" , trim_floatnumber( d_actually_read_parent_output_from_global_storage_memory_time_consume ).c_str() ) + sseperator;
	//if ( !bshort ) temp1 += string_format( "d_actually_transfer_parent_output_from_global_storage_to_vm_memory(%s s)" , trim_floatnumber( d_actually_transfer_parent_output_from_global_storage_to_vm_memory_time_consume ).c_str() ) + sseperator;
	//if ( !bshort ) temp1 += string_format( "d_actually_write_parent_output_global_storage_to_vm_memory(%s s)" , trim_floatnumber( d_actually_write_parent_output_global_storage_to_vm_memory_time_consume ).c_str() ) + sseperator;
	//if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_actually_read_parent_output_from_local_region_memory(%s s)" , trim_floatnumber( d_actually_read_parent_output_from_local_region_memory_time_consume ).c_str() ) + sseperator;
	//if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_actually_transfer_parent_output_from_local_to_vm_memory(%s s)" , trim_floatnumber( d_actually_transfer_parent_output_from_local_to_vm_memory_time_consume ).c_str() ) + sseperator;
	//if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_actually_write_parent_output_from_local_to_vm_memory(%s s)" , trim_floatnumber( d_actually_write_parent_output_from_local_to_vm_memory_time_consume ).c_str() ) + sseperator;
	//if ( !bshort ) temp1 += string_format( "d_actually_exec_task_on_vm_cpu(%s s)" , trim_floatnumber( d_actually_exec_task_on_vm_cpu_time_consume ).c_str() ) + sseperator;
	//if ( !bshort ) temp1 += string_format( "d_actually_write_task_output_to_vm_memory(%s s)" , trim_floatnumber( d_actually_write_task_output_to_vm_memory_time_consume ).c_str() ) + sseperator;
	//if ( !bshort ) temp1 += string_format( "d_actually_transfer_output_to_global_storage(%s s)" , trim_floatnumber( d_actually_transfer_output_to_global_storage_time_consume ).c_str() ) + sseperator;
	//if ( !bshort ) temp1 += string_format( "d_actually_write_output_task_on_global_storage_disk(%s s)" , trim_floatnumber( d_actually_write_output_task_on_global_storage_disk_time_consume ).c_str() ) + sseperator;
	
	//if ( pre_scheduling_quality() )
	//{
	//	if ( d_pAlias_bundled_nodes_ordered->front()->get_actual_triggered_time() != NULL &&
	//		 d_pAlias_bundled_nodes_ordered->back()->get_actual_complete_time() != NULL )
	//		if ( !bshort ) temp1 += string_format( "d_actually_TotalTimeConsume(%s s)" , trim_floatnumber( *d_pAlias_bundled_nodes_ordered->back()->get_actual_complete_time() - *d_pAlias_bundled_nodes_ordered->front()->get_actual_triggered_time() ).c_str() ) + sseperator;
	//}
	//else
	//{
	//	if ( d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() != NULL &&
	//		 d_pAllocated_bundled_nodes_ordered->back()->get_actual_complete_time() != NULL )
	//		if ( !bshort ) temp1 += string_format( "d_actually_TotalTimeConsume(%s s)" , trim_floatnumber( *d_pAllocated_bundled_nodes_ordered->back()->get_actual_complete_time() - *d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() ).c_str() ) + sseperator;
	//}
	//if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_actually_local_memory_parent_ids%s" , d_actually_local_memory_parent_ids.c_str() ) + sseperator;
	//if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_actually_local_region_parent_ids%s" , d_actually_local_region_parent_ids.c_str() ) + sseperator;
	//if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_actually_globally_parent_ids%s" , d_actually_globally_parent_ids.c_str() ) + sseperator;
	//if ( !d_EPSM_algorithm ) if ( !bshort ) temp1 += string_format( "d_actually_in_local_region_memory_resident_ids%s" , d_actually_in_local_region_memory_resident_ids.c_str() ) + sseperator;

	if ( !bshort ) temp1 += string_format( "TotalProcessFlops(%s flop)\nTotalTaskSize(%s b)" , trim_floatnumber( d_estimate_TotalProcessFlops ).c_str() , trim_floatnumber(d_estimate_TotalTaskSize_byte ).c_str() ) + sseperator;
	if ( !bshort ) temp1 += string_format( "estimate_TotalOutputToVMMemory(%s b)" ,
										   trim_floatnumber( d_estimate_TotalOutputToVMMemory_size_byte ).c_str() ) + sseperator;
	if ( !bshort ) temp1 += string_format( "estimate_finished_prev_running_job_time_consume(%s s)\nestimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time(%s s)" ,
										   trim_floatnumber( d_estimate_finished_prev_running_job_time_consume ).c_str() , trim_floatnumber( d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time ).c_str() ) + sseperator;

	if ( temp1.length() > 0 ) XBT_INFO_NOTIME( temp1 , "" );

	//if ( d_ret_message.length() > 0 ) XBT_INFO_NOTIME( d_ret_message , "" );
	CATCH_TRY
}

VMQuality::~VMQuality()
{
	if ( SETTINGS_FAST()->d_low_memory_mode )
	{
		// باعث می شد که در اسمجول مواردی داشتیم که اصل نود پاک شده بود
		////if ( d_clean_dag_after_quality_finished && d_quality_bundled_nodes_ordered != NULL )
		////{
		////	d_quality_bundled_nodes_ordered->front()->d_pOwnerDAG->clear_unnecessary_data();
		////}
	}
	cleanup();
}
void VMQuality::cleanup()
{
	if ( d_pAllocated_bundled_nodes_ordered != NULL )
	{
		__DELETE( d_pAllocated_bundled_nodes_ordered );
		d_pAllocated_bundled_nodes_ordered = NULL;
	}
	if ( d_pAllocated_bundled_nodes_shuffled != NULL )
	{
		__DELETE( d_pAllocated_bundled_nodes_shuffled );
		d_pAllocated_bundled_nodes_shuffled = NULL;
	}
}

void VMQuality::start_scheduling_quality( const VMQuality * this_quality )
{
	BEGIN_TRY
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

	std::get<1>( this_quality->get_vmm()->d_statistics[ AeStatisticElement::stt_online_vm_count ]
		[ STATISTICS_INT( this_quality->d_pAliasVM->d_vmType->d_type ) ].online_vm_count ) += 1; // busy
	//this_quality->d_pVM->d_owner_region->d_vm_type_online_leased_busy_count[ this_quality->d_pVM->d_vmType->d_type ] += 1;

	this_quality->get_DAG()->d_dag_deadline.declare_dag_start_schedule();
	for ( auto & node : *this_quality->d_pAllocated_bundled_nodes_ordered )
	{
		node->d_pRuntime->d_taskScheduled_in_vm = true;
		// تسک در واقع اینجا شروع نمی شود بلکه اول لود می شود در ماشین س÷س تک به تک به آن فرمان اجرا داده می شود
		// و لود شدن هم زمانی است که برای خود تسک است و از زمان خودش کسر می شود
		// در ضمن وقتی تسکی دیتای والدین خود را دریافت می کند این دیتا در واقع در کانتینر مستقر می ماند
		////node->d_task_deadline->declare_task_start_time(); // هر تسک وقتی شروع می شود استاراش می خورد
	}
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	d_pAliasVM->d_leased_cycle_mgr.start_to_use( this_quality );
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	
	if ( SETTINGS_FAST()->d_app_mode < AeAppMode::apmd_first_bundle_scheduled_mode )
	{
		SETTINGS_FAST()->d_app_mode = AeAppMode::apmd_first_bundle_scheduled_mode;
	}

	CATCH_TRY
}

void VMQuality::finish_executing_quality( const VMQuality * this_quality )
{
	double differ_from_estimate_duration_and_actual_finish_time = 0;
	BEGIN_TRY
		//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
		////std::lock_guard lock(*this->d_pVM->d_owner_region->d_pGlobalVMM->d_critical_section_entered_mutex); // ماشینی که داره کار میکنه در طرح جمع اوری نمی رود

		assert( d_pAllocated_bundled_nodes_ordered != NULL );
		assert( d_pAllocated_bundled_nodes_ordered->size() );

		double bundle_execution_duration = ( *d_pAllocated_bundled_nodes_ordered->back()->get_actual_complete_time() -
											 *d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() );

		differ_from_estimate_duration_and_actual_finish_time =
			this->d_estimate_TotalTimeConsume - bundle_execution_duration;
				
		d_pAliasVM->d_leased_cycle_mgr.end_use( this_quality );
		
		std::get<1>( this_quality->get_vmm()->d_statistics[AeStatisticElement::stt_online_vm_count][STATISTICS_INT(this_quality->d_pAliasVM->d_vmType->d_type)].online_vm_count) =
			max( std::get<1>( this_quality->get_vmm()->d_statistics[ AeStatisticElement::stt_online_vm_count ][ STATISTICS_INT( this_quality->d_pAliasVM->d_vmType->d_type ) ].online_vm_count ) - 1 , 0 ); // idle
		////this_quality->d_pVM->d_owner_region->d_vm_type_online_leased_busy_count[ this_quality->d_pVM->d_vmType->d_type ] = max( this_quality->d_pVM->d_owner_region->d_vm_type_online_leased_busy_count[ this_quality->d_pVM->d_vmType->d_type ] - 1 , 0 );

	CATCH_TRY

	// اگر اختلاف بین زمان تخمینی و واقعی زیاد بود دامپش کن
	if ( SETTINGS_FAST()->d_dump_bundle_if_estimation_and_real_execution_time_different_larger_than > 0.0 &&
		 fabs( differ_from_estimate_duration_and_actual_finish_time ) > SETTINGS_FAST()->d_dump_bundle_if_estimation_and_real_execution_time_different_larger_than )
	{
		//if ( SETTINGS_FAST()->d_print_bundle_internal_externals )
		//{
		//	this_quality->d_actually_in_local_region_memory_resident_ids = this_quality->d_pVM->d_owner_region->serialize_local_memory_resident_node_output();
		//}

		this->dump( true , false );
	}
}

AbsoluteTime VMQuality::calculate_bundle_softdeadline()
{
	AbsoluteTime deadline = -1;
	if ( d_pAlias_bundled_nodes_ordered == NULL ) d_pAlias_bundled_nodes_ordered = d_pAllocated_bundled_nodes_ordered;
	assert( d_pAlias_bundled_nodes_ordered != NULL );
	for ( auto & node : *d_pAlias_bundled_nodes_ordered )
	{
		if ( deadline < 0 || node->d_task_deadline.d_task_soft_deadline_time > deadline )
		{
			deadline = node->d_task_deadline.d_task_soft_deadline_time;
		}
	}
	return deadline;
}

#pragma endregion

#pragma region VM_Leased_Cycle_Mgr

VM_Leased_Cycle_Mgr::~VM_Leased_Cycle_Mgr()
{
	cleanup();
}
void VM_Leased_Cycle_Mgr::cleanup()
{
	for ( auto & cycle : d_allocated_continuous_leased_cycles )
	{
		if ( cycle != NULL )
		{
			__DELETE( cycle );
			cycle = NULL;
		}
	}
	d_allocated_continuous_leased_cycles.clear();
}

Leased_Cycle * VM_Leased_Cycle_Mgr::check_point(bool request_by_register_just_last_point_or_whole_cycle ,
												const char * request_by , AbsoluteTime tnow , bool b_continue_used_flag )
{
	BEGIN_TRY
	AbsoluteTime current_time = SIMULATION_NOW_TIME;
	if ( tnow >= 0 )
	{
		current_time = tnow;
	}
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	Leased_Cycle * last_cycle = NULL; // back in empty list make memory curropt
	if ( d_allocated_continuous_leased_cycles.size() > 0 )
	{
		last_cycle = d_allocated_continuous_leased_cycles.back(); // last
	}
	if ( last_cycle != NULL )
	{
		if ( current_time < last_cycle->d_start_cycle_time )
		{
			XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
		}
	}
	////if ( !b_continue_cycle_anyway && last_cycle == NULL ) return;
	while ( d_allocated_continuous_leased_cycles.size() == 0 || current_time > last_cycle->d_end_cycle_time )
	{
		Leased_Cycle * new_cycle = __NEW( Leased_Cycle() );
		new_cycle->d_pAlias_CycleMgr = this;
		new_cycle->d_index = ( ( d_allocated_continuous_leased_cycles.size() == 0 ) ? 0 : last_cycle->d_index + 1 );
		new_cycle->d_prev_cycle = ( ( d_allocated_continuous_leased_cycles.size() == 0 ) ? NULL : last_cycle );
		new_cycle->d_start_cycle_time = ( ( d_allocated_continuous_leased_cycles.size() == 0 ) ? current_time : last_cycle->d_end_cycle_time );
		new_cycle->d_end_cycle_time = new_cycle->d_start_cycle_time + SETTINGS_FAST()->d_VM_LEASED_CYCLE_TIME_SEC;
		new_cycle->d_request_by += ( request_by_register_just_last_point_or_whole_cycle ? "" : request_by );
		
		if ( b_continue_used_flag && !d_pAliasVM->is_idle_vm() )
		{
			new_cycle->start_to_use_cycle();
		}
		d_allocated_continuous_leased_cycles.push_back( new_cycle );
		last_cycle = new_cycle;
	}
	if ( request_by_register_just_last_point_or_whole_cycle )
	{
		last_cycle->d_request_by += request_by;
	}

	if ( current_time < last_cycle->d_start_cycle_time || current_time > last_cycle->d_end_cycle_time )
	{
		XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
	}
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	return last_cycle;
	CATCH_TRY
	return NULL;
}

void VM_Leased_Cycle_Mgr::start_to_use( const VMQuality * this_quality )
{
	BEGIN_TRY
	check_point( true , this_quality->bundle_nodes_text().c_str() )->start_to_use_cycle();

	if ( SETTINGS_FAST()->d_dump_at_start_to_use_cycle_mgr_report )
	{
		XBT_INFO_EMPTY_LINE();
		XBT_INFO( "#(%d)(%s)" , __LINE__ , __FUNCTION__ );
		this->dump( true );
	}
	CATCH_TRY
}

void VM_Leased_Cycle_Mgr::end_use( const VMQuality * this_quality )
{
	BEGIN_TRY
	AbsoluteTime current_time = SIMULATION_NOW_TIME;

	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

	if ( d_allocated_continuous_leased_cycles.size() == 0 )
	{
		XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
	}
	check_point( false , this_quality->bundle_nodes_text().c_str() )->start_to_use_cycle();

	Leased_Cycle * iterate_cycle = NULL; // back in empty list make memory curropt
	if ( d_allocated_continuous_leased_cycles.size() > 0 ) // check bikhody
	{
		iterate_cycle = d_allocated_continuous_leased_cycles.back(); // last
	}

	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose , "#(%d) " , __LINE__ );
	DurationTime exec_quality_duration = *this_quality->d_pAllocated_bundled_nodes_ordered->back()->get_actual_complete_time() -
										*this_quality->d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time();
	Leased_Cycle_Used last_cycle_used; // temp
	last_cycle_used.d_use_finish_time = *this_quality->d_pAllocated_bundled_nodes_ordered->back()->get_actual_complete_time();
	last_cycle_used.d_use_start_time = *this_quality->d_pAllocated_bundled_nodes_ordered->back()->get_actual_complete_time();

	assert( iterate_cycle != NULL );
	while ( last_cycle_used.d_use_finish_time > iterate_cycle->d_end_cycle_time )
	{
		iterate_cycle = iterate_cycle->d_prev_cycle;
	}
	assert( iterate_cycle != NULL );

	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose , "#(%d) " , __LINE__ );
	// الان ته سیکل ها هستیم و سیکل های درگیر در اجرا را به ترتیب عکس یعنی آخر به اول می پیماییم و یوزد تخصیص می دهیم
	while ( exec_quality_duration > 0.0 )
	{
		Leased_Cycle_Used cycle_used;
		cycle_used.d_use_finish_time = last_cycle_used.d_use_start_time; // آیا اتمام قبلی نمی شود ابتدای بعدی
		cycle_used.d_use_start_time = std::max( cycle_used.d_use_finish_time - exec_quality_duration , ( iterate_cycle != NULL ) ? iterate_cycle->d_start_cycle_time : 0 );
		////cycle_used->d_used_to_exec_bundle = quality; // برای مصرف کمتر حافظه
		
		auto ttt = iterate_cycle->start_to_use_cycle();
		ttt->push_back( cycle_used );

		exec_quality_duration -= ( cycle_used.d_use_finish_time - cycle_used.d_use_start_time );
		last_cycle_used = cycle_used;
		if ( exec_quality_duration > 0.0 ) // 0.0 > 0.0 XXX
		{
			if ( iterate_cycle == NULL )
			{ 
				XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
			}
			else
			{
				iterate_cycle = iterate_cycle->d_prev_cycle;
				if ( iterate_cycle == NULL ) break;
			}
		}
	}
	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose , "#(%d) " , __LINE__ );

	if ( SETTINGS()->d_dump_at_end_use_cycle_mgr_report )
	{
		XBT_INFO_EMPTY_LINE();
		XBT_INFO( "#(%d)(%s)" , __LINE__ , __FUNCTION__ );
		this->dump( true );
	}
	CATCH_TRY
}

Leased_Cycle * VM_Leased_Cycle_Mgr::nowTimeIsInsideCycleOccupiedByOther()
{
	AbsoluteTime current_time = SIMULATION_NOW_TIME;
	if ( d_allocated_continuous_leased_cycles.size() > 0 )
	{
		Leased_Cycle * last_leased_cycle = d_allocated_continuous_leased_cycles.back();
		assert( current_time >= last_leased_cycle->d_start_cycle_time );
		if ( current_time >= last_leased_cycle->d_start_cycle_time && current_time < last_leased_cycle->d_end_cycle_time )
		{
			if ( last_leased_cycle->isUsed() )
			{
				return last_leased_cycle;
			}
		}
	}
	return NULL;
}

void VM_Leased_Cycle_Mgr::release_manager_for_deprovisioning()
{
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	//d_statistic_leased_cycles.insert( d_statistic_leased_cycles.end() , d_continuous_leased_cycles.begin() , d_continuous_leased_cycles.end() );
	if ( d_allocated_continuous_leased_cycles.size() > 0 )
	{
		this->check_point( false , "" );

		double used_total = 0;
		for ( auto & cyclee : d_allocated_continuous_leased_cycles )
		{
			double used_time = 0;
			if ( cyclee->isUsed() )
			{
				for ( auto & cycle_used : *cyclee->d_pAllocated_list_cycle_used )
				{
					used_time += cycle_used.used_time();
				}
			}
			assert( used_time <= cyclee->cycle_duration() );
			used_total += used_time;
		}

		d_leased_cycles_cost.push_back( make_tuple( d_allocated_continuous_leased_cycles.size() ,
													this->d_pAliasVM->d_vmType->d_type , 
													this->d_pAliasVM->d_vmType->d_dollar_cost_per_cycle ,
													SETTINGS_FAST()->d_VM_LEASED_CYCLE_TIME_SEC ,
													used_total ,
													this->d_pAliasVM->d_pAlias_owner_region->d_regionID ) );
		cleanup();
	}
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
}

void VM_Leased_Cycle_Mgr::release_manager_for_next_iteration()
{
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	cleanup();
	d_leased_cycles_cost.clear();
	//d_statistic_leased_cycles.clear();
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
}

void VM_Leased_Cycle_Mgr::dump( bool inside_another ) const
{
	BEGIN_TRY
	if ( !inside_another ) XBT_INFO_EMPTY_LINE();
	XBT_INFO( "dump_leasecyclemgr @vm(%d)" , this->d_pAliasVM->d_vm_uniq_id );

	for ( const auto & ref_leased_cycle : d_allocated_continuous_leased_cycles )
	{
		ref_leased_cycle->dump( true );
	}
	CATCH_TRY
}

//

void Leased_Cycle::dump( bool inside_another ) const
{
	BEGIN_TRY
	if ( !inside_another ) XBT_INFO_EMPTY_LINE();
	
	XBT_INFO( "dump_leasecycle @vm(%d) , |-(at %s s)---(at %s s)-| , %s" ,
				this->d_pAlias_CycleMgr->d_pAliasVM->d_vm_uniq_id , trim_floatnumber( d_start_cycle_time ).c_str()  , trim_floatnumber( d_end_cycle_time ).c_str() , d_request_by.c_str());
	if ( d_pAllocated_list_cycle_used != NULL )
	{
		for ( const auto & ref_leased_cycle_used : *d_pAllocated_list_cycle_used )
		{
			ref_leased_cycle_used.dump( true );
		}
	}
	CATCH_TRY
}

//

void Leased_Cycle_Used::dump( bool inside_another ) const
{
	BEGIN_TRY
	if ( !inside_another ) XBT_INFO_EMPTY_LINE();
	XBT_INFO( "dump_leasecycleuse , |-(at %s s)--dif(%s s)--(at %s s)-|" ,
			  trim_floatnumber( d_use_start_time ).c_str() , trim_floatnumber( d_use_finish_time - d_use_start_time ).c_str() ,
			  trim_floatnumber( d_use_finish_time ).c_str() /* , ( d_used_to_exec_bundle != NULL ) ? d_used_to_exec_bundle->bundle_nodes_text().c_str() : ""*/);
	CATCH_TRY
}

#pragma endregion

#pragma region ClassVM

ClassVM::ClassVM()
{
	d_vm_uniq_id = -1;

	d_pVmHost = NULL;
	d_pVmActor = NULL;
	d_pVmActorKeeper = NULL;

	//d_pLink2GlobalStorage = NULL;
	//d_pVmDisk = NULL;

	d_pAlias_owner_region = NULL;

	//d_pLastContainer = NULL;

	d_vmType = NULL;

	////d_idle_has_container_has_input_before_deadline__count = 0;
	////d_idle_has_container_before_deadline__count = 0;
	////d_idle_before_deadline__count = 0;
	////d_new_vm_can_finish_before_deadline__count = 0;
	////d_new_vm_cant_finish_before_deadline__count = 0;

	////d_place_task_on_idle_slot_machine__count = 0;
	////d_place_task_on_new_cycle_machine__count = 0;
}

void ClassVM::init_vm( ClassVM * pVM , Vmm * pVmm , sg4::NetZone * srcpZone )
{
	BEGIN_TRY
	static int _vmID = 0;
	d_vm_uniq_id = ++_vmID;

	d_pLastContainer.cleanup();
	d_leased_cycle_mgr.d_pAliasVM = pVM;

	d_pVmHost = srcpZone->create_host( string_format( "h%02d" , d_vm_uniq_id ) , /*d_vmType->d_cpu_speed*/ 0);
	//d_pVmHost->set_core_count( 1 );
	d_pVmHost->turn_off();

	////static int _linkid = 0;
	////d_pLink2GlobalStorage = srcpZone->create_split_duplex_link
	////(
	////	string_format( "lnkvs:%d" , ++_linkid ) , 0/*d_vmType->d_wan_network_speed*/
	////)->set_latency( 0 /*d_vmType->d_network_latency*/ );
	////srcpZone->add_route
	////(
	////	pVmm->d_pVmmGlobalStorageHost->get_netpoint() , d_pVmHost->get_netpoint() , nullptr , nullptr ,
	////    { {d_pLink2GlobalStorage, sg4::LinkInRoute::Direction::UP} } , true
	////);
	////if ( !SETTINGS_FAST()->d_use_EPSM_algorithm )
	////{
	////	for ( const shared_ptr< ClassVM > & vm : pVmm->d_available_VMs )
	////	{
	////		if ( vm->d_owner_region->d_regionID == this->d_owner_region->d_regionID )
	////		{
	////			auto * link2otherhost =
	////				srcpZone->create_split_duplex_link
	////				(
	////					string_format( "lnkvs:%d" , ++_linkid ) ,
	////					SETTINGS_FAST()->d_lan_bandwidth
	////				)->set_latency( 0 /*d_vmType->d_network_latency*/ );
	////			srcpZone->add_route
	////			(
	////				vm->d_pVmHost->get_netpoint() , d_pVmHost->get_netpoint() , nullptr , nullptr ,
	////				{ {link2otherhost, sg4::LinkInRoute::Direction::UP} } , true
	////			);
	////		}
	////	}
	////}
	//// create disk
	////d_pVmDisk = d_pVmHost->create_disk( string_format( "diskvm:%d" , d_vm_uniq_id ) , 0 , 0 /*d_vmType->d_memory_access_speed , d_vmType->d_memory_access_speed*/ )->seal();

	CATCH_TRY
}

void ClassVM::configureVM_ForProvisioning( VMQuality * pQuality )
{
	BEGIN_TRY
	assert( d_pVmActor == NULL );
	assert( d_pLastContainer.allocated_ptr == NULL );
	assert( d_vmType == NULL );
	d_pVmActor = NULL; // to insure
	d_pLastContainer.cleanup();

	assert( pQuality->d_vm_type != NULL);
	assert( pQuality->d_pAliasVmOwnerRegion != NULL );
	assert( pQuality->d_pAliasVM != NULL );
	assert( pQuality->d_pAliasVM->d_vm_uniq_id >= 0 );
	assert( pQuality->d_pAliasVM->d_vmType == NULL );
	assert( pQuality->d_pAliasVM->d_pAlias_owner_region == NULL );

	pQuality->d_pAliasVM->d_vmType = pQuality->d_vm_type;
	pQuality->d_pAliasVM->d_pAlias_owner_region = pQuality->d_pAliasVmOwnerRegion;
	pQuality->d_pAliasVmOwnerRegion->d_alias_local_vms.push_back(pQuality->d_pAliasVM);

	std::get<0>( get_vmm()->d_statistics[AeStatisticElement::stt_online_vm_count][STATISTICS_INT(this->d_vmType->d_type)].online_vm_count) += 1; // leased

	if ( std::get<0>( get_vmm()->d_statistics[ AeStatisticElement::stt_online_vm_count ][ STATISTICS_INT(this->d_vmType->d_type) ].online_vm_count ) >
		 std::get<2>( get_vmm()->d_statistics[ AeStatisticElement::stt_online_vm_count ][ STATISTICS_INT(this->d_vmType->d_type) ].online_vm_count ) )
	{
		std::get<2>( get_vmm()->d_statistics[ AeStatisticElement::stt_online_vm_count ][ STATISTICS_INT(this->d_vmType->d_type) ].online_vm_count ) =
			std::get<0>( get_vmm()->d_statistics[ AeStatisticElement::stt_online_vm_count ][ STATISTICS_INT(this->d_vmType->d_type) ].online_vm_count ); // max leased
	}

	get_vmm()->d_statistic_leased_vm[ this->d_pAlias_owner_region->d_regionID ][ this->d_vmType->d_type ] += 1;

	get_vmm()->d_online_vm_count++;
	if ( get_vmm()->d_online_vm_count > get_vmm()->d_max_online_vm_count )
	{
		get_vmm()->d_max_online_vm_count = get_vmm()->d_online_vm_count;
		if ( get_vmm()->d_max_online_vm_count > 1200 )
		{
			cout << "max online vm count is : " << get_vmm()->d_max_online_vm_count << endl;
		}
	}

	////d_pVmHost->set_pstate_speed(dspeed);
	////d_pLink2GlobalStorage->set_bandwidth( d_vmType->d_wan_network_speed );
	////d_pVmDisk->set_readwrite_bandwidth( d_vmType->d_memory_access_speed );

	d_pVmHost->turn_on();
	
	CATCH_TRY
}

void ClassVM::deprovisionVM()
{
	BEGIN_TRY
	assert( d_pVmActor == NULL );
	d_pVmActor = NULL; // to insure
	if ( d_pLastContainer.allocated_ptr != NULL )
	{
		d_pLastContainer.allocated_ptr->d_obsolete_container_time = SIMULATION_NOW_TIME;
	}
	d_pLastContainer.cleanup();
	d_leased_cycle_mgr.release_manager_for_deprovisioning();
	if ( get_vmm() != NULL ) get_vmm()->d_online_vm_count--;
	if ( d_pVmHost->is_on() )
	{
		////d_pVmHost->set_pstate( 0 );
		////d_pLink2GlobalStorage->set_bandwidth( 0 );
		////d_pVmDisk->set_readwrite_bandwidth( 0 );

		d_pVmHost->turn_off();
	}
	if ( this->d_pAlias_owner_region != NULL )
	{
		// یکی از تعداد این نوع ماشین اجاره شده کم می کند
		std::get<0>( this->get_vmm()->d_statistics[ AeStatisticElement::stt_online_vm_count ][ STATISTICS_INT( this->d_vmType->d_type ) ].online_vm_count ) =
			max( std::get<0>( this->get_vmm()->d_statistics[ AeStatisticElement::stt_online_vm_count ][ STATISTICS_INT( this->d_vmType->d_type ) ].online_vm_count ) - 1 , 0 );
	}

	d_vmType = NULL;
	if ( this->d_pAlias_owner_region != NULL )
	{
		ClassVM * pThis = this;
		this->d_pAlias_owner_region->d_alias_local_vms.erase(
			std::remove_if(
				this->d_pAlias_owner_region->d_alias_local_vms.begin() ,
				this->d_pAlias_owner_region->d_alias_local_vms.end() ,
				[ & ]( ClassVM * vm ) noexcept
				{
					return vm == pThis;
				} ) ,
			this->d_pAlias_owner_region->d_alias_local_vms.end() );
		this->d_pAlias_owner_region = NULL;
	}

	CATCH_TRY
}

//SectionTimeKeeper * _pSectionTimeKeeper = NULL;
//#define SECTION_TIME( comment ) SectionTime CONCAT_NAME_( instance , __LINE__ )( _pSectionTimeKeeper , __LINE__ , comment );

bool ClassVM::calculate_quality_ratio_can_fit_in_idle_slot( VMQuality * ref_in_out_quality )
{
	BEGIN_TRY

	//this->d_owner_region->d_pGlobalVMM->d_pIteration->pDGG->d_in_each_sort_try_schedule++;
	this->get_vmm()->d_total_try_quality++; // برای بررسی سرعت پیشزفت کار
	//this->d_owner_region->d_pGlobalVMM->d_last_subbundle_try_quality++;

	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	if ( !SETTINGS()->d_use_wait_policy && !this->is_idle_vm() )
	{
		//ref_in_out_quality->d_ret_message = "vm is not idle";
		return false;
	}
	////ref_in_out_quality->d_occupied_vm = !this->is_idle_vm();

	bool b_stablish_new_container_if_this_quality_win = false;
	if
	((
		!this->is_provisioned() ||
		( this->d_pLastContainer.allocated_ptr->d_container_dag_config.d_dag_type != ref_in_out_quality->d_quality_dag_config.d_dag_type )
	))
	{
		b_stablish_new_container_if_this_quality_win = true;
	}

	// sorat kasr
	// ResidentInput  یعنی روی ماشینی زمانبندی شود که قبلا پدر های بیشتری را اجرا کرده باشد
	// import input یعنی ماشینی که کمترین پدر را دارد و لازم است همه را دریافت کند
	{
		//SECTION_TIME("");
		//map< eNodeID , DAGNode * > resident_input_nodes;
		map< eNodeID , DAGNode * > need_to_transfer_input_nodes;
		for ( const auto & ref_bundle_node : *ref_in_out_quality->d_pAlias_bundled_nodes_shuffled )
		{
			for ( auto & ref_bundle_node_parent : ref_bundle_node.second->d_parents )
			{
				// اگر نود پدر در باندل بود کاری لازم نیست بکنی
				if ( ref_in_out_quality->d_pAlias_bundled_nodes_shuffled->count( ref_bundle_node_parent->d_node_uniq_id ) != 0 ) continue;

				// اگر روی ماشین هست که خیلی هم عالیه
				if ( !b_stablish_new_container_if_this_quality_win &&
					 ref_in_out_quality->d_pAliasVM->d_pLastContainer.allocated_ptr != NULL )
				{
					// agar bana bashad container taghir nakonad va node mohasebati input khod ra dar container yaft in data resident behesab miaayad
					if ( ref_in_out_quality->d_pAliasVM->d_pLastContainer.allocated_ptr->d_in_ram_task_output.count( ref_bundle_node_parent->d_node_uniq_id ) != 0 ) // agar parent dar hamin container exec shodeh bood
					{
						//resident_input_nodes[ ref_bundle_node_parent->d_node_uniq_id ] = ref_bundle_node_parent;
						ref_in_out_quality->d_estimate_kasr_TotalResidentInputs_size_byte += ref_bundle_node_parent->d_task_output_size_byte;
						// علت استفاده از مپ حذف موارد تکراری است
						// دیگر موارد تکراری را حذف نمب کنم چرا
						// در ضمن متغیر هایی که در نامشان کسر نوشته شده را چک کنم جایی برای محاسبه هزینه و زمان استفاده نکنم
						// زیرا موارد تکراری در نظر می گیرد فقط برای شاخص کووالیتی
						// اگر پدر دو تا نود داخل باندل در ماشینی اجرا شده خب چه بهتر
						// روی ماشینی اجرا کنیم که چند تا فرزند را پوشش می دهد خب خوبه دیگه
						continue;
					}
				}
				// اگر روی ماشین هم نیست خب باید از بیرون بیاید
				// سوال . آیا ماشینی که دو تا فرزند دنبال یک پدر بگردند بهتر است و یا ماشینی که یک فرزند دنبال پدر بگردد
				// این سوال کمک می کند ببینم آیا تکراری را حذف کنم یا نه
				// اگر باندل روی ماشینی باشد که پدر های بیشتری اجرا شده این باندل حال می کند
				// اگر باندل روی ماشینی قرار بگیرد که پدر های کمتری دارد هر کدام اینها زجر می کشند پس نبود پدر تکراری زجر بیشتر می دهد آیا
				// پس شد نبود پدر بیشتر زجر بیشتر نمی دهد چون یک بار منتقل میشه دیگه پس مپ برای این حالت مفید است
				////if ( ref_in_out_quality->d_quality_bundled_nodes_shuffled->count( ref_bundle_node_parent->d_node_uniq_id ) == 0 )
				if ( need_to_transfer_input_nodes.count( ref_bundle_node_parent->d_node_uniq_id ) == 0 )
				{
					need_to_transfer_input_nodes[ ref_bundle_node_parent->d_node_uniq_id ] = ref_bundle_node_parent;
				}
			}
		}
		////for ( const pair< eNodeID , shared_ptr< DAGNode > > & ref_resident_input_node : resident_input_nodes )
		////{
		////	ref_in_out_quality->d_estimate_kasr_TotalResidentInputs_size_byte += ref_resident_input_node.second->d_task_output_size_byte;
		////}
		for ( const auto & ref_need_to_transfer_input_node : need_to_transfer_input_nodes )
		{
			ref_in_out_quality->d_estimate_kasr_TotalTransferedInput_size_byte += ref_need_to_transfer_input_node.second->d_task_output_size_byte;
		}
	}

	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	// sorat kasr
	// internal edge یال بین خودی در باندل
	{
		//SECTION_TIME("");
		////map< eNodeID , shared_ptr< DAGNode > > inner_bundle_edge;
		for ( const auto & ref_bundle_node : *ref_in_out_quality->d_pAlias_bundled_nodes_shuffled )
		{
			for ( const auto & ref_bundle_node_parent : ref_bundle_node.second->d_parents )
			{
				if ( ref_in_out_quality->d_pAlias_bundled_nodes_shuffled->count( ref_bundle_node_parent->d_node_uniq_id ) != 0 ) // list has node parent
				{
					// پدری که دارد به دو فرزند در باندل کمک می کند با اجراش پس کار را برایشان راحت می کند پس بهتر است
					// پس تکراری حذف نشود
					// پدر پر شاخه هم با همه شاخه هاش در باندل باشد انتقال دیتا از خافطه داخلی است و بعتر است به تعداد شاخه ها
					////inner_bundle_edge[ ref_bundle_node_parent->d_node_uniq_id ] = ref_bundle_node_parent;
					ref_in_out_quality->d_estimate_kasr_TotalInternalEdgeWeight_size_byte += ref_bundle_node_parent->d_task_output_size_byte;
				}
			}
		}
		////for ( const pair< eNodeID , shared_ptr< DAGNode > > & ref_inner_bundle_edge : inner_bundle_edge )
		////{
		////	ref_in_out_quality->d_estimate_kasr_TotalInternalEdgeWeight_size_byte += ref_inner_bundle_edge.second->d_task_output_size_byte; // output parent = edge weight ertebaty farz shodeh
		////}
	}

	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	// sorat kasr
	// external edge
	{
		//SECTION_TIME("");
		map< eNodeID , DAGNode * > export_bundle_node;
		for ( const auto & ref_bundle_node : *ref_in_out_quality->d_pAlias_bundled_nodes_shuffled )
		{
			if ( ref_bundle_node.second->d_links.size() == 0 )
			{
				////ref_in_out_quality->d_estimate_kasr_TotalExternalEdgeWeight_size_byte += ref_bundle_node.second->d_task_output_size_byte;
				if ( export_bundle_node.count( ref_bundle_node.second->d_node_uniq_id ) == 0 )
				{
					export_bundle_node[ ref_bundle_node.second->d_node_uniq_id ] = ref_bundle_node.second;
				}
			}
			for ( const auto & ref_external_bundle_edge : ref_bundle_node.second->d_links )
			{
				if ( ref_in_out_quality->d_pAlias_bundled_nodes_shuffled->count( (ref_external_bundle_edge->d_pDestination)->d_node_uniq_id ) == 0 ) // list does not have node childs
				{
					// beh ezayeh har child i keh dar list nist yek khoroji bayad beravad baadan
					if ( export_bundle_node.count( ref_bundle_node.second->d_node_uniq_id ) == 0 )
					{
						export_bundle_node[ ref_bundle_node.second->d_node_uniq_id ] = ref_bundle_node.second;
					}
					// وقتی از یک جایی برش می خورد که خیلی خروجی دارد یعنی بعدا زحمت بیشتری درست می شود و ما اینرا می خواهیم کم کنیم
					// پس در متغیر زیر معناش اینست که نود با خروجی زیاد تاثیر مکرر دارد سختی کار و ما می خواهیم از نود های کم خروجی برش بزنیم
					ref_in_out_quality->d_estimate_kasr_TotalExternalEdgeWeight_size_byte += ref_bundle_node.second->d_task_output_size_byte;
				}
			}
		}
		for ( const auto & ref_export_bundle_node : export_bundle_node )
		{
			// ما به ازای هر خروجی فقط یک بار روی حافظه عمومی می نویسیم ولی به تعداد لینک ها یال خروجی داریم که این تفاوت منطق چند خط بالا تر و این خط است
			ref_in_out_quality->d_estimate_TotalExportNodeOutputToGS_size_byte += ref_export_bundle_node.second->d_task_output_size_byte; // output parent = edge weight ertebaty farz shodeh
		}
	}
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	//// makhraj kasr
	{
		//// cost and time
		{
			//// calc total proc flops . task size . output 2 VMmem
			{
				////SECTION_TIME("");
				for ( const auto & ref_bundle_node : *ref_in_out_quality->d_pAlias_bundled_nodes_shuffled )
				{
					ref_in_out_quality->d_estimate_TotalProcessFlops += ref_bundle_node.second->d_task_cpu_usage_flops;
					ref_in_out_quality->d_estimate_TotalTaskSize_byte += ref_bundle_node.second->d_task_size_byte;
					ref_in_out_quality->d_estimate_TotalOutputToVMMemory_size_byte += ref_bundle_node.second->d_task_output_size_byte;
				}

				if ( SETTINGS_FAST()->d_echo__calculate_quality_ratio__value_chack )
				{
					XBT_INFO( "TotalProcessFlops(%f)" , ref_in_out_quality->d_estimate_TotalProcessFlops );
					XBT_INFO( "TotalTaskSize(%f)" , ref_in_out_quality->d_estimate_TotalTaskSize_byte );
					XBT_INFO( "TotalOutputToVMMemory(%f)" , ref_in_out_quality->d_estimate_TotalOutputToVMMemory_size_byte );
				}
			}
			//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
			//// calculate input transfered
			{
				//SECTION_TIME("");
				map< eNodeID , DAGNode * > parent_input_nodes;
				// make uniq list
				{
					////SECTION_TIME("");
					for ( const auto & ref_bundle_node : *ref_in_out_quality->d_pAlias_bundled_nodes_shuffled )
					{
						for ( auto & ref_bundle_node_parent : ref_bundle_node.second->d_parents )
						{
							if ( parent_input_nodes.count( ref_bundle_node_parent->d_node_uniq_id ) == 0 &&
								 ref_in_out_quality->d_pAlias_bundled_nodes_shuffled->count( ref_bundle_node_parent->d_node_uniq_id ) == 0 )
							{
								parent_input_nodes[ ref_bundle_node_parent->d_node_uniq_id ] = ref_bundle_node_parent;
							}
						}
					}
				}
				// یافتن نود های مستقر و نود های غیر مستقر و منتقل شونده
				for ( const auto & ref_parent_node : parent_input_nodes )
				{
					// in the bundle
					{
						////SECTION_TIME("");
						if ( ref_in_out_quality->d_pAlias_bundled_nodes_shuffled->count( ref_parent_node.second->d_node_uniq_id ) != 0 )
						{
							////if ( SETTINGS_FAST()->d_print_bundle_internal_externals )
							////{
							////	ref_in_out_quality->d_estimate_local_bundle_parent_ids += string_format( "(%d)" , ref_parent_node.first );
							////}
							continue;
						}
					}

					//// in local vm memory . in no exec vm status
					////if ( !b_stablish_new_container_if_this_quality_win &&
					////		ref_in_out_quality->d_pVM->d_pLastContainer != NULL &&
					////		ref_in_out_quality->d_pVM->d_pLastContainer->d_pAssignedQuality != NULL ) // check quality on container with vm binded
					////{
					////	if ( ref_in_out_quality->d_pVM->d_pLastContainer->d_pAssignedQuality->d_pTemp_bundled_nodes_shuffled->count( ref_parent_node.second->d_node_uniq_id ) != 0 )
					////	{
					////		continue;
					////	}
					////} این خط ها امکان ندارد زیرا باندلی دارد اجرا می شود در ماشین و در این باندل پدر باندل فعلی وجود دارد و چون پدر اجرا نشده فرزند هیچ وقت در اسکچول قرار نمی گیرد

					// in local vm memory . in no exec vm status
					{
						////SECTION_TIME("");
						if ( !b_stablish_new_container_if_this_quality_win &&
							 ref_in_out_quality->d_pAliasVM->d_pLastContainer.allocated_ptr != NULL )
						{
							// agar bana bashad container taghir nakonad va node mohasebati input khod ra dar container yaft in data resident behesab miaayad
							if ( ref_in_out_quality->d_pAliasVM->d_pLastContainer.allocated_ptr->d_in_ram_task_output.count( ref_parent_node.second->d_node_uniq_id ) != 0 ) // agar parent dar hamin container exec shodeh bood
							{
								if ( SETTINGS_FAST()->d_print_bundle_internal_externals )
								{
									ref_in_out_quality->d_estimate_local_memory_parent_ids += string_format( "(%d)" , ref_parent_node.first );
								}
								continue;
							}
						}
					}
					
					// in local region vm memory
					{
						////SECTION_TIME("");
						vector< ClassVM * > lst_local_region_input_transfer_time;
						{
							for ( auto & probable_vm : ref_parent_node.second->d_pRuntime->d_probable_keeper_vm )
							{
								if ( probable_vm.second->is_provisioned() &&
									 probable_vm.second->d_pLastContainer.allocated_ptr != NULL &&
									 probable_vm.second->d_pLastContainer.allocated_ptr->d_obsolete_container_time < 0 &&
									 probable_vm.second->d_pLastContainer.allocated_ptr->d_container_uniq_id == probable_vm.first && // همچنان در ماشین همان کانتینر بود
									 probable_vm.second->d_vm_uniq_id != this->d_vm_uniq_id &&
									 probable_vm.second->d_pAlias_owner_region->d_regionID == this->d_pAlias_owner_region->d_regionID && // خوبه لوکال رجیون اینجا چک شده
									 probable_vm.second->d_pLastContainer.allocated_ptr->d_in_ram_task_output.count( ref_parent_node.second->d_node_uniq_id ) != 0
									)
								{
									assert( SETTINGS_FAST()->d_all_machine_type_memory_access_speed > 0 );
									lst_local_region_input_transfer_time.push_back( probable_vm.second );
									break;
								}
							}

							////for ( shared_ptr< ClassVM > & local_region_vm : ref_in_out_quality->d_pVM->d_owner_region->d_local_vms )
							////{
							////	if ( local_region_vm->is_provisioned() && local_region_vm->d_vm_uniq_id != ref_in_out_quality->d_pVM->d_vm_uniq_id )
							////	{
							////		////if ( local_region_vm->d_pLastContainer->d_pAssignedQuality != NULL && // از قبل اجرا داره
							////		////	 local_region_vm->d_pLastContainer->d_pAssignedQuality->d_pTemp_bundled_nodes_shuffled->count( ref_parent_node.second->d_node_uniq_id ) != 0 )
							////		////{
							////		////	lst_local_region_input_transfer_time.push_back( local_region_vm );
							////		////	// همه ماشین های لوکال که دیتا دارند لیست شوند بهترین ماشین ماشینی است که سرعت خواندن بالاتری دارد از حافظه
							////		//// امکان نداره پدری در حال اجرا باشه و فرزند هم بیاید برای اجرا
							////		////}
							////		// این شرط پایین په شرط بیخودیه
							////		// اومدیم یکی در حال کاری در عین حال در حافظش دیتای مورد نیاز ما را دارد
							////		// در ضمن اون پدر حتما پروسس شده که من اجازه داشتم در باندل اضافه شوم
							////		if ( //local_region_vm->d_pLastContainer->d_pAssignedQuality == NULL && // شاید الان اجرای دیگری داشته باشد و همچنان دیتای قبلی را نگه داشته باشد
							////			 local_region_vm->d_pLastContainer->d_in_ram_task_output.count( ref_parent_node.second->d_node_uniq_id ) != 0 )
							////		{
							////			lst_local_region_input_transfer_time.push_back( local_region_vm );
							////			// همه ماشین های لوکال که دیتا دارند لیست شوند بهترین ماشین ماشینی است که سرعت خواندن بالاتری دارد از حافظه
							////		}
							////	}
							////}
						}
						////SECTION_TIME("");
						if ( lst_local_region_input_transfer_time.size() > 0 ) // can retrive input from local region machine
						{
							std::sort( lst_local_region_input_transfer_time.begin() , lst_local_region_input_transfer_time.end() , []( ClassVM *& a , ClassVM *& b )
							{
								return a->d_vmType->d_memory_access_speed > b->d_vmType->d_memory_access_speed; // اونیکه دسترسی حافظه سریعتری داره . زودتر لود میکنه
							} );

							ref_in_out_quality->d_estimate_TotalTransferedInputFromLocalRegionSize_byte += ref_parent_node.second->d_task_output_size_byte;
							ref_in_out_quality->d_estimate_TotalTransfered_ReadMemory_InputFromLocalRegionTimeConsume +=
								lst_local_region_input_transfer_time.front()->d_vmType->get_duration_datatransfer_on_mem( ref_parent_node.second->d_task_output_size_byte );
							if ( SETTINGS_FAST()->d_print_bundle_internal_externals )
							{
								ref_in_out_quality->d_estimate_local_region_parent_ids += string_format( "(%d)" , ref_parent_node.first );
							}

							continue;
						}
					}

					// in global storage memroy
					{
						////SECTION_TIME("");
						if ( this->get_vmm()->d_in_GS_output.count( ref_parent_node.second->d_node_uniq_id ) == 0 )
						{
							XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
						}
						else
						{
							ref_in_out_quality->d_estimate_TotalTransferedInputFromGlobalStorageSize_byte += ref_parent_node.second->d_task_output_size_byte;
							if ( SETTINGS_FAST()->d_print_bundle_internal_externals )
							{
								ref_in_out_quality->d_estimate_globally_parent_ids += string_format( "(%d)" , ref_parent_node.first );
							}
						}
					}
				} // for

				////SECTION_TIME("");

				if ( SETTINGS_FAST()->d_echo__calculate_quality_ratio__value_chack )
				{
					XBT_INFO( "TotalTransferedInputFromLocalRegionSize(%f)" , ref_in_out_quality->d_estimate_TotalTransferedInputFromLocalRegionSize_byte );
					XBT_INFO( "TotalTransfered_ReadMemory_InputFromLocalRegionTimeConsume(%f)" , ref_in_out_quality->d_estimate_TotalTransfered_ReadMemory_InputFromLocalRegionTimeConsume );
					XBT_INFO( "TotalTransferedInputFromGlobalStorageSize(%f)" , ref_in_out_quality->d_estimate_TotalTransferedInputFromGlobalStorageSize_byte );
				}

				ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromLocalRegionTimeConsume +=
					ref_in_out_quality->d_pAliasVM->d_vmType->get_duration_datatransfer_on_lan( ref_in_out_quality->d_estimate_TotalTransferedInputFromLocalRegionSize_byte );

				ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume +=
					ref_in_out_quality->d_pAliasVM->d_vmType->get_duration_datatransfer_on_wan( ref_in_out_quality->d_estimate_TotalTransferedInputFromGlobalStorageSize_byte );

				if ( SETTINGS_FAST()->d_echo__calculate_quality_ratio__value_chack )
				{
					XBT_INFO( "TotalTransfered_OnNetwork_InputFromLocalRegionTimeConsume(%f)" , ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromLocalRegionTimeConsume );
					XBT_INFO( "TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume(%f)" , ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume );
				}

				if ( ref_in_out_quality->d_estimate_kasr_TotalTransferedInput_size_byte !=
					 ref_in_out_quality->d_estimate_TotalTransferedInputFromGlobalStorageSize_byte +
					 ref_in_out_quality->d_estimate_TotalTransferedInputFromLocalRegionSize_byte )
				{
					if ( SETTINGS_FAST()->d_warn_impossible_condition )
					{
						//XBT_INFO( "#(%d) assert . fault . error . impossible" , __LINE__ );
					}
				}
			}
			//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
			//// calculate cost
			if ( 1 )
			{
				////SECTION_TIME("");
				ref_in_out_quality->d_estimate_kasr_ProcessCost =
					this->d_vmType->get_execution_cost( ref_in_out_quality->d_estimate_TotalProcessFlops ); // IMPORTANT

				//if ( SETTINGS_FAST()->d_echo__calculate_quality_ratio__value_chack )
				//{
				//	XBT_INFO( "kasr_ProcessCost(%f) , TotalProcessFlops(%f) , cpu(%f) , d_dollar_cost_per_cycle(%f) , VM_LEASED_CYCLE_TIME_SEC(%f)" ,
				//			  ref_in_out_quality->d_estimate_kasr_ProcessCost , ref_in_out_quality->d_estimate_TotalProcessFlops , this->d_vmType->d_cpu_speed ,
				//			  this->d_vmType->d_dollar_cost_per_cycle , SETTINGS_FAST()->d_VM_LEASED_CYCLE_TIME_SEC );
				//}

				ref_in_out_quality->d_estimate_kasr_DataTransferCost = this->d_vmType->get_cost_duration( this->d_vmType->get_duration_datatransfer_on_wan(
																											ref_in_out_quality->d_estimate_TotalTaskSize_byte +
																											ref_in_out_quality->d_estimate_TotalExportNodeOutputToGS_size_byte ) +
																											ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromLocalRegionTimeConsume +
																											ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume );

				if ( SETTINGS_FAST()->d_echo__calculate_quality_ratio__value_chack )
				{
					XBT_INFO( "kasr_DataTransferCost(%f) , TotalTaskSize(%f) , kasr_TotalExternalEdgeWeight(%f) , d_wan_network_speed(%f) , FromLocalRegion(%f) , FromGlobal(%f)" ,
							  ref_in_out_quality->d_estimate_kasr_DataTransferCost , ref_in_out_quality->d_estimate_TotalTaskSize_byte , ref_in_out_quality->d_estimate_kasr_TotalExternalEdgeWeight_size_byte ,
							  this->d_vmType->d_wan_network_speed , ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromLocalRegionTimeConsume ,
							  ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume );
				}

				ref_in_out_quality->d_estimate_TotalCost += ref_in_out_quality->d_estimate_kasr_ProcessCost;
				ref_in_out_quality->d_estimate_TotalCost += ref_in_out_quality->d_estimate_kasr_DataTransferCost;

				ref_in_out_quality->d_estimate_kasr_AllocationNewMachineCost = 0; // IMPORTANT
				ref_in_out_quality->d_estimate_kasr_ContainerStablishmentCost = 0; // IMPORTANT

				// calculate time and cost
				ref_in_out_quality->d_estimate_TotalTimeConsume = 0; // IMPORTANT

				// warm up vm time
				if ( !this->is_provisioned() ) // is new
				{
					ref_in_out_quality->d_stablish_vm = true;
					// time
					ref_in_out_quality->d_estimate_VirtualMachine_Warmup_time_consume = SETTINGS_FAST()->d_VIRTUALMACHINE_WARMUP_TIME_SEC;
					ref_in_out_quality->d_estimate_TotalTimeConsume += ref_in_out_quality->d_estimate_VirtualMachine_Warmup_time_consume;
					// cost
					ref_in_out_quality->d_estimate_kasr_AllocationNewMachineCost = this->d_vmType->get_cost_duration( ref_in_out_quality->d_estimate_VirtualMachine_Warmup_time_consume );
					ref_in_out_quality->d_estimate_TotalCost += ref_in_out_quality->d_estimate_kasr_AllocationNewMachineCost;
				}

				// load container structure on vm time
				// new vm or dag type changed
				if ( b_stablish_new_container_if_this_quality_win )
				{
					ref_in_out_quality->d_stablish_container = true;
					// time
					ref_in_out_quality->d_estimate_StablishContainer_time_consume = ref_in_out_quality->d_quality_dag_config.d_container_initialization_time;
					ref_in_out_quality->d_estimate_TotalTimeConsume += ref_in_out_quality->d_estimate_StablishContainer_time_consume;
					// cost
					ref_in_out_quality->d_estimate_kasr_ContainerStablishmentCost = this->d_vmType->get_cost_duration( ref_in_out_quality->d_estimate_StablishContainer_time_consume ); // time consume cost for transfer container and write it
					ref_in_out_quality->d_estimate_TotalCost += ref_in_out_quality->d_estimate_kasr_ContainerStablishmentCost;
				}

				// اگر قرار است یک باندل سیاست توفف برایش جوای بدهد باید کل زمانی که لازم دارد به علاوه زمان تا انتهای اجرا شده قبلی زمان داشته باشد
			}
			//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
			////calculate time
			{
				////SECTION_TIME("");
				if ( SETTINGS_FAST()->d_echo__calculate_quality_ratio__value_chack )
				{
					XBT_INFO( "d_TotalCost(%f)" , ref_in_out_quality->d_estimate_TotalCost );
				}

				
				ref_in_out_quality->d_estimate_read_tasks_from_global_storage_time_consume = ref_in_out_quality->d_estimate_TotalTaskSize_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;
				ref_in_out_quality->d_estimate_transfer_tasks_from_global_storage_to_vm_time_consume = d_vmType->get_duration_datatransfer_on_wan( ref_in_out_quality->d_estimate_TotalTaskSize_byte );
				ref_in_out_quality->d_estimate_write_tasks_to_vm_time_consume = d_vmType->get_duration_datatransfer_on_mem( ref_in_out_quality->d_estimate_TotalTaskSize_byte );

				ref_in_out_quality->d_estimate_read_parent_output_from_global_storage_memory_time_consume = ref_in_out_quality->d_estimate_TotalTransferedInputFromGlobalStorageSize_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;
				//// ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume
				ref_in_out_quality->d_estimate_write_output_parent_from_global_storage_on_vm = d_vmType->get_duration_datatransfer_on_mem( ref_in_out_quality->d_estimate_TotalTransferedInputFromGlobalStorageSize_byte );

				////ref_in_out_quality->d_estimate_TotalTransfered_ReadMemory_InputFromLocalRegionTimeConsume
				////ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromLocalRegionTimeConsume
				ref_in_out_quality->d_estimate_write_output_parent_from_local_region_on_vm = d_vmType->get_duration_datatransfer_on_mem( ref_in_out_quality->d_estimate_TotalTransferedInputFromLocalRegionSize_byte );

				ref_in_out_quality->d_estimate_exec_tasks_time_consume = d_vmType->get_execution_duration( ref_in_out_quality->d_estimate_TotalProcessFlops );

				ref_in_out_quality->d_estimate_write_output_to_vm_time_consume = d_vmType->get_duration_datatransfer_on_mem( ref_in_out_quality->d_estimate_TotalOutputToVMMemory_size_byte );
				ref_in_out_quality->d_estimate_transfer_output_to_global_storage_time_consume = d_vmType->get_duration_datatransfer_on_wan( ref_in_out_quality->d_estimate_TotalExportNodeOutputToGS_size_byte );
				ref_in_out_quality->d_estimate_write_output_to_global_storage_time_consume = ref_in_out_quality->d_estimate_TotalExportNodeOutputToGS_size_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;

				ref_in_out_quality->d_estimate_TotalTimeConsume += ref_in_out_quality->d_estimate_read_tasks_from_global_storage_time_consume +
					ref_in_out_quality->d_estimate_transfer_tasks_from_global_storage_to_vm_time_consume +
					ref_in_out_quality->d_estimate_write_tasks_to_vm_time_consume +
					ref_in_out_quality->d_estimate_read_parent_output_from_global_storage_memory_time_consume +
					ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume +
					ref_in_out_quality->d_estimate_write_output_parent_from_global_storage_on_vm +
					ref_in_out_quality->d_estimate_TotalTransfered_ReadMemory_InputFromLocalRegionTimeConsume +
					ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromLocalRegionTimeConsume +
					ref_in_out_quality->d_estimate_write_output_parent_from_local_region_on_vm +
					ref_in_out_quality->d_estimate_exec_tasks_time_consume +
					ref_in_out_quality->d_estimate_write_output_to_vm_time_consume +
					ref_in_out_quality->d_estimate_transfer_output_to_global_storage_time_consume +
					ref_in_out_quality->d_estimate_write_output_to_global_storage_time_consume;

				if ( SETTINGS_FAST()->d_print_bundle_internal_externals )
				{
					ref_in_out_quality->d_estimate_in_local_region_memory_resident_ids = this->d_pAlias_owner_region->serialize_local_memory_resident_node_output();
				}

				if ( SETTINGS_FAST()->d_echo__calculate_quality_ratio__value_chack )
				{
					XBT_INFO( "TotalTimeConsume(%f s)" , ref_in_out_quality->d_estimate_TotalTimeConsume );
				}
			}
			//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
			//// wait policy
			if ( ref_in_out_quality->d_pAliasVM->is_idle_vm() == false )
			{

				////SECTION_TIME("");
				ref_in_out_quality->d_occupied_vm = true; // existence of actor and quality is same thing

				assert( ref_in_out_quality->d_pAliasVM->d_pLastContainer.allocated_ptr != NULL );
				assert( ref_in_out_quality->d_pAliasVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr != NULL );
				assert( ref_in_out_quality->d_pAliasVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr->d_pAllocated_bundled_nodes_ordered != NULL );
				assert( ref_in_out_quality->d_pAliasVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr->d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() != NULL );
				////assert( *ref_in_out_quality->d_pVM->d_pLastContainer->d_pAssignedQuality->d_quality_bundled_nodes_ordered->front()->get_actual_complete_time().get() >= 0);
				//// time
				ref_in_out_quality->d_estimate_finished_prev_running_job_time_consume = std::max( (
					*ref_in_out_quality->d_pAliasVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr->d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() +
					ref_in_out_quality->d_pAliasVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr->d_estimate_TotalTimeConsume ) -
					SIMULATION_NOW_TIME , 0.0 );

				//XBT_INFO( "finished_prev_running_job_time_consume(%f)" , ref_in_out_quality->d_estimate_finished_prev_running_job_time_consume );

				//if ( ref_in_out_quality->d_estimate_finished_prev_running_job_time_consume <= 0.0 )
				//{
				//	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
				//	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "%f" , SIMULATION_NOW_TIME );
				//	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "%f" , ref_in_out_quality->d_estimate_finished_prev_running_job_time_consume );
				//	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "%f" , *ref_in_out_quality->d_pVM->d_pLastContainer->d_pAssignedQuality->d_quality_bundled_nodes_ordered->front()->get_actual_triggered_time().get() );
				//	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "%f" , ref_in_out_quality->d_pVM->d_pLastContainer->d_pAssignedQuality->d_estimate_TotalTimeConsume );
				//	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "%f" , SETTINGS()->d_schedule_periode_delay_time - fmod( ref_in_out_quality->d_estimate_finished_prev_running_job_time_consume , SETTINGS()->d_schedule_periode_delay_time ) );
				//	ref_in_out_quality->d_pVM->d_pLastContainer->d_pAssignedQuality->dump( true , false );
				//}
				ref_in_out_quality->d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time =
					SETTINGS_FAST()->d_schedule_periode_delay_time -
					fmod( ref_in_out_quality->d_estimate_finished_prev_running_job_time_consume , SETTINGS_FAST()->d_schedule_periode_delay_time );

				// increase time and cost
				////ref_in_out_quality->d_estimate_TotalTimeConsume += ref_in_out_quality->d_estimate_finished_prev_running_job_time_consume +
				////	ref_in_out_quality->d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time;

				////ref_in_out_quality->d_estimate_TotalCost += ( ref_in_out_quality->d_estimate_finished_prev_running_job_time_consume +
				////											  ref_in_out_quality->d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time ) *
				////	this->d_vmType->d_dollar_cost_per_cycle / SETTINGS()->d_VM_LEASED_CYCLE_TIME_SEC;

				if ( ref_in_out_quality->d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time < 0.0 )
				{
					XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
				}

				if ( 1 )
				{
					AbsoluteTime t_absolute_after_complete_prev_job = SIMULATION_NOW_TIME + ref_in_out_quality->d_estimate_finished_prev_running_job_time_consume +
						ref_in_out_quality->d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time;

					////shared_ptr< Leased_Cycle > last_leased_cycle = NULL; // لیست مشکل دارد اگر خالی باشد و ما آخریش رو بخواهیم مموری خراب می شود
					////if ( cloned_cycle_mgr->d_continuous_leased_cycles.size() > 0 ) // ماشینی که کارش شروع نشده نباید بخاطر صرفا دیدن باقیمانده دوره شروع بکار کند . شروع بکار با اولین تسک تخصیصی شروع می شود
					////{
						////last_leased_cycle = cloned_cycle_mgr->check_point( false , "" , t_after_complete_prev_job , true ); // after check size bashad

						////last_leased_cycle = d_leased_cycle_mgr.d_continuous_leased_cycles.back(); // last
						////if ( current_time < last_leased_cycle->d_start_cycle_time || current_time > last_leased_cycle->d_end_cycle_time )
						////{
						////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
						////}
					////}
					// TOCHECK . بررسی این کارم در پایین
					ref_in_out_quality->d_estimate_use_leased_cycle_cost = 0;
					ref_in_out_quality->d_estimate_use_new_leased_cycle_cost = 0;
					// محاسبه هزینه سیکل های مورد استفاده
					////if ( cloned_cycle_mgr->d_continuous_leased_cycles.size() > 0 ) // یعنی اگر ماشین چیزی اجرا کرده بود
					if ( ref_in_out_quality->d_pAliasVM->d_leased_cycle_mgr.d_allocated_continuous_leased_cycles.size() > 0 )
					{
						auto last_cycle = ref_in_out_quality->d_pAliasVM->d_leased_cycle_mgr.d_allocated_continuous_leased_cycles.back();
						////if ( last_leased_cycle->isUsed() )
						if ( t_absolute_after_complete_prev_job <= last_cycle->d_end_cycle_time ) // قطعا دول این سایکل پرداخت شده زیرا اصلا یکی هست که داره الان اجرا میشه روش
						{
							ref_in_out_quality->d_estimate_use_new_leased_cycle_cost =
								this->d_vmType->get_cost_duration( std::max( ref_in_out_quality->d_estimate_TotalTimeConsume - 
																			 ( last_cycle->d_end_cycle_time - t_absolute_after_complete_prev_job ) , 0.0 ) );
						}
						else
						{
							ref_in_out_quality->d_estimate_use_new_leased_cycle_cost =
								this->d_vmType->get_cost_duration( ref_in_out_quality->d_estimate_TotalTimeConsume );
						}
					}
					else
					{
						XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ ); // الان وسط اجراست چطور میشه سیکل نداشته باشه
					}
					ref_in_out_quality->d_estimate_use_leased_cycle_cost = this->d_vmType->get_cost_duration( ref_in_out_quality->d_estimate_TotalTimeConsume );

					if ( ref_in_out_quality->d_estimate_use_new_leased_cycle_cost > ref_in_out_quality->d_estimate_use_leased_cycle_cost )
					{
						XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ ); // قسمت مصرف شده از سیکل ها بیشتر از سیکل ها که نمیشه
					}

				}
			}
			//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
		}

		// complete leased cycle
		if ( ref_in_out_quality->d_occupied_vm == false )
		{
			////SECTION_TIME("");
			AbsoluteTime current_time = SIMULATION_NOW_TIME;
			////shared_ptr< Leased_Cycle > last_leased_cycle = NULL; // لیست مشکل دارد اگر خالی باشد و ما آخریش رو بخواهیم مموری خراب می شود
			////if ( d_leased_cycle_mgr.d_continuous_leased_cycles.size() > 0 ) // ماشینی که کارش شروع نشده نباید بخاطر صرفا دیدن باقیمانده دوره شروع بکار کند . شروع بکار با اولین تسک تخصیصی شروع می شود
			{
				////d_leased_cycle_mgr.check_point( false , "" ); // after check size bashad

				////last_leased_cycle = d_leased_cycle_mgr.d_continuous_leased_cycles.back(); // last
				////if ( current_time < last_leased_cycle->d_start_cycle_time || current_time > last_leased_cycle->d_end_cycle_time )
				////{
				////	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
				////}
			}
			ref_in_out_quality->d_estimate_use_leased_cycle_cost = 0;
			ref_in_out_quality->d_estimate_use_new_leased_cycle_cost = 0;

			auto last_leased_cycle = d_leased_cycle_mgr.nowTimeIsInsideCycleOccupiedByOther();

			// محاسبه هزینه سیکل های مورد استفاده
			if ( last_leased_cycle != NULL )
			{
				//XBT_INFO_ALIVE_CHECK( "#(%d) yesyes" , __LINE__ );
				// یعنی اگر وی ام اجاره شده از قبل
				////if ( last_leased_cycle->isUsed() ) // قبلا تسکی روی سیکل اجرا شده بود
				{
					// هر سیکل را یک باندل پولش را می دهد و ته باقیمانده تسک برای باندل بعدی مجانی است تا سعی کنیم باندل ها همیشه در اسلات های خالی زمانبندی گردد
					// اگر وسط یک سیکلی بودیم که یکی دیگه پولش را داده بود
					// هزینه استفاده از سیکل ها می شود سیکل های بعد از این سیکلی که توشیم
					ref_in_out_quality->d_estimate_use_new_leased_cycle_cost =
						this->d_vmType->get_cost_duration( std::max( ref_in_out_quality->d_estimate_TotalTimeConsume - ( last_leased_cycle->d_end_cycle_time - current_time ) , 0.0 ) );
				}
				////else // اینجا میگه یعنی شروع کار و الان وسط یک سیکلیم که هیچ تسکی روی آن اجرا نشده
				////{
				////	// اگر در یک سیکل جدید بودیم کلا حالا یا وسطشیم و یا اولش
				////	// هزینه استفاده از سیکل ها می شود تنهای قسمت مصرف ما از سیکل ها و و در سیکل ابتدا و انتها شاید بازه محاسبه نشده داشته باشم که از قصد است و صورت کسر را کاهش می دهد
				////	ref_in_out_quality->d_estimate_use_new_leased_cycle_cost =
				////		( ( ref_in_out_quality->d_estimate_TotalTimeConsume / SETTINGS_FAST()->d_VM_LEASED_CYCLE_TIME_SEC ) * d_vmType->d_dollar_cost_per_cycle );
				////}
				////ref_in_out_quality->d_total_leased_cycle_cost += d_vmType->d_dollar_cost_per_cycle; //  قیمت تا ته این سیکلی که توشیم
				////ref_in_out_quality->d_total_leased_cycle_cost +=
				////	std::ceil( std::max( ref_in_out_quality->TotalTimeConsume - ( last_leased_cycle->d_end_cycle_time - current_time ) , 0.0 ) / CNFG()->d_VM_LEASED_CYCLE_TIME_SEC ) * d_vmType->d_dollar_cost_per_cycle;
				// قیمت توتال مخرج کسر می شود سقف قیمت سیکل های جدید یعنی کلیه سیکل های درگیر به علاوه تک سیکلی که توش داریم اجرا می کنید
			}
			else
			{
				ref_in_out_quality->d_estimate_use_new_leased_cycle_cost =
					this->d_vmType->get_cost_duration( ref_in_out_quality->d_estimate_TotalTimeConsume );

				////ref_in_out_quality->d_total_leased_cycle_cost +=
				////	std::ceil( ref_in_out_quality->TotalTimeConsume / CNFG()->d_VM_LEASED_CYCLE_TIME_SEC ) * d_vmType->d_dollar_cost_per_cycle;
			}
			ref_in_out_quality->d_estimate_use_leased_cycle_cost = this->d_vmType->get_cost_duration( ref_in_out_quality->d_estimate_TotalTimeConsume );

			if ( ref_in_out_quality->d_estimate_use_new_leased_cycle_cost > ref_in_out_quality->d_estimate_use_leased_cycle_cost )
			{
				XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ ); // قسمت مصرف شده از سیکل ها بیشتر از سیکل ها که نمیشه
			}
		}
		//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	}

	if ( !SETTINGS_FAST()->d_use_free_slot_policy )
	{
		ref_in_out_quality->d_estimate_use_new_leased_cycle_cost = 0;
	}

	// barayeh yek cycle barname rizi mi konim keh chon dar vm az ghabl ejareh shodeh por karadan cycle manfeat darad va polash az ghabl pardakht shodeh
	// farz mi konam mohasebat baad az in barayeh digar bundle ha dar digar vm ha zamani nemi barad va zarti quality dar vm schedule mi shavad
	// اینجا فرضم اینست که اگر الان اسکچول شود روی ماشین با زمانی از کل ددلاین دگ در اختیارش است می تواند کارش را به موقع تمام کند
	// این کامنت بالا برای مورد سیاست توقف در سیکل فعلی خیلی مهم است
	
	if ( ref_in_out_quality->d_occupied_vm )
	{
		double bundle_least_vm_qouta_time = 0;
		for ( auto & node : *ref_in_out_quality->d_pAlias_bundled_nodes_ordered )
		{
			bundle_least_vm_qouta_time += node->d_task_deadline.d_task_quota_duration_based_on_process_need + node->d_task_deadline.d_task_spare_duration_quota;
		}
		double z = ( ref_in_out_quality->calculate_bundle_softdeadline() - ( SIMULATION_NOW_TIME +
																			 ref_in_out_quality->d_estimate_finished_prev_running_job_time_consume +
																			 ref_in_out_quality->d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time +
																			 ref_in_out_quality->d_estimate_TotalTimeConsume ) ) /
																			bundle_least_vm_qouta_time;

		////SECTION_TIME("");
		// اگر زمان اضافی های باندل جمعش باهم به اندازه زمان مورد نیاز تا دور بعد اسکچول باشه به نحوی که این ماشین آزاد بشه خوب میشه صبر کرد
		if ( z >= 0 )
		{
			switch ( SETTINGS_FAST()->d_wait_policy_ver )
			{
				case 1: // quality ration set to 1
				{
					_ONE_TIME_VAR();
					_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
					ref_in_out_quality->d_estimate_quality_ratio = 1;
					break;
				}
				case 2: // quality ration set to formula
				case 3: // quality ration set to formula and only first schedule node spare time consider for waiting
				case 4: // wait quality race with the normal quality about ratio value
				{
					ref_in_out_quality->d_estimate_quality_ratio =
						(	(
								std::max( ref_in_out_quality->d_estimate_kasr_TotalResidentInputs_size_byte + ref_in_out_quality->d_estimate_kasr_TotalInternalEdgeWeight_size_byte , 1.0 ) /
								std::max( ref_in_out_quality->d_estimate_kasr_TotalTransferedInput_size_byte + ref_in_out_quality->d_estimate_kasr_TotalExternalEdgeWeight_size_byte , 1.0 )
							) /
							(
								( (
									(
										( pow( ref_in_out_quality->d_estimate_kasr_ProcessCost , 2.0 ) + pow( ref_in_out_quality->d_estimate_kasr_DataTransferCost , 2.0 ) ) /
										( ref_in_out_quality->d_estimate_kasr_ProcessCost + ref_in_out_quality->d_estimate_kasr_DataTransferCost )
									) +
									(
										ref_in_out_quality->d_estimate_kasr_AllocationNewMachineCost + ref_in_out_quality->d_estimate_kasr_ContainerStablishmentCost 
									) ) / /*tavan adad 0 ta 1 ra kam mi konad*/
									(
										std::max( ref_in_out_quality->d_estimate_use_leased_cycle_cost - ref_in_out_quality->d_estimate_use_new_leased_cycle_cost , 0.000001 )
									)
								) 
							)
						) * ( SETTINGS_FAST()->d_quality_ratio_calculation_ver == 1 ? 1 : z + 1 );
					break;
				}
				default:
				{
					_ONE_TIME_VAR();
					_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
					return false;
				}
			}
			return true;
		}
		return false;
	}

	{
		double bundle_least_vm_qouta_time = 0;
		for ( auto & node : *ref_in_out_quality->d_pAlias_bundled_nodes_ordered )
		{
			bundle_least_vm_qouta_time += node->d_task_deadline.d_task_quota_duration_based_on_process_need + node->d_task_deadline.d_task_spare_duration_quota;
		}
		double z = ( ref_in_out_quality->calculate_bundle_softdeadline() - ( SIMULATION_NOW_TIME + ref_in_out_quality->d_estimate_TotalTimeConsume ) ) /
			bundle_least_vm_qouta_time;
		if ( z >= 0 )
		{
			////if ( ref_in_out_quality->kasr_TotalTransferedInput + ref_in_out_quality->kasr_TotalExternalEdgeWeight == 0 ) return false;
			////if ( ref_in_out_quality->kasr_ProcessCost == 0 ) return false;
			////if ( ref_in_out_quality->d_total_leased_cycle_cost == 0 ) return false;

			ref_in_out_quality->d_estimate_quality_ratio =
				(
					(
						std::max( ref_in_out_quality->d_estimate_kasr_TotalResidentInputs_size_byte + ref_in_out_quality->d_estimate_kasr_TotalInternalEdgeWeight_size_byte , 1.0 ) /
						std::max( ref_in_out_quality->d_estimate_kasr_TotalTransferedInput_size_byte + ref_in_out_quality->d_estimate_kasr_TotalExternalEdgeWeight_size_byte , 1.0 )
					) /
					(
						( (
							(
								( pow( ref_in_out_quality->d_estimate_kasr_ProcessCost , 2.0 ) + pow( ref_in_out_quality->d_estimate_kasr_DataTransferCost , 2.0 ) ) /
								( ref_in_out_quality->d_estimate_kasr_ProcessCost + ref_in_out_quality->d_estimate_kasr_DataTransferCost )
							) +
							(
								ref_in_out_quality->d_estimate_kasr_AllocationNewMachineCost + ref_in_out_quality->d_estimate_kasr_ContainerStablishmentCost
							) /*tavan adad 0 ta 1 ra kam mi konad*/
						) /
						(
							std::max( ref_in_out_quality->d_estimate_use_leased_cycle_cost - ref_in_out_quality->d_estimate_use_new_leased_cycle_cost , 0.000001 )
						)
						)
					)
				) * ( SETTINGS_FAST()->d_quality_ratio_calculation_ver == 1 ? 1 : z + 1 );

			//if ( ref_in_out_quality->d_occupied_vm )
			//{
			//	XBT_INFO( "#(%d) d_quality_ratio(%f s)" , __LINE__ , ref_in_out_quality->d_quality_ratio );
			//}

			if ( SETTINGS_FAST()->d_echo__calculate_quality_ratio__value_chack )
			{
				XBT_INFO( "#(%d) d_quality_ratio(%f s)" , __LINE__ , ref_in_out_quality->d_estimate_quality_ratio );
			}
		}
		else
		{
			////if ( ref_in_out_quality->kasr_ProcessCost == 0 ) return false;
			////if ( ref_in_out_quality->d_total_leased_cycle_cost == 0 ) return false;

			ref_in_out_quality->d_estimate_quality_ratio =
				( (
					( (
						(
							( pow( ref_in_out_quality->d_estimate_kasr_ProcessCost , 2.0 ) + pow( ref_in_out_quality->d_estimate_kasr_DataTransferCost , 2.0 ) ) /
							( ref_in_out_quality->d_estimate_kasr_ProcessCost + ref_in_out_quality->d_estimate_kasr_DataTransferCost )
						) +
						( ref_in_out_quality->d_estimate_kasr_AllocationNewMachineCost + ref_in_out_quality->d_estimate_kasr_ContainerStablishmentCost )
						) /
						(
							std::max( ref_in_out_quality->d_estimate_use_leased_cycle_cost - ref_in_out_quality->d_estimate_use_new_leased_cycle_cost , 0.000001 )
						) )
					) /
					(
						std::max( ref_in_out_quality->d_estimate_kasr_TotalResidentInputs_size_byte + ref_in_out_quality->d_estimate_kasr_TotalInternalEdgeWeight_size_byte , 1.0 ) /
						std::max( ref_in_out_quality->d_estimate_kasr_TotalTransferedInput_size_byte + ref_in_out_quality->d_estimate_kasr_TotalExternalEdgeWeight_size_byte , 1.0 )
					)
				) * ( SETTINGS_FAST()->d_quality_ratio_calculation_ver == 1 ? -1 : z - 1 );

			if ( SETTINGS_FAST()->d_echo__calculate_quality_ratio__value_chack )
			{
				XBT_INFO( "#(%d) d_quality_ratio(%f s)" , __LINE__ , ref_in_out_quality->d_estimate_quality_ratio );
			}
		}
	}
	return true;
	CATCH_TRY
	return false;
}

bool ClassVM::calculate_quality_for_EPSM_algorithm( VMQuality * ref_in_out_quality )
{
	BEGIN_TRY

	ref_in_out_quality->d_pAliasVmOwnerRegion->d_pAlias_VMM->d_total_try_quality++; // برای بررسی سرعت پیشزفت کار

	bool b_stablish_new_container_if_this_quality_win = false;
	if
	(
		!this->is_provisioned() ||
		( this->d_pLastContainer.allocated_ptr->d_container_dag_config.d_dag_type != ref_in_out_quality->d_quality_dag_config.d_dag_type )
	)
	{
		b_stablish_new_container_if_this_quality_win = true;
	}

	////top_quality->d_pTemp_bundled_nodes_ordered = NULL;
	////top_quality->d_pTemp_bundled_nodes_shuffled = NULL;

	auto & task = ref_in_out_quality->d_pAlias_bundled_nodes_ordered->front();

	////for ( const pair< eNodeID , shared_ptr< DAGNode > > & ref_bundle_node : *ref_in_out_quality->d_pTemp_bundled_nodes_shuffled.get() )
	{
		ref_in_out_quality->d_estimate_TotalProcessFlops += task->d_task_cpu_usage_flops;
		ref_in_out_quality->d_estimate_TotalTaskSize_byte += task->d_task_size_byte;
		ref_in_out_quality->d_estimate_TotalOutputToVMMemory_size_byte += task->d_task_output_size_byte;
	}

	// calculate input transfered
	{
		// make uniq list
		////for ( const pair< eNodeID , shared_ptr< DAGNode > > & ref_bundle_node : *ref_in_out_quality->d_pTemp_bundled_nodes_shuffled.get() )
		{
			for ( auto & parent_node : task->d_parents )
			{
				// in local vm memory . in no exec vm status
				if ( !b_stablish_new_container_if_this_quality_win &&
					 ref_in_out_quality->d_pAliasVM->d_pLastContainer.allocated_ptr != NULL )
				{
					// agar bana bashad container taghir nakonad va node mohasebati input khod ra dar container yaft in data resident behesab miaayad
					if ( ref_in_out_quality->d_pAliasVM->d_pLastContainer.allocated_ptr->d_in_ram_task_output.count( parent_node->d_node_uniq_id ) != 0 ) // agar parent dar hamin container exec shodeh bood
					{
						ref_in_out_quality->d_estimate_kasr_TotalResidentInputs_size_byte += parent_node->d_task_output_size_byte;

						continue;
					}
				}

				// in global storage memroy
				{
					if ( ref_in_out_quality->d_pAliasVmOwnerRegion->d_pAlias_VMM->d_in_GS_output.count( parent_node->d_node_uniq_id ) == 0 )
					{
						XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
					}
					else
					{
						ref_in_out_quality->d_estimate_TotalTransferedInputFromGlobalStorageSize_byte += parent_node->d_task_output_size_byte;
					}
				}
			}
		} // for

		ref_in_out_quality->d_estimate_read_parent_output_from_local_vm_memory_time_consume +=
			ref_in_out_quality->d_pAliasVM->d_vmType->get_duration_datatransfer_on_mem( ref_in_out_quality->d_estimate_kasr_TotalResidentInputs_size_byte );

		ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume +=
			ref_in_out_quality->d_pAliasVM->d_vmType->get_duration_datatransfer_on_wan( ref_in_out_quality->d_estimate_TotalTransferedInputFromGlobalStorageSize_byte );
	}

	//calculate time
	{
		ref_in_out_quality->d_estimate_kasr_AllocationNewMachineCost = 0; // IMPORTANT
		ref_in_out_quality->d_estimate_kasr_ContainerStablishmentCost = 0; // IMPORTANT

		// calculate time and cost
		ref_in_out_quality->d_estimate_TotalTimeConsume = 0; // IMPORTANT

		// warm up vm time
		if ( !this->is_provisioned() ) // is new
		{
			ref_in_out_quality->d_stablish_vm = true;
			// time
			ref_in_out_quality->d_estimate_VirtualMachine_Warmup_time_consume = SETTINGS_FAST()->d_VIRTUALMACHINE_WARMUP_TIME_SEC;
			ref_in_out_quality->d_estimate_TotalTimeConsume += ref_in_out_quality->d_estimate_VirtualMachine_Warmup_time_consume;
		}

		// load container structure on vm time
		// new vm or dag type changed
		if ( b_stablish_new_container_if_this_quality_win )
		{
			ref_in_out_quality->d_stablish_container = true;
			// time
			ref_in_out_quality->d_estimate_StablishContainer_time_consume = ref_in_out_quality->d_quality_dag_config.d_container_initialization_time;
			ref_in_out_quality->d_estimate_TotalTimeConsume += ref_in_out_quality->d_estimate_StablishContainer_time_consume;
		}

		ref_in_out_quality->d_estimate_read_tasks_from_global_storage_time_consume = ref_in_out_quality->d_estimate_TotalTaskSize_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;
		ref_in_out_quality->d_estimate_transfer_tasks_from_global_storage_to_vm_time_consume = d_vmType->get_duration_datatransfer_on_wan( ref_in_out_quality->d_estimate_TotalTaskSize_byte );
		ref_in_out_quality->d_estimate_write_tasks_to_vm_time_consume = d_vmType->get_duration_datatransfer_on_mem( ref_in_out_quality->d_estimate_TotalTaskSize_byte );

		//ref_in_out_quality->d_estimate_read_parent_output_from_local_vm_memory_time_consume

		ref_in_out_quality->d_estimate_read_parent_output_from_global_storage_memory_time_consume = ref_in_out_quality->d_estimate_TotalTransferedInputFromGlobalStorageSize_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;
		// ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume
		ref_in_out_quality->d_estimate_write_output_parent_from_global_storage_on_vm = d_vmType->get_duration_datatransfer_on_mem( ref_in_out_quality->d_estimate_TotalTransferedInputFromGlobalStorageSize_byte );

		ref_in_out_quality->d_estimate_exec_tasks_time_consume = d_vmType->get_execution_duration( ref_in_out_quality->d_estimate_TotalProcessFlops );

		ref_in_out_quality->d_estimate_write_output_to_vm_time_consume = d_vmType->get_duration_datatransfer_on_mem( ref_in_out_quality->d_estimate_TotalOutputToVMMemory_size_byte );
		ref_in_out_quality->d_estimate_transfer_output_to_global_storage_time_consume = d_vmType->get_duration_datatransfer_on_wan( ref_in_out_quality->d_estimate_TotalOutputToVMMemory_size_byte );
		ref_in_out_quality->d_estimate_write_output_to_global_storage_time_consume = ref_in_out_quality->d_estimate_TotalOutputToVMMemory_size_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;

		ref_in_out_quality->d_estimate_TotalTimeConsume +=
			ref_in_out_quality->d_estimate_read_tasks_from_global_storage_time_consume +
			ref_in_out_quality->d_estimate_transfer_tasks_from_global_storage_to_vm_time_consume +
			ref_in_out_quality->d_estimate_write_tasks_to_vm_time_consume +
			ref_in_out_quality->d_estimate_read_parent_output_from_local_vm_memory_time_consume +
			ref_in_out_quality->d_estimate_read_parent_output_from_global_storage_memory_time_consume +
			ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume +
			ref_in_out_quality->d_estimate_write_output_parent_from_global_storage_on_vm +
			////ref_in_out_quality->d_estimate_TotalTransfered_ReadMemory_InputFromLocalRegionTimeConsume +
			////ref_in_out_quality->d_estimate_TotalTransfered_OnNetwork_InputFromLocalRegionTimeConsume +
			////ref_in_out_quality->d_estimate_write_output_parent_from_local_region_on_vm +
			ref_in_out_quality->d_estimate_exec_tasks_time_consume +
			ref_in_out_quality->d_estimate_write_output_to_vm_time_consume +
			ref_in_out_quality->d_estimate_transfer_output_to_global_storage_time_consume +
			ref_in_out_quality->d_estimate_write_output_to_global_storage_time_consume;

		if ( !ref_in_out_quality->d_pAliasVM->is_idle_vm() )
		{
			double d_estimate_finished_prev_running_job_time_consume = std::max( (
				*ref_in_out_quality->d_pAliasVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr->d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() +
				ref_in_out_quality->d_pAliasVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr->d_estimate_TotalTimeConsume ) -
				SIMULATION_NOW_TIME , 0.0 );

			// زمان تا دور بعدی اسکجول
			double d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time =
				SETTINGS_FAST()->d_schedule_periode_delay_time -
				fmod( d_estimate_finished_prev_running_job_time_consume , SETTINGS_FAST()->d_schedule_periode_delay_time );
			
			ref_in_out_quality->d_estimate_TotalTimeConsume +=
				d_estimate_finished_prev_running_job_time_consume + d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time;
		}
	}

	// complete leased cycle
	if ( ref_in_out_quality->d_pAliasVM->is_provisioned() == false || /*در حال اجرا نیست*/
		 ref_in_out_quality->d_pAliasVM->is_idle_vm() == true ) /*قیلی کارش تموم شده*/
	{
		// از تابع برای محاسبه اینکه می توان ایا تسک را برای دور بعدی اسکجول کرد هم استفاده می کنم فلذا قسمت های غیر ضروری مهم نیستند
		AbsoluteTime current_time = SIMULATION_NOW_TIME;
		Leased_Cycle * last_leased_cycle = NULL; // لیست مشکل دارد اگر خالی باشد و ما آخریش رو بخواهیم مموری خراب می شود
		if ( d_leased_cycle_mgr.d_allocated_continuous_leased_cycles.size() > 0 ) // ماشینی که کارش شروع نشده نباید بخاطر صرفا دیدن باقیمانده دوره شروع بکار کند . شروع بکار با اولین تسک تخصیصی شروع می شود
		{
			last_leased_cycle = d_leased_cycle_mgr.d_allocated_continuous_leased_cycles.back(); // last
		}
		ref_in_out_quality->d_estimate_use_new_leased_cycle_cost = 0;
		// محاسبه هزینه سیکل های مورد استفاده

		if ( last_leased_cycle != NULL && current_time > last_leased_cycle->d_start_cycle_time &&
			 current_time <= last_leased_cycle->d_end_cycle_time && last_leased_cycle->isUsed() ) // قبلا تسکی روی سیکل اجرا شده بود
		{
			// یک چیزی به ذهنم رسید اگر ما بیاییم و فقط قیمت مصرف از دوره ماشین را برای نود در نظر بگیریم و برای محاسبه نود
			// اتفاقی که می افتد اینست که ماشین سریعتر زودتر کار را تمام می کند و اثر کاهش قیمت دارد
			// در ضمن اگر در ادامه دوره تسک دیگری بیفتد که مجانی است پس پول را همان اولی حساب کند
			ref_in_out_quality->d_estimate_use_new_leased_cycle_cost =
				std::ceil( std::max( ref_in_out_quality->d_estimate_TotalTimeConsume - ( last_leased_cycle->d_end_cycle_time - current_time ) , 0.0 ) /
							SETTINGS_FAST()->d_VM_LEASED_CYCLE_TIME_SEC ) * /*پول دور کامل را حساب می کند*/
				d_vmType->d_dollar_cost_per_cycle;
		}
		else // اینجا میگه یعنی شروع کار و الان وسط یک سیکلیم که هیچ تسکی روی آن اجرا نشده
		{
			ref_in_out_quality->d_estimate_use_new_leased_cycle_cost =
				( std::ceil( ref_in_out_quality->d_estimate_TotalTimeConsume / SETTINGS_FAST()->d_VM_LEASED_CYCLE_TIME_SEC ) * d_vmType->d_dollar_cost_per_cycle );
		}

		ref_in_out_quality->d_estimate_TotalCost = ref_in_out_quality->d_estimate_use_new_leased_cycle_cost;
	}

	// bundle task max deadline
	////ref_in_out_quality->d_bundle_duration_need_on_cheapest_vm_plus_spare = 0;
	////for ( const shared_ptr< DAGNode > & ref_node : *ref_in_out_quality->d_pTemp_bundled_nodes_ordered.get() )
	////{
	////	ref_in_out_quality->d_bundle_duration_need_on_cheapest_vm_plus_spare +=
	////		ref_in_out_quality->d_pTemp_bundled_nodes_ordered->front()->d_task_deadline->d_task_quota_duration_based_on_process_need +
	////		ref_in_out_quality->d_pTemp_bundled_nodes_ordered->front()->d_task_deadline->d_task_spare_duration_quota;
	////}

	
	if ( SIMULATION_NOW_TIME + ref_in_out_quality->d_estimate_TotalTimeConsume <=
		 *task->d_task_deadline.get_absolute_soft_deadline( true ) )
	{
		return true;
	}

	CATCH_TRY
	return false;
}

void ClassVM::dump( bool inside_another , bool bshort ) const
{
	BEGIN_TRY
	if ( !inside_another ) XBT_INFO_EMPTY_LINE();
	XBT_INFO( "dump_VM %s_%d , @%s" , d_vmType->get_vmtypeName() , d_vm_uniq_id , this->d_pAlias_owner_region->d_region_name.c_str() );

	d_vmType->dump( true );

	if ( !bshort )
	{
		this->d_leased_cycle_mgr.dump( true );
	}
	CATCH_TRY
}

Vmm * ClassVM::get_vmm() const
{
	if ( d_pAlias_owner_region == NULL ) return NULL;
	return d_pAlias_owner_region->d_pAlias_VMM;
}

VM_Region::VM_Region()
{
	static int _region_id = 0;
	d_regionID = ++_region_id;
	d_region_name = string_format( "region_%d" , d_regionID );
	d_pAlias_VMM = NULL;
}

void VM_Region::dump( bool inside_another , bool bshort ) const
{
	BEGIN_TRY
	if ( !inside_another ) XBT_INFO_EMPTY_LINE();
	XBT_INFO( "dump_%s with (%lu vms)" , d_region_name.c_str() , d_alias_local_vms.size() );
	CATCH_TRY
}

string VM_Region::serialize_local_memory_resident_node_output()
{
	string retstr;
	for ( auto & vm : d_alias_local_vms )
	{
		if ( vm->is_provisioned() )
		{
			for ( auto & output : vm->d_pLastContainer.allocated_ptr->d_in_ram_task_output )
			{
				retstr += string_format( "(%d@%d^%s)" , output.first , vm->d_pAlias_owner_region->d_regionID , trim_floatnumber( output.second.first ).c_str() );
			}
		}
	}
	return retstr;
}

#pragma endregion

#pragma region containerC

containerC::containerC()
{
	d_container_uniq_id = -1;
	//d_container_dag_type = srcType;

	//d_pAssignedQuality = NULL;

	d_obsolete_container_time = -1;
}

containerC::containerC( DAG_config srcType )
{
	d_container_uniq_id = -1;
	d_container_dag_config = srcType;

	//d_pAssignedQuality = NULL;

	d_obsolete_container_time = -1;
}
containerC::~containerC()
{
	cleanup();
}
void containerC::cleanup()
{
	d_pAssignedQuality.cleanup();
}

/// <summary>
/// تسک ها اول لود می شوند در ماشین و سپس تک به تک اجرا می شوند و هر کدام که تمام شد زمان اضافه بر سازمانش رو به بچه هاش ارث میرسونه
/// </summary>
void ClassVM::callback_execute_bundle_on_container()
{
	VMQuality * pQuality = d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr;
	BEGIN_TRY

	ClassVM * pVM = pQuality->d_pAliasVM;
	VM_Region * pRegion = pVM->d_pAlias_owner_region;
	Vmm * pVmm = pRegion->d_pAlias_VMM;
	DAG * pDAG = pQuality->d_pAllocated_bundled_nodes_ordered->front()->d_pOwnerDAG;
	containerC * pThisVmContainer = this->d_pLastContainer.allocated_ptr;
	VMachine * pvmtype = pVM->d_vmType;

	//SECTION_ALIVE_CHECK("");

	//static int op_instruction_counter = 0;
	assert(d_pLastContainer.allocated_ptr != NULL);

	ScopeSemaphore scope( &pVmm->d_parallismCounter , ( int )pDAG->d_dag_uniq_id ); // میزان موازی سازی را حساب می کند
	ScopeSemaphore scopeWorkload( &pVmm->d_workload_concurrency , &pVmm->d_workload_max_concurrency ); // میزان کل موازی سازی در وورک لود را می سنجد


	if ( SETTINGS_FAST()->d_echo__callback_actor_container__start_execute )
	{
		XBT_INFO_EMPTY_LINE();
		XBT_INFO( "---------------------------------------" , "" );
		XBT_INFO( "#(%d) start execute (at %f s) %s" , __LINE__ , SIMULATION_NOW_TIME , pQuality->bundle_nodes_text().c_str() );
		this->dump( true , true );
	}

	pVmm->d_statistics[ AeStatisticElement::stt_workload_ccr_vmtype_mapped ]
		[ STATISTICS_CCR_VMTYPE( pDAG->d_ccr , (int64_t)pvmtype->d_type ) ].ccr_vmtype_mapped_count += 1; // تعداد نوع ماشین های تخصیص یافته به هر سی سی ار را می شمارد

	// reset task start time
	//double cumulative_sleep = 0;
	for ( const auto & ref_assign_node : *pQuality->d_pAllocated_bundled_nodes_ordered )
	{
		double cumulative_sleep = 0;

		ref_assign_node->d_task_deadline.declare_task_actual_start_time(); // این نخوه مثل اینست که از ببیرون بهش دستور میدیم اجرا بشه

		// load vm and stablish container affect first node
		// warm up vm
		if ( pQuality->d_stablish_vm )
		{
			pQuality->d_stablish_vm = false;
			double vm_provision_duration = pQuality->d_estimate_VirtualMachine_Warmup_time_consume;
			double vm_provision_duration_cost = pvmtype->get_cost_duration( vm_provision_duration );
			cumulative_sleep += vm_provision_duration;

			pVmm->d_statistics[ AeStatisticElement::stt_vm_provisioning_cost_duration ]
				[ STATISTICS_NAME_NAME( STATISTICS_workload_vm_provision_duration , "total" ) ].vm_provision_duration += vm_provision_duration;
			pVmm->d_statistics[ AeStatisticElement::stt_vm_provisioning_cost_duration ]
				[ STATISTICS_NAME_NAME( STATISTICS_workload_vm_provision_cost , "total" ) ].vm_provision_cost += vm_provision_duration_cost;

			pVmm->d_statistics[ AeStatisticElement::stt_vm_provisioning_cost_duration ]
				[ STATISTICS_NAME_NAME( STATISTICS_workload_vm_provision_duration_per_vm_type , pvmtype->get_vmtypeName() ) ].vm_provision_duration += vm_provision_duration;
			pVmm->d_statistics[ AeStatisticElement::stt_vm_provisioning_cost_duration ]
				[ STATISTICS_NAME_NAME( STATISTICS_workload_vm_provision_cost_per_vm_type , pvmtype->get_vmtypeName() ) ].vm_provision_cost += vm_provision_duration_cost;

			pVmm->d_statistics[ AeStatisticElement::stt_vm_provisioning_cost_duration ]
				[ STATISTICS_NAME_NAME( STATISTICS_workload_vm_provision_duration_per_region , pRegion->d_region_name.c_str() ) ].vm_provision_duration += vm_provision_duration;
			pVmm->d_statistics[ AeStatisticElement::stt_vm_provisioning_cost_duration ]
				[ STATISTICS_NAME_NAME( STATISTICS_workload_vm_provision_cost_per_region , pRegion->d_region_name.c_str() ) ].vm_provision_cost += vm_provision_duration_cost;
		}

		// load container structure on vm
		if ( pQuality->d_stablish_container )
		{
			pQuality->d_stablish_container = false;
			double stablish_container_duration = pQuality->d_quality_dag_config.d_container_initialization_time;
			double stablish_container_duration_cost = pvmtype->get_cost_duration( stablish_container_duration );
			cumulative_sleep += stablish_container_duration;

			pVmm->d_statistics[ AeStatisticElement::stt_stablish_container_cost_duration ]
				[ STATISTICS_NAME_NAME( STATISTICS_workload_stablish_container_duration , "total" ) ].stablish_container_duration += stablish_container_duration;
			pVmm->d_statistics[ AeStatisticElement::stt_stablish_container_cost_duration ]
				[ STATISTICS_NAME_NAME( STATISTICS_workload_stablish_container_cost , "total" ) ].stablish_container_cost += stablish_container_duration_cost;

			pVmm->d_statistics[ AeStatisticElement::stt_stablish_container_cost_duration ]
				[ STATISTICS_NAME_NAME( STATISTICS_workload_stablish_container_duration_per_vm_type , pvmtype->get_vmtypeName() ) ].stablish_container_duration += stablish_container_duration;
			pVmm->d_statistics[ AeStatisticElement::stt_stablish_container_cost_duration ]
				[ STATISTICS_NAME_NAME( STATISTICS_workload_stablish_container_cost_per_vm_type , pvmtype->get_vmtypeName() ) ].stablish_container_cost += stablish_container_duration_cost;

			pVmm->d_statistics[ AeStatisticElement::stt_stablish_container_cost_duration ]
				[ STATISTICS_NAME_NAME( STATISTICS_workload_stablish_container_duration_per_region , pRegion->d_region_name.c_str() ) ].stablish_container_duration += stablish_container_duration;
			pVmm->d_statistics[ AeStatisticElement::stt_stablish_container_cost_duration ]
				[ STATISTICS_NAME_NAME( STATISTICS_workload_stablish_container_cost_per_region , pRegion->d_region_name.c_str() ) ].stablish_container_cost += stablish_container_duration_cost;
		}

		#pragma region read task from gs
		// read task from gs
		{
			cumulative_sleep += ref_assign_node->d_task_size_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;
			// خواندن و نوشتن روی دیسک حافظه عمومی اینقدر سریع است که زمانش به حساب نمی آید
		}

		// data transfer . task from global storage to vm through network
		{
			double duration_on_wan = pvmtype->get_duration_datatransfer_on_wan( ref_assign_node->d_task_size_byte );
			double duration_on_wan_cost = pvmtype->get_cost_duration( duration_on_wan );
			cumulative_sleep += duration_on_wan;

			{ /*statistics*/
				pDAG->d_pStatistic->d_dag_communication_byte_need += ref_assign_node->d_task_size_byte;
				pDAG->d_pStatistic->d_dag_communication_duration += duration_on_wan;
				//
				pDAG->d_pStatistic->d_dag_expectation_communication_byte_on_slow_link += ref_assign_node->d_task_size_byte; // برای محاسبه نسبت انتقال داده روی لینک کند به کل داده انتقالی
				pDAG->d_pStatistic->d_dag_reality_communication_byte_on_slow_link += ref_assign_node->d_task_size_byte;
				{ /*globaly*/
					// data
					pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
						[ STATISTICS_executing_static__read_from_global_storage_count ].read_from_global_storage_count += 1;
					pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
						[ STATISTICS_executing_static__read_from_global_storage_size ].read_from_global_storage_size += ref_assign_node->d_task_size_byte;
					// cost
					pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
						[ STATISTICS_executing_static__access_WAN_cost ].access_WAN_cost += duration_on_wan_cost;
					// duration
					pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
						[ STATISTICS_executing_static__access_WAN_duration ].access_WAN_duration += duration_on_wan;
				}
				{ /*ccr WLM ly*/
					// data
					pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
						[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_WAN_size , pDAG->d_ccr ) ].access_WAN_size += ref_assign_node->d_task_size_byte;
					// cost
					pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
						[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_WAN_cost , pDAG->d_ccr ) ].access_WAN_cost += duration_on_wan_cost;
					// duration
					pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
						[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_WAN_duration , pDAG->d_ccr ) ].access_WAN_duration += duration_on_wan;
				}
				{ /*workload ly*/
					// cost
					pVmm->d_statistics[ AeStatisticElement::stt_workload_process_transfer_cost_duration ]
						[ STATISTICS_workload_transfer_cost ].totally_transfer_cost += duration_on_wan_cost;
					// duration
					pVmm->d_statistics[ AeStatisticElement::stt_workload_process_transfer_cost_duration ]
						[ STATISTICS_workload_transfer_duration ].totally_transfer_duration += duration_on_wan;
				}
				{ /*workload ccr ly*/
					// cost
					pVmm->d_statistics[ AeStatisticElement::stt_workload_ccr_process_transfer_cost_duration ]
						[ STATISTICS_NAME_CCR( STATISTICS_workload_transfer_cost , pDAG->d_ccr ) ].totally_transfer_cost += duration_on_wan_cost;
					// duration
					pVmm->d_statistics[ AeStatisticElement::stt_workload_ccr_process_transfer_cost_duration ]
						[ STATISTICS_NAME_CCR( STATISTICS_workload_transfer_duration , pDAG->d_ccr ) ].totally_transfer_duration += duration_on_wan;
				}
				{ /*vmtype transfer exec ly*/
					// data
					pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
						[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_datatransfer_size , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_size += ref_assign_node->d_task_size_byte;
					// cost
					pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
						[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_datatransfer_cost , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_cost += duration_on_wan_cost;
					// duration
					pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
						[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_datatransfer_duration , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_duration += duration_on_wan;
				}
			}
		}

		// store task in vm hard
		{
			double duration_on_mem = pvmtype->get_duration_datatransfer_on_mem( ref_assign_node->d_task_size_byte );
			double duration_on_mem_cost = pvmtype->get_cost_duration( duration_on_mem );
			cumulative_sleep += duration_on_mem;

			{ /*statistics*/
				{ /*globaly*/
					// data
					pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
						[ STATISTICS_executing_static__access_local_memory_count ].access_local_memory_count++;
					pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
						[ STATISTICS_executing_static__access_local_memory_size ].access_local_memory_size += ref_assign_node->d_task_size_byte;
					// cost
					pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
						[ STATISTICS_executing_static__access_Mem_cost ].access_Mem_cost += duration_on_mem_cost;
					// duration
					pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
						[ STATISTICS_executing_static__access_Mem_duration ].access_Mem_duration += duration_on_mem;
				}
				{ /*ccr WLM ly*/
					// data
					pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
						[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_size , pDAG->d_ccr ) ].access_Mem_size += ref_assign_node->d_task_size_byte;
					// cost
					pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
						[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_cost , pDAG->d_ccr ) ].access_Mem_cost += duration_on_mem_cost;
					// duration
					pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
						[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_duration , pDAG->d_ccr ) ].access_Mem_duration += duration_on_mem;
				}
				{ /*workload ly*/
					// none . فقط انتقال و پروسس
				}
				{ /*workload ccr ly*/
					// none . فقط انتقال و پروسس
				}
			}
		}
		#pragma endregion
		
		// read from local vm ram then local region the other region then ram then global storage
		for ( const auto & ref_parent : ref_assign_node->d_parents )
		{
			// byte need to transfer to vm
			pDAG->d_pStatistic->d_dag_communication_byte_need += ref_parent->d_task_output_size_byte;
			//
			pDAG->d_pStatistic->d_dag_expectation_communication_byte_on_slow_link += ref_parent->d_task_output_size_byte;

			// تسک ها سریالی است فلذا تسک فرزند حتما بعد از تسک پدر اجرا می شود
			if ( pThisVmContainer->d_in_ram_task_output.count( ref_parent->d_node_uniq_id ) == 0 ) // پدری داشت که در مموری خروجی نداشت
			{
				if ( pVmm->d_in_GS_output.count( ref_parent->d_node_uniq_id ) == 0 )
				{
					XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
				}

				ref_parent->d_pRuntime->d_probable_keeper_vm[ pThisVmContainer->d_container_uniq_id ] = pVM; // نود پدر در کانتینر الان ذخیره می شود و الیته شاید بپرد

				// read from local region
				if ( !pQuality->d_EPSM_algorithm )
				{
					vector< ClassVM * > list_local_vm_has_input;
					for ( auto & probable_vm : ref_parent->d_pRuntime->d_probable_keeper_vm ) // در ماشین محلی یکی دیتا داره
					{
						if ( probable_vm.second->is_provisioned() &&
							 probable_vm.second->d_pLastContainer.allocated_ptr != NULL &&
							 probable_vm.second->d_pLastContainer.allocated_ptr->d_obsolete_container_time < 0 &&
							 probable_vm.second->d_pLastContainer.allocated_ptr->d_container_uniq_id == probable_vm.first && // همچنان در ماشین همان کانتینر بود
							 probable_vm.second->d_vm_uniq_id != pVM->d_vm_uniq_id && // در اوکال بود و نه در این ماشین
							 probable_vm.second->d_pAlias_owner_region->d_regionID == pVM->d_pAlias_owner_region->d_regionID &&
							 probable_vm.second->d_pLastContainer.allocated_ptr->d_in_ram_task_output.count( ref_parent->d_node_uniq_id ) != 0
						)
						{
							list_local_vm_has_input.push_back( probable_vm.second );
							break; // شبکه لن یک سرعت دارد برای همه ماشین ها
						}
					}
					if ( list_local_vm_has_input.size() > 0 )
					{
						std::sort( list_local_vm_has_input.begin() , list_local_vm_has_input.end() , []( ClassVM * & a , ClassVM * & b )
						{
							return a->d_vmType->d_memory_access_speed > b->d_vmType->d_memory_access_speed;
						} );

						auto fastest_local_vm = list_local_vm_has_input.front();

						// read parent output from local region host
						{
							double duration_on_mem = fastest_local_vm->d_vmType->get_duration_datatransfer_on_mem( ref_parent->d_task_output_size_byte );
							double duration_on_mem_cost = pvmtype->get_cost_duration( duration_on_mem );
							cumulative_sleep += duration_on_mem;

							{ /*statistics*/
								{ /*globaly*/
									// data
									pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
										[ STATISTICS_executing_static__access_local_memory_count ].access_local_memory_count++;
									pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
										[ STATISTICS_executing_static__access_local_memory_size ].access_local_memory_size += ref_parent->d_task_output_size_byte;
									// cost
									pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
										[ STATISTICS_executing_static__access_Mem_cost ].access_Mem_cost += duration_on_mem_cost;
									// duration
									pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
										[ STATISTICS_executing_static__access_Mem_duration ].access_Mem_duration += duration_on_mem;
								}
								{ /*ccr WLM ly*/
									// data
									pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
										[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_size , pDAG->d_ccr ) ].access_Mem_size += ref_parent->d_task_output_size_byte;
									// cost
									pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
										[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_cost , pDAG->d_ccr ) ].access_Mem_cost += duration_on_mem_cost;
									// duration
									pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
										[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_duration , pDAG->d_ccr ) ].access_Mem_duration += duration_on_mem;
								}
								{ /*workload ly*/
								  // none . فقط انتقال و پروسس
								}
								{ /*workload ccr ly*/
								  // none . فقط انتقال و پروسس
								}
							}
						}

						// data transfer .transfer total parent output from local region host to vm network transfer
						{
							double duration_on_lan = fastest_local_vm->d_vmType->get_duration_datatransfer_on_lan( ref_parent->d_task_output_size_byte );
							double duration_on_lan_cost = pvmtype->get_cost_duration( duration_on_lan );
							cumulative_sleep += duration_on_lan;
							
							pDAG->d_pStatistic->d_dag_communication_duration += duration_on_lan;
							{ /*statistics*/
								{ /*globaly*/
									// data
									pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
										[ STATISTICS_executing_static__read_from_local_region_count ].read_from_local_region_count += 1;
									pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
										[ STATISTICS_executing_static__read_from_local_region_size ].read_from_local_region_size += ref_parent->d_task_output_size_byte;
									// cost
									pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
										[ STATISTICS_executing_static__access_LAN_cost ].access_LAN_cost += duration_on_lan_cost; // هزینه زمان روی این ماشین حساب میشه نه روس ماشین مبدا دیتا
									// duration
									pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
										[ STATISTICS_executing_static__access_LAN_duration ].access_LAN_duration += duration_on_lan;
								}
								{ /*ccr WLM ly*/
									// data
									pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
										[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_LAN_size , pDAG->d_ccr ) ].access_LAN_size += ref_parent->d_task_output_size_byte;
									// cost
									pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
										[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_LAN_cost , pDAG->d_ccr ) ].access_LAN_cost += duration_on_lan_cost;
									// duration
									pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
										[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_LAN_duration , pDAG->d_ccr ) ].access_LAN_duration += duration_on_lan;
								}
								{ /*workload ly*/
									// cost
									pVmm->d_statistics[ AeStatisticElement::stt_workload_process_transfer_cost_duration ]
										[ STATISTICS_workload_transfer_cost ].totally_transfer_cost += duration_on_lan_cost;
									// duration
									pVmm->d_statistics[ AeStatisticElement::stt_workload_process_transfer_cost_duration ]
										[ STATISTICS_workload_transfer_duration ].totally_transfer_duration += duration_on_lan;
								}
								{ /*workload ccr ly*/
									// cost
									pVmm->d_statistics[ AeStatisticElement::stt_workload_ccr_process_transfer_cost_duration ]
										[ STATISTICS_NAME_CCR( STATISTICS_workload_transfer_cost , pDAG->d_ccr ) ].totally_transfer_cost += duration_on_lan_cost;
									// duration
									pVmm->d_statistics[ AeStatisticElement::stt_workload_ccr_process_transfer_cost_duration ]
										[ STATISTICS_NAME_CCR( STATISTICS_workload_transfer_duration , pDAG->d_ccr ) ].totally_transfer_duration += duration_on_lan;
								}
								{ /*vmtype transfer exec ly*/
									// data
									pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
										[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_datatransfer_size , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_size += ref_parent->d_task_output_size_byte;
									// cost
									pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
										[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_datatransfer_cost , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_cost += duration_on_lan_cost;
									// duration
									pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
										[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_datatransfer_duration , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_duration += duration_on_lan;
								}
							}
						}

						// store parent output in vm hard
						{
							double duration_on_mem = pvmtype->get_duration_datatransfer_on_mem( ref_parent->d_task_output_size_byte );
							double duration_on_mem_cost = pvmtype->get_cost_duration( duration_on_mem );
							cumulative_sleep += duration_on_mem;

							{ /*statistics*/
								{ /*globaly*/
									// data
									pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
										[ STATISTICS_executing_static__access_local_memory_count ].access_local_memory_count++;
									pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
										[ STATISTICS_executing_static__access_local_memory_size ].access_local_memory_size += ref_parent->d_task_output_size_byte;
									// cost
									pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
										[ STATISTICS_executing_static__access_Mem_cost ].access_Mem_cost += duration_on_mem_cost;
									// duration
									pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
										[ STATISTICS_executing_static__access_Mem_duration ].access_Mem_duration += duration_on_mem;
								}
								{ /*ccr WLM ly*/
									// data
									pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
										[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_size , pDAG->d_ccr ) ].access_Mem_size += ref_parent->d_task_output_size_byte;
									// cost
									pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
										[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_cost , pDAG->d_ccr ) ].access_Mem_cost += duration_on_mem_cost;
									// duration
									pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
										[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_duration , pDAG->d_ccr ) ].access_Mem_duration += duration_on_mem;
								}
								{ /*workload ly*/
								  // none . فقط انتقال و پروسس
								}
								{ /*workload ccr ly*/
								  // none . فقط انتقال و پروسس
								}
							}
						}

						pThisVmContainer->d_in_ram_task_output[ ref_parent->d_node_uniq_id ] = make_pair( SIMULATION_NOW_TIME , ref_parent ); // این یعنی لود می کنیم پدر لود نشده را
						////if ( SETTINGS_FAST()->d_print_bundle_internal_externals )
						////{
						////	this->d_pAssignedQuality->d_actually_local_region_parent_ids += string_format( "(%d)" , ref_parent->d_node_uniq_id );
						////}
						continue; // donot read from gs anymore
					}
				}

				// read from global storage
				{
					// read parent output from gs
					{
						cumulative_sleep += ref_parent->d_task_output_size_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;
					}

					// data transfer . transfer total parent output from global storage to vm network transfer
					{
						double duration_on_wan = pvmtype->get_duration_datatransfer_on_wan( ref_parent->d_task_output_size_byte );
						double duration_on_wan_cost = pvmtype->get_cost_duration( duration_on_wan );
						cumulative_sleep += duration_on_wan;

						pDAG->d_pStatistic->d_dag_communication_duration += duration_on_wan;
						pDAG->d_pStatistic->d_dag_reality_communication_byte_on_slow_link += ref_parent->d_task_output_size_byte;

						{ /*statistics*/
							{ /*globaly*/
								// data
								pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
									[ STATISTICS_executing_static__read_from_global_storage_count ].read_from_global_storage_count += 1;
								pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
									[ STATISTICS_executing_static__read_from_global_storage_size ].read_from_global_storage_size += ref_parent->d_task_output_size_byte;
								// cost
								pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
									[ STATISTICS_executing_static__access_WAN_cost ].access_WAN_cost += duration_on_wan_cost; // هزینه زمان روی این ماشین حساب میشه نه روس ماشین مبدا دیتا
								// duration
								pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
									[ STATISTICS_executing_static__access_WAN_duration ].access_WAN_duration += duration_on_wan;
							}
							{ /*ccr WLM ly*/
								// data
								pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
									[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_WAN_size , pDAG->d_ccr ) ].access_WAN_size += ref_parent->d_task_output_size_byte;
								// cost
								pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
									[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_WAN_cost , pDAG->d_ccr ) ].access_WAN_cost += duration_on_wan_cost;
								// duration
								pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
									[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_WAN_duration , pDAG->d_ccr ) ].access_WAN_duration += duration_on_wan;
							}
							{ /*workload ly*/
								// cost
								pVmm->d_statistics[ AeStatisticElement::stt_workload_process_transfer_cost_duration ]
									[ STATISTICS_workload_transfer_cost ].totally_transfer_cost += duration_on_wan_cost;
								// duration
								pVmm->d_statistics[ AeStatisticElement::stt_workload_process_transfer_cost_duration ]
									[ STATISTICS_workload_transfer_duration ].totally_transfer_duration += duration_on_wan;
							}
							{ /*workload ccr ly*/
								// cost
								pVmm->d_statistics[ AeStatisticElement::stt_workload_ccr_process_transfer_cost_duration ]
									[ STATISTICS_NAME_CCR( STATISTICS_workload_transfer_cost , pDAG->d_ccr ) ].totally_transfer_cost += duration_on_wan_cost;
								// duration
								pVmm->d_statistics[ AeStatisticElement::stt_workload_ccr_process_transfer_cost_duration ]
									[ STATISTICS_NAME_CCR( STATISTICS_workload_transfer_duration , pDAG->d_ccr ) ].totally_transfer_duration += duration_on_wan;
							}
							{ /*vmtype transfer exec ly*/
								// data
								pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
									[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_datatransfer_size , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_size += ref_parent->d_task_output_size_byte;
								// cost
								pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
									[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_datatransfer_cost , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_cost += duration_on_wan_cost;
								// duration
								pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
									[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_datatransfer_duration , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_duration += duration_on_wan;
							}
						}
					}

					// store parent output in vm hard
					{
						double duration_on_mem = pvmtype->get_duration_datatransfer_on_mem( ref_parent->d_task_output_size_byte );
						double duration_on_mem_cost = pvmtype->get_cost_duration( duration_on_mem );
						cumulative_sleep += duration_on_mem;

						{ /*statistics*/
							{ /*globaly*/
								// data
								pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
									[ STATISTICS_executing_static__access_local_memory_count ].access_local_memory_count++;
								pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
									[ STATISTICS_executing_static__access_local_memory_size ].access_local_memory_size += ref_parent->d_task_output_size_byte;
								// cost
								pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
									[ STATISTICS_executing_static__access_Mem_cost ].access_Mem_cost += duration_on_mem_cost;
								// duration
								pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
									[ STATISTICS_executing_static__access_Mem_duration ].access_Mem_duration += duration_on_mem;
							}
							{ /*ccr WLM ly*/
								// data
								pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
									[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_size , pDAG->d_ccr ) ].access_Mem_size += ref_parent->d_task_output_size_byte;
								// cost
								pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
									[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_cost , pDAG->d_ccr ) ].access_Mem_cost += duration_on_mem_cost;
								// duration
								pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
									[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_duration , pDAG->d_ccr ) ].access_Mem_duration += duration_on_mem;
							}
							{ /*workload ly*/
							  // none . فقط انتقال و پروسس
							}
							{ /*workload ccr ly*/
							  // none . فقط انتقال و پروسس
							}
						}
					}

					assert( pVmm->d_in_GS_output.count( ref_parent->d_node_uniq_id ) != 0 );

					////pDAG->d_dag_parent_communication_byte_transfered_from_gs += ref_parent->d_task_output_size_byte;

					pThisVmContainer->d_in_ram_task_output[ ref_parent->d_node_uniq_id ] = make_pair( SIMULATION_NOW_TIME , ref_parent ); // این یعنی لود می کنیم پدر لود نشده را
					////if ( SETTINGS_FAST()->d_print_bundle_internal_externals )
					////{
					////	this->d_pAssignedQuality->d_actually_globally_parent_ids += string_format( "(%d)" , ref_parent->d_node_uniq_id );
					////}
					continue;
				}
			}
			else // d_in_ram_task_output.count( ref_parent->d_node_uniq_id ) != 0
			{
				double duration_on_mem = pvmtype->get_duration_datatransfer_on_mem( ref_parent->d_task_output_size_byte );
				double duration_on_mem_cost = pvmtype->get_cost_duration( duration_on_mem );

				{ /*statistics*/
					{ /*globaly*/
						// data
						pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
							[ STATISTICS_executing_static__access_local_memory_count ].access_local_memory_count++;
						pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
							[ STATISTICS_executing_static__access_local_memory_size ].access_local_memory_size += ref_parent->d_task_output_size_byte;
						// cost
						pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
							[ STATISTICS_executing_static__access_Mem_cost ].access_Mem_cost += duration_on_mem_cost;
						// duration
						pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
							[ STATISTICS_executing_static__access_Mem_duration ].access_Mem_duration += duration_on_mem;
					}
					{ /*ccr WLM ly*/
						// data
						pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
							[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_size , pDAG->d_ccr ) ].access_Mem_size += ref_parent->d_task_output_size_byte;
						// cost
						pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
							[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_cost , pDAG->d_ccr ) ].access_Mem_cost += duration_on_mem_cost;
						// duration
						pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
							[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_duration , pDAG->d_ccr ) ].access_Mem_duration += duration_on_mem;
					}
					{ /*workload ly*/
					  // none . فقط انتقال و پروسس
					}
					{ /*workload ccr ly*/
					  // none . فقط انتقال و پروسس
					}
				}
				////if ( SETTINGS_FAST()->d_print_bundle_internal_externals )
				////{
				////	this->d_pAssignedQuality->d_actually_local_memory_parent_ids += string_format( "(%d)" , ref_parent->d_node_uniq_id );
				////}
			}
		}
		
		// exec task
		{
			double duration_execution = pvmtype->get_execution_duration( ref_assign_node->d_task_cpu_usage_flops );
			double duration_execution_cost = pvmtype->get_cost_duration( duration_execution );
			cumulative_sleep += duration_execution;
			//
			pDAG->d_pStatistic->d_dag_reality_computation_time += duration_execution;
			double avg = 0;
			for ( auto & machine : VMachine::get_type_instances() )
			{
				avg += machine.get_execution_duration( ref_assign_node->d_task_cpu_usage_flops );
			}
			avg /= VMachine::get_type_instances().size();
			pDAG->d_pStatistic->d_dag_computation_time_on_avg_machine += avg;

			{ /*statistics*/
				{ /*workload ly*/
					// cost
					pVmm->d_statistics[ AeStatisticElement::stt_workload_process_transfer_cost_duration ]
						[ STATISTICS_workload_execution_cost ].totally_execution_cost += duration_execution_cost;
					// duration
					pVmm->d_statistics[ AeStatisticElement::stt_workload_process_transfer_cost_duration ]
						[ STATISTICS_workload_execution_duration ].totally_execution_duration += duration_execution;
				}
				{ /*workload ccr ly*/
					// cost
					pVmm->d_statistics[ AeStatisticElement::stt_workload_ccr_process_transfer_cost_duration ]
						[ STATISTICS_NAME_CCR( STATISTICS_workload_execution_cost , pDAG->d_ccr ) ].totally_execution_cost += duration_execution_cost;
					// duration
					pVmm->d_statistics[ AeStatisticElement::stt_workload_ccr_process_transfer_cost_duration ]
						[ STATISTICS_NAME_CCR( STATISTICS_workload_execution_duration , pDAG->d_ccr ) ].totally_execution_duration += duration_execution;
				}
				{ /*vmtype transfer exec ly*/
					// data
					pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
						[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_execution_size , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_size += ref_assign_node->d_task_cpu_usage_flops;
					// cost
					pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
						[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_execution_cost , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_cost += duration_execution_cost;
					// duration
					pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
						[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_execution_duration , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_duration += duration_execution;
				}
			}
		}

		// store output to vm hard
		{
			double duration_on_mem = pvmtype->get_duration_datatransfer_on_mem( ref_assign_node->d_task_output_size_byte );
			double duration_on_mem_cost = pvmtype->get_cost_duration( duration_on_mem );
			cumulative_sleep += duration_on_mem;

			{ /*statistics*/
				{ /*globaly*/
					// data
					pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
						[ STATISTICS_executing_static__access_local_memory_count ].access_local_memory_count++;
					pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
						[ STATISTICS_executing_static__access_local_memory_size ].access_local_memory_size += ref_assign_node->d_task_output_size_byte;
					// cost
					pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
						[ STATISTICS_executing_static__access_Mem_cost ].access_Mem_cost += duration_on_mem_cost;
					// duration
					pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
						[ STATISTICS_executing_static__access_Mem_duration ].access_Mem_duration += duration_on_mem;
				}
				{ /*ccr WLM ly*/
					// data
					pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
						[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_size , pDAG->d_ccr ) ].access_Mem_size += ref_assign_node->d_task_output_size_byte;
					// cost
					pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
						[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_cost , pDAG->d_ccr ) ].access_Mem_cost += duration_on_mem_cost;
					// duration
					pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
						[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_Mem_duration , pDAG->d_ccr ) ].access_Mem_duration += duration_on_mem;
				}
				{ /*workload ly*/
					// none . فقط انتقال و پروسس
				}
			}
		}

		// each task store on vm::container ram
		pThisVmContainer->d_in_ram_task_output[ ref_assign_node->d_node_uniq_id ] = make_pair( SIMULATION_NOW_TIME , ref_assign_node );
		ref_assign_node->d_pRuntime->d_probable_keeper_vm[ pThisVmContainer->d_container_uniq_id ] = pVM;
		//
		pDAG->d_pStatistic->d_dag_expectation_communication_byte_on_slow_link += ref_assign_node->d_task_output_size_byte; // انتظار میره این خروجی در حافظه عمومی برود ولی شاید نرود


		// check if has child outside bundle send output to gs
		bool bHasOutsideChild = ref_assign_node->d_links.size() == 0; // آخرین نود ها به گلوبال استوریج دیتا باید بفرستند
		for ( auto & ref_lnk : ref_assign_node->d_links )
		{
			if ( pQuality->d_pAllocated_bundled_nodes_shuffled->count( (ref_lnk->d_pDestination)->d_node_uniq_id ) == 0 )
			{
				bHasOutsideChild = true;
				break;
			}
		}
		if ( bHasOutsideChild )
		{
			// data transfer .transfer output to global storage . network
			{
				double duration_on_wan = pvmtype->get_duration_datatransfer_on_wan( ref_assign_node->d_task_output_size_byte );
				double duration_on_wan_cost = pvmtype->get_cost_duration( duration_on_wan );
				cumulative_sleep += duration_on_wan;

				pDAG->d_pStatistic->d_dag_communication_byte_need += ref_assign_node->d_task_output_size_byte;
				pDAG->d_pStatistic->d_dag_communication_duration += duration_on_wan;
				pDAG->d_pStatistic->d_dag_reality_communication_byte_on_slow_link += ref_assign_node->d_task_output_size_byte;
				
				{ /*statistics*/
					{ /*globaly*/
						// data
						pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
							[ STATISTICS_executing_static__write_to_global_storage_count ].write_to_global_storage_count += 1;
						pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
							[ STATISTICS_executing_static__write_to_global_storage_size ].write_to_global_storage_size += ref_assign_node->d_task_output_size_byte;
						// cost
						pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
							[ STATISTICS_executing_static__access_WAN_cost ].access_WAN_cost += duration_on_wan_cost; // هزینه زمان روی این ماشین حساب میشه نه روس ماشین مبدا دیتا
						// duration
						pVmm->d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
							[ STATISTICS_executing_static__access_WAN_duration ].access_WAN_duration += duration_on_wan;
					}
					{ /*ccr WLM ly*/
						// data
						pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
							[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_WAN_size , pDAG->d_ccr ) ].access_WAN_size += ref_assign_node->d_task_output_size_byte;
						// cost
						pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
							[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_WAN_cost , pDAG->d_ccr ) ].access_WAN_cost += duration_on_wan_cost;
						// duration
						pVmm->d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ]
							[ STATISTICS_NAME_CCR( STATISTICS_executing_static__access_WAN_duration , pDAG->d_ccr ) ].access_WAN_duration += duration_on_wan;
					}
					{ /*workload ly*/
						// cost
						pVmm->d_statistics[ AeStatisticElement::stt_workload_process_transfer_cost_duration ]
							[ STATISTICS_workload_transfer_cost ].totally_transfer_cost += duration_on_wan_cost;
						// duration
						pVmm->d_statistics[ AeStatisticElement::stt_workload_process_transfer_cost_duration ]
							[ STATISTICS_workload_transfer_duration ].totally_transfer_duration += duration_on_wan;
					}
					{ /*workload ccr ly*/
						// cost
						pVmm->d_statistics[ AeStatisticElement::stt_workload_ccr_process_transfer_cost_duration ]
							[ STATISTICS_NAME_CCR( STATISTICS_workload_transfer_cost , pDAG->d_ccr ) ].totally_transfer_cost += duration_on_wan_cost;
						// duration
						pVmm->d_statistics[ AeStatisticElement::stt_workload_ccr_process_transfer_cost_duration ]
							[ STATISTICS_NAME_CCR( STATISTICS_workload_transfer_duration , pDAG->d_ccr ) ].totally_transfer_duration += duration_on_wan;
					}
					{ /*vmtype transfer exec ly*/
						// data
						pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
							[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_datatransfer_size , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_size += ref_assign_node->d_task_output_size_byte;
						// cost
						pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
							[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_datatransfer_cost , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_cost += duration_on_wan_cost;
						// duration
						pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ]
							[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_datatransfer_duration , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_duration += duration_on_wan;
					}
				}
			}

			// store on global storage . write
			{
				cumulative_sleep += ref_assign_node->d_task_output_size_byte / SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED;
				//
				pVmm->d_in_GS_output[ ref_assign_node->d_node_uniq_id ] = ref_assign_node; // store executed node on container memory
			}
		}

		if ( SETTINGS_FAST()->d_override_cumulative_sleep < 1 )
		{
			sg4::this_actor::sleep_for( cumulative_sleep );
		}
		else
		{
			_ONE_TIME_VAR();
			_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
			sg4::this_actor::sleep_for(SETTINGS_FAST()->d_override_cumulative_sleep); // temp
		}

		pVmm->d_statistics[ AeStatisticElement::stt_workload_tasks_process_duration_cost ]
			[ STATISTICS_workload_tasks_process_cost ].workload_tasks_process_cost_sum += pvmtype->get_cost_duration( cumulative_sleep );
		pVmm->d_statistics[ AeStatisticElement::stt_workload_tasks_process_duration_cost ]
			[ STATISTICS_workload_tasks_process_duration ].workload_tasks_process_duration_sum += cumulative_sleep;

		pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_process_statistics ]
			[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_tasks_process_cost , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_cost +=
			pvmtype->get_cost_duration( cumulative_sleep );
		pVmm->d_statistics[ AeStatisticElement::stt_workload_vmtype_process_statistics ]
			[ STATISTICS_NAME_VMTYPE( STATISTICS_workload_vmtype_process_duration , (int64_t)pvmtype->d_type ) ].workload_vmtype_process_duration += cumulative_sleep;

		pVmm->d_pIteration->pDGG->d_proccessed_node_edge +=
			ref_assign_node->d_task_cpu_usage_flops * ref_assign_node->d_Katz_value;

		////ref_assign_node->d_taskProccessed = true; // agregate and set later . important to not here . not comment this
		////ref_assign_node->d_task_complete_time = SIMULATION_NOW_TIME; // set in common

		ref_assign_node->d_task_deadline.declare_task_actual_finish_time();
	}
	////sg4::this_actor::sleep_for(cumulative_sleep);
	
	// تا وقتی کار باندل تمام نشده تسک های وابسته نتوانند کارشان را بکنند
	for ( const auto & ref_assign_node : *pQuality->d_pAllocated_bundled_nodes_ordered )
	{
		ref_assign_node->d_pRuntime->d_taskProccessed = true;
		////ref_assign_node->d_task_deadline->declare_task_actual_finish_time(); // 14020818 . خیلی اجرا کند پیش می زفت گفتم اسلیپ یکجا جمع باشد که شاید سریعتر پیش برود
	}

	pQuality->finish_executing_quality( pQuality );
	// 14020829 . ای خط را با خط پست پروسس در این تاریخ جایجا کردم یعنی اول میگیم موالیتی کارش تمام شد و سپس نود های بعدی را اضافه می کنیم یا اتمام کار درا اعلام می کنیم

	pVmm->d_pIteration->pDGG->post_process_DAG( pQuality );
	
	if ( SETTINGS_FAST()->d_echo__callback_actor_container__done_execute )
	{
		//XBT_INFO_EMPTY_LINE();
		XBT_INFO( "#(%d) done execute (at %f s) nodes(%s-%s)" , __LINE__ , SIMULATION_NOW_TIME , pQuality->d_pAllocated_bundled_nodes_ordered->front()->d_allocated_node_name , pQuality->bundle_nodes_text().c_str() );
		if ( SETTINGS_FAST()->d_echo__callback_actor_container__done_execute_dump_bundle )
		{
			this->dump( true , false );
		}
		//XBT_INFO( "---------------------------------------" , "" );
	}

	CATCH_TRY
			
	this->d_pLastContainer.allocated_ptr->d_pAssignedQuality.cleanup(); // important
	this->d_pVmActorKeeper = this->d_pVmActor;
	this->d_pVmActor = NULL; // ravesh tashkhis vm not idle az royeh actor hast . important
}

void containerC::dump( bool inside_another , bool bshort ) const
{
	BEGIN_TRY
	if ( !inside_another ) XBT_INFO_EMPTY_LINE();
	XBT_INFO( "dump_Container %d  type(%d)" , d_container_uniq_id , d_container_dag_config.d_dag_type );

	if ( d_pAssignedQuality.allocated_ptr != NULL )
	{
		if ( bshort )
		{
			d_pAssignedQuality.allocated_ptr->d_pAliasVM->dump( true , true );
		}
		else
		{
			d_pAssignedQuality.allocated_ptr->dump( true , true );
		}
	}
	if ( !bshort )
	{
		string done_nodes;
		done_nodes += "container inmemory node outputs(";
		for ( auto & ref_nodes : d_in_ram_task_output )
		{
			done_nodes += "(" + string_format( "%s" , ref_nodes.second.second->d_allocated_node_name ) + "),";
		}
		XBT_INFO( "%s" , done_nodes.c_str() );
	}
	CATCH_TRY
}

#pragma endregion

#pragma region Vmm

Vmm::Vmm()
{
	d_pVmmGlobalStorageHost = NULL;
	//d_pVmmGlobalStorageDisk = NULL;
	d_pVMMDeprovisioningActor = NULL;
	d_resume_iteration = NULL;
	d_total_try_quality = 0;
	//d_last_subbundle_try_quality = 0;
	//d_max_subbundle_try_quality = 0;
	d_last_candidate_quality_count = 0;
	d_max_candidate_quality_count = 0;
	d_reject_vm_by_branch_and_bound = 0;
	d_workload_concurrency = 0;
	d_workload_max_concurrency = 0;
	d_online_vm_count = d_max_online_vm_count = 0;
}

void Vmm::init_VManagement( sg4::NetZone * srcpZone , int simulationIteration , Vmm * prev_vmm )
{
	BEGIN_TRY
	
	if ( SETTINGS_FAST()->d_notify_init_VManagement_finished )
	{
		XBT_INFO( "#(%d) Vmm::init_VManagement" , __LINE__ );
	}
	d_critical_section_entered_mutex = sg4::Mutex::create();
	if ( prev_vmm == NULL )
	{
		d_pVmmGlobalStorageHost = srcpZone->create_host( string_format( "hVMM%d" , simulationIteration ) , SETTINGS_FAST()->d_VMM_HOST_SPEED );
		////d_pVmmGlobalStorageDisk = d_pVmmGlobalStorageHost->create_disk( string_format( "dVMM%d" , simulationIteration ) , SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED , SETTINGS_FAST()->d_GLOBAL_STORAGE_MEMORY_SPEED )->seal();

		for ( int i = 1 ; i <= 4 ; i++ ) // ابن تعداد ریجن نیست همبیجوری یک ضریب است
		{
			////CPointer< VM_Region > new_region;
			////new_region.allocate();
			////new_region.allocated_ptr->d_pAlias_VMM = this;
			for ( int ivm = 0 ; ivm < SETTINGS_FAST()->d_vm_per_region_count ; ivm++ )
			{

				ClassVM * new_vm = __NEW( ClassVM() );
				////new_vm->d_owner_region = new_region;
				new_vm->init_vm( new_vm , this , srcpZone);
				d_allocated_available_VMs.push_back( new_vm ); // insert into vm pool then init region and type when nedded
				////new_region->d_local_vms.push_back( new_vm ); // must insert after init

			}
			////d_allocated_regions.push_back( new_region );
		}
	}
	else
	{
		d_pVmmGlobalStorageHost = prev_vmm->d_pVmmGlobalStorageHost;
		////d_pVmmGlobalStorageDisk = prev_vmm->d_pVmmGlobalStorageDisk;

		////d_allocated_regions = prev_vmm->d_allocated_regions; // each vmm has its own regions count
		d_allocated_available_VMs = prev_vmm->d_allocated_available_VMs; // vm quantity is fixed
	}
	
	if ( SETTINGS_FAST()->d_use_deprovisioning_vm_policy )
	{
		std::function<void()> f_callback_resource_manager = std::bind( &Vmm::callback_resource_manager , this );
		d_pVMMDeprovisioningActor = sg4::Actor::create( string_format( "VMM_ResouceManager%d" , simulationIteration ) , d_pVmmGlobalStorageHost , f_callback_resource_manager );
	}

	//if ( SETTINGS_FAST()->d_dump_region_after_creation )
	//{
	//	for ( auto & reg : d_allocated_regions )
	//	{
	//		reg.allocated_ptr->dump( true , true );
	//	}
	//}
	if ( SETTINGS_FAST()->d_notify_init_VManagement_finished )
	{
		XBT_INFO( "#(%d) ~Vmm::init_VManagement" , __LINE__ );
	}
	CATCH_TRY
}

void Vmm::callback_resource_manager()
{
	while ( d_resume_iteration == NULL ) // for next iteration that not started yet
	{
		{
			sg4::this_actor::sleep_for( nextTime( 1.0 / SETTINGS_FAST()->d_deprovision_delay_duration ) );
		}
	};
	XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	while ( *d_resume_iteration && sg4::this_actor::get_host()->is_on() )
	{
		{
			std::lock_guard lock(*d_critical_section_entered_mutex);
			int deprovision_count = 0;
			int provision_count = 0;
			int provision_idle_count = 0;
			for ( auto & vm : d_allocated_available_VMs )
			{
				if ( vm->is_provisioned() )
				{
					provision_count++;
					if ( vm->is_idle_vm() )
					{
						provision_idle_count++;

						Leased_Cycle * last_exist_cycle = NULL; // back in empty list make memory curropt
						if ( vm->d_leased_cycle_mgr.d_allocated_continuous_leased_cycles.size() > 0 )
						{
							////vm->d_leased_cycle_mgr.check_point( false , "" ); // همون وقتی که لازم است توسعه اش بدهم یعنی وقت دیپروویژن
							last_exist_cycle = vm->d_leased_cycle_mgr.d_allocated_continuous_leased_cycles.back(); // last
						}
						if ( last_exist_cycle == NULL )
						{
							XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
						}
						if ( last_exist_cycle != NULL )
						{
							//if ( SIMULATION_NOW_TIME < last_cycle->d_start_cycle_time || SIMULATION_NOW_TIME > last_cycle->d_end_cycle_time )
							//{
							//	XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ );
							//}

							double now_to_end_cycle = last_exist_cycle->d_end_cycle_time - SIMULATION_NOW_TIME; // tr
							double time_need_to_deprovision = SETTINGS_FAST()->d_deprovisioning_delay_estimate; // td

							if ( ( now_to_end_cycle < 0 /*in non used cycle*/ ) ||
								 ( ( now_to_end_cycle - time_need_to_deprovision >= 0 /*agar vaght dareh barayeh dprovision*/ ) &&
								   ( now_to_end_cycle - time_need_to_deprovision <= SETTINGS_FAST()->d_PROV_POLLING_TIME /*near end to cycle*/ ) ) )
							{
								vm->deprovisionVM();
								deprovision_count++;
							}
						}
					}
				}
			}
			if ( SETTINGS_FAST()->d_dump_deprovision_count_if_greater_than != -1 && deprovision_count > SETTINGS_FAST()->d_dump_deprovision_count_if_greater_than )
			{
				XBT_INFO_EMPTY_LINE();
				XBT_INFO( "deprovision vm count(%d) - provision_count(%d) - provision_idle_count(%d)" ,
						  deprovision_count , provision_count , provision_idle_count );
			}
		}
		sg4::this_actor::sleep_for( SETTINGS_FAST()->d_deprovision_delay_duration );
	}
}

#pragma region boundling stuff

class queue_map_connector
{
public:
	queue_map_connector( map< eNodeID , double > & srcmap , map< int , int > * pBFS_sortorder ) : d_Dijkstra_sortorder( srcmap ) , d_pBFS_sortorder( pBFS_sortorder )
	{
	}

	map< eNodeID , double > & d_Dijkstra_sortorder;
	map< int , int > * d_pBFS_sortorder;

	//bool operator()(Node& a,Node& b) // overloading both operators 
	//{
	//	return a.w < b.w: // if you want increasing order;(i.e increasing for minPQ)
	//	return a.w > b.w // if you want reverse of default order;(i.e decreasing for minPQ)
	//}

	bool operator()( DAGNode * & left , DAGNode * & right )
	{
		if ( d_pBFS_sortorder != NULL )
		{
			return (*d_pBFS_sortorder)[ left->d_node_uniq_id ] > (*d_pBFS_sortorder)[ right->d_node_uniq_id ]; // درج شده اول بیاید اول
		}
		return d_Dijkstra_sortorder[ left->d_node_uniq_id ] > d_Dijkstra_sortorder[ right->d_node_uniq_id ]; // میخواهیم کوچکتر بیاید اول
	}
};

#pragma endregion

// yek edge data beh vm nemi ferestad . har task chand parent darad
void Vmm::bundling_and_scheduleOnVM( DAGNode * ref_Node )
{
	BEGIN_TRY

	////if ( d_last_subbundle_try_quality > d_max_subbundle_try_quality )
	////{
	////	d_max_subbundle_try_quality = d_last_subbundle_try_quality;
	////}
	////d_last_subbundle_try_quality = 0;

	std::lock_guard lock( *d_critical_section_entered_mutex );
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	//SECTION_ALIVE_CHECK(ref_Node->d_pOwnerDAG->get_name());

	//SectionTimeKeeper sectionTimeKeeper( &_pSectionTimeKeeper , SETTINGS_FAST()->d_SectionTimeKeeper_minimum_duration_to_dump_slow_part_in_milisecond );

	if ( SETTINGS_FAST()->d_echo__scheduleOnVM__schedule_new_node )
	{
		XBT_INFO_EMPTY_LINE();
		XBT_INFO( "#(%d) schedule node(%s) @%s" , __LINE__ , ref_Node->d_allocated_node_name ,
				  ref_Node->d_pOwnerDAG->get_name().c_str() );
	}

	map< eNodeID , DAGNode * > bundled_nodes_shuffled; // order shuffled . keep node ready to schedule
	vector< DAGNode * > bundled_nodes_ordered; // order keeped
	int bundle_id = -1;
	bool one_wait_quality_is_in_queue = false; // فکرم اینست که وقتی یک کوالیتی مثبت است بر روی ماشین از قبل در حال کار خوب ادامه بررسی ماشین ها به چه دردی می خورد
	bool one_candidate_is_positive = false;

	vector< CPointer< VMQuality > > candidate_qualities;

	map< eNodeID , double > forward_and_seen_node_weight; // hameh nod ehayeh sar zadeh shodeh va in bundle
	map< int , int > node_sort_order;
	priority_queue< DAGNode * , vector< DAGNode * > , queue_map_connector >  p_que { queue_map_connector( forward_and_seen_node_weight , SETTINGS_FAST()->d_greedy_expantion_ver == 1 ? NULL : &node_sort_order ) };

	forward_and_seen_node_weight[ ref_Node->d_node_uniq_id ] = 0;
	node_sort_order[ ref_Node->d_node_uniq_id ] = (int)node_sort_order.size();
	p_que.push( ref_Node );
	
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	{
		//SECTION_TIME( ( std::to_string( ref_Node->d_dax_id ) + "@" + ( ref_Node->d_pOwnerDAG->get_name().c_str() ) ).c_str() );
		while ( !p_que.empty() )
		{
			auto current_node = p_que.top(); // do not use reference . 
			p_que.pop();

			if ( SETTINGS_FAST()->d_echo__scheduleOnVM__nearest_node )
			{
				XBT_INFO_EMPTY_LINE();
				XBT_INFO
				(
					"#(%d) nearest node_%s @%s" , __LINE__ , current_node->d_allocated_node_name ,
					current_node->d_pOwnerDAG->get_name().c_str()
				);
			}
			//return;
			//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
			bool isSuitableForAnyMachine = false;
			bool isSuitableForAnyProvisionedVM = false;
			{
				if ( bundled_nodes_shuffled.count( current_node->d_node_uniq_id ) != 0 )
				{
					XBT_INFO_OVERRIDEABLE__( AeDumpLevel::to_dump_impossible_condition , "#(%d) assert . fault . error . impossible" , __LINE__ ); // قسمت مصرف شده از سیکل ها بیشتر از سیکل ها که نمیشه
				}
				bundled_nodes_shuffled[ current_node->d_node_uniq_id ] = current_node; // add node to list posible for exec on vm
				bundled_nodes_ordered.push_back( current_node );
				bundle_id++;

				// این چک برای اطمینان است اگر نه که اصلا نباید به اینجا بیاید وقتی یک ویت داریم
				if ( SETTINGS_FAST()->d_wait_policy_ver == 3 )
				{
					if ( one_wait_quality_is_in_queue ) // اگر یک ماشین در حال کار کوالیتی مثبتی داشت خوب دیگر کار تمام است و هر چه باندل را ادامه دهیم بازهم نود اول دچار ویت می شود
					{
						break;
					}
				}
				else if ( SETTINGS_FAST()->d_wait_policy_ver == 4 )
				{
					// do nothing
				}

				// avaz lock va inha hameh ehtemal bedahand keh royeh hamem container ha emkan
				// schedule hast baadan saresh dava konand va har kas tavanest zood tar schedule konad barandeh ast
				// mutex for lock reserve container for process

				//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

				// find suitable vm in provisioned
				if ( 1 )
				{
					//string sss;
					//for ( auto & node : bundled_nodes_ordered )
					//{
					//	sss += ( std::to_string( node->d_dax_id ) + " " ).c_str();
					//}
					////SECTION_TIME(sss.c_str());
					for ( auto & provisionedVM : d_allocated_available_VMs )
					{
						if ( provisionedVM->is_provisioned() ) // if ( provisionedVM->is_provisioned() && provisionedVM->is_idle_vm() )
						{
							bool bvmfiltered = false;
							switch ( SETTINGS_FAST()->d_vm_share_policy_ver )
							{
								case 1: // full sharing
								{
									bvmfiltered = false;
									break;
								}
								case 2: // only dag
								{
									_ONE_TIME_VAR();
									_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
									bvmfiltered = true;
									if ( provisionedVM->d_pLastContainer.allocated_ptr != NULL &&
										 provisionedVM->d_pLastContainer.allocated_ptr->d_dag_id_assigned == current_node->d_pOwnerDAG->d_dag_uniq_id )
									{
										bvmfiltered = false;
									}
									break;
								}
								case 3: // only dag type
								{
									_ONE_TIME_VAR();
									_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
									bvmfiltered = true;
									if ( provisionedVM->d_pLastContainer.allocated_ptr != NULL &&
										 provisionedVM->d_pLastContainer.allocated_ptr->d_container_dag_config.d_dag_type == current_node->d_pOwnerDAG->d_dag_config.d_dag_type )
									{
										bvmfiltered = false;
									}
									break;
								}
								default:
								{
									_ONE_TIME_VAR();
									_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
									break;
								}
							}
							if ( bvmfiltered )
							{
								continue;
							}


							{
								if ( !SETTINGS_FAST()->d_use_wait_policy && !provisionedVM->is_idle_vm() ) continue;

								if ( SETTINGS_FAST()->d_use_wait_policy && SETTINGS_FAST()->d_wait_policy_ver == 3 )
								{
									if ( bundle_id > 1 && !provisionedVM->is_idle_vm() ) // در این نوع ویت پالیسی فقط برای نود اسکجول شده اگر زمان اسپیر کافی بود ویت اعمال می شود
									{
										continue;
									}
								}

								if ( SETTINGS_FAST()->d_wait_policy_ver == 3 )
								{
									if ( one_wait_quality_is_in_queue ) // اگر یک ماشین در حال کار کوالیتی مثبتی داشت خوب دیگر کار تمام است و هر چه باندل را ادامه دهیم بازهم نود اول دچار ویت می شود
									{
										break;
									}
								}

								// اگر تا دور بعدی اسکچول آزاد نمیشه کلا بدرد نمی خوره
								// branch and bound
								if ( SETTINGS_FAST()->d_use_wait_policy )
								{
									if ( !provisionedVM->is_idle_vm() &&
										 provisionedVM->d_pLastContainer.allocated_ptr != NULL &&
										 provisionedVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr != NULL )
									{
										double d_estimate_finished_prev_running_job_time_consume = std::max( (
											*provisionedVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr->d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() +
											provisionedVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr->d_estimate_TotalTimeConsume ) -
											SIMULATION_NOW_TIME , 0.0 );

										// برای برنچ از باند همینکه زمان اجرا تا قبل از دور بعدی باشد کافیست
										//double d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time =
										//	SETTINGS_FAST()->d_schedule_periode_delay_time -
										//	fmod( d_estimate_finished_prev_running_job_time_consume , SETTINGS_FAST()->d_schedule_periode_delay_time );

										// آزاد شدن ماشین تا دور بعدی
										if ( d_estimate_finished_prev_running_job_time_consume /* +
											 d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time*/ > SETTINGS_FAST()->d_schedule_periode_delay_time )
										{
											d_reject_vm_by_branch_and_bound++; // 1
											continue;
										}
									}
								}

								// حداقل اجرای باندل روی ماشین به زمان ددلاین میخوره یا نه
								// یکی بیاد حساب کنه ماشین های مشغول بعدی باید حداقل کار را بتوانند تا قبل ار زمان ددلاین باندل انجام دهند
								// Branch and bound
								{
									//bool one_candidate_is_positive = false;
									//for ( auto & candidate_quality : candidate_qualities ) // اگر یک کاندیدی زودتر از ددلاین به پایان رساند تمام ماشین هایی که حداقل را نمی توانند رودتر از ددلاین به پایان برسانند رد می شوند
									//{
									//	if ( candidate_quality->d_estimate_quality_ratio >= 0 )
									//	{
									//		one_candidate_is_positive = true;
									//		break;
									//	}
									//}
									// اگر یک مثبت دیدی درب و داغون ها را بریز دور
									if ( one_candidate_is_positive )
									{
										bool this_vm_is_not_suitable = false;
										for ( auto & candidate_quality : candidate_qualities )
										{
											// اگر از همین باندل
											if ( candidate_quality.allocated_ptr->d_bundle_id == bundle_id ) // اگر یکی هست که می تونه باندل را قیل از ددلاین انجام بده دیگر همه ماشین هایی که نمی توانند حداقل را انجام دهند رد صلاحیت می شوند
											{
												double minimum_time_need = 0;

												minimum_time_need += provisionedVM->d_vmType->get_duration_datatransfer_on_wan( candidate_quality.allocated_ptr->d_estimate_TotalExportNodeOutputToGS_size_byte ); // خروجی لازم منتقل شود به گلوبال استوریج
												minimum_time_need += provisionedVM->d_vmType->get_duration_datatransfer_on_wan( candidate_quality.allocated_ptr->d_estimate_TotalTaskSize_byte ); // تسک ها بیاید در ماشین
												minimum_time_need += provisionedVM->d_vmType->get_execution_duration( candidate_quality.allocated_ptr->d_estimate_TotalProcessFlops );
												// نوشتن و خواندن روی حافظه اینقدر ناچیز است که نادیده گرفتم

												// قبلا یک تسک از دگ متفاوت اجرا کرده زمان تغییر کانتینر را می شود لحاظ کرد
												if ( provisionedVM->d_pLastContainer.allocated_ptr != NULL &&
													 provisionedVM->d_pLastContainer.allocated_ptr->d_container_dag_config.d_dag_type != ref_Node->d_pOwnerDAG->d_dag_config.d_dag_type )
												{
													minimum_time_need += ref_Node->d_pOwnerDAG->d_dag_config.d_container_initialization_time; // اینیکی کانتینرش بخواهد بیاید
												}

												if ( SETTINGS_FAST()->d_use_wait_policy &&
													 !provisionedVM->is_idle_vm() &&
													 provisionedVM->d_pLastContainer.allocated_ptr != NULL &&
													 provisionedVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr != NULL ) // در حال اجرای فعلی کی میرود پی کارش
												{
													// زمان اجرای قبلی
													double d_estimate_finished_prev_running_job_time_consume = std::max( (
														*provisionedVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr->d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() +
														provisionedVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr->d_estimate_TotalTimeConsume ) -
														SIMULATION_NOW_TIME , 0.0 );

													// زمان تا دور بعدی اسکجول
													double d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time =
														SETTINGS_FAST()->d_schedule_periode_delay_time -
														fmod( d_estimate_finished_prev_running_job_time_consume , SETTINGS_FAST()->d_schedule_periode_delay_time );

													// مدت زمانی که لازم است تا تسک قبلی تمام شود و تازه دور ایکجول بعدی هم بشود
													minimum_time_need += d_estimate_finished_prev_running_job_time_consume + d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time;
												}

												if ( SIMULATION_NOW_TIME + minimum_time_need > candidate_quality.allocated_ptr->calculate_bundle_softdeadline() )
												{
													this_vm_is_not_suitable = true;
												}

												break;
											}
										}
										if ( this_vm_is_not_suitable )
										{
											d_reject_vm_by_branch_and_bound++; // 2
											continue;
										}
									}
								} // ~Branch and bound

								if ( SETTINGS_FAST()->d_echo__scheduleOnVM__try_to_find_Provisioned_vm )
								{
									XBT_INFO_EMPTY_LINE();
									XBT_INFO( "----------------------------------------" , "" );
									XBT_INFO( "#(%d)  try to find Provisioned vm(%d) type(%s) at region(%s)" ,
											  __LINE__ , provisionedVM->d_vm_uniq_id , provisionedVM->d_vmType->get_vmtypeName() , provisionedVM->d_pAlias_owner_region->d_region_name.c_str() );
								}
							}

							VMQuality tempQuality;
							tempQuality.d_bundle_id = bundle_id;
							tempQuality.d_quality_dag_config = ref_Node->d_pOwnerDAG->d_dag_config;
							tempQuality.d_pAliasVM = provisionedVM;
							tempQuality.d_pAliasVmOwnerRegion = provisionedVM->d_pAlias_owner_region;
							tempQuality.d_vm_type = provisionedVM->d_vmType;
							tempQuality.d_pAlias_bundled_nodes_ordered = &bundled_nodes_ordered;
							tempQuality.d_pAlias_bundled_nodes_shuffled = &bundled_nodes_shuffled;
							tempQuality.d_stablish_vm = false;
							tempQuality.d_stablish_container = false; // if necessary fxn true it
							tempQuality.d_occupied_vm = false; // if necessary fxn true it
							//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
							if ( tempQuality.d_pAliasVM->calculate_quality_ratio_can_fit_in_idle_slot( &tempQuality ) )
							{
								if ( tempQuality.d_estimate_quality_ratio >= 0 )
								{
									one_candidate_is_positive = true;
								}

								if ( SETTINGS_FAST()->d_isSuitableForAnyMachine__ver != 2 )
								{
									_ONE_TIME_VAR();
									_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
								}

								//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
								if ( SETTINGS_FAST()->d_isSuitableForAnyMachine__ver == 0 )
								{
									isSuitableForAnyMachine = true;
								}
								else if ( SETTINGS_FAST()->d_isSuitableForAnyMachine__ver == 1 )
								{
									if ( tempQuality.d_estimate_quality_ratio >= 0 )
									{
										isSuitableForAnyMachine = true;
									}
								}

								//if ( SETTINGS_FAST()->d_echo__scheduleOnVM__dump_calc_ratio )
								//{
								//	XBT_INFO( "#(%d) calc quality ratio in Provisioned vm(%d)" , __LINE__  , pQuality->d_pVM->d_vm_uniq_id );
								//	pQuality->dump( true , false );
								//}

								// because lack of memory is so bad
								bool b_better_quality_is_in_list = false;
								//if ( tempQuality.d_occupied_vm == false ) // در ماشین های در حال کار اگر مثبت بود یعنی رعایت ددلاین می کرد کافی است برای درج در لیست
								{
									for ( auto & qual : candidate_qualities )
									{
										if ( qual.allocated_ptr->d_estimate_quality_ratio > tempQuality.d_estimate_quality_ratio ||
											 ( tempQuality.d_estimate_quality_ratio == qual.allocated_ptr->d_estimate_quality_ratio &&
											   qual.allocated_ptr->d_estimate_TotalCost < tempQuality.d_estimate_TotalCost ) )
										{
											b_better_quality_is_in_list = true;
										}
									}
								}
								if ( !b_better_quality_is_in_list )
								{
									if ( SETTINGS_FAST()->d_isSuitableForAnyMachine__ver == 2 ) // default
									{
										isSuitableForAnyMachine = true;
									}
									else if ( SETTINGS_FAST()->d_isSuitableForAnyMachine__ver == 3 )
									{
										if ( tempQuality.d_estimate_quality_ratio >= 0 ) // تا جایی توسعه می دهد باندل را که هم افزایش کوالیتی داشته باشیم و هم ددلاین حفظ شود
										{
											isSuitableForAnyMachine = true;
										}
									}
									else if ( SETTINGS_FAST()->d_isSuitableForAnyMachine__ver == 4 )
									{
										if ( tempQuality.d_estimate_quality_ratio >= 0 ) // تا جایی توسعه می دهد باندل را که هم افزایش کوالیتی داشته باشیم و هم ددلاین حفظ شود
										{
											isSuitableForAnyMachine = true;
										}
										// داریم میگیم اگر که باندل داره رشد میکنه حتما باید ددلاین رو رعایت کنه زیرا اگر رعایت نکنه هم همروندی اجرا را کم کردیم و هم در رعایت ددلاین مسامحه کردیم
										if ( bundled_nodes_ordered.size() > 1 && candidate_qualities.size() > 0 && tempQuality.d_estimate_quality_ratio < 0 )
										{
											continue;
										}
									}
									isSuitableForAnyProvisionedVM = true;
									if ( SETTINGS_FAST()->d_wait_policy_ver == 3 )
									{
										if ( tempQuality.d_occupied_vm )
										{
											one_wait_quality_is_in_queue = true;
										}
									}

									tempQuality.d_pAlias_bundled_nodes_ordered = NULL;
									tempQuality.d_pAlias_bundled_nodes_shuffled = NULL;

									tempQuality.d_pAllocated_bundled_nodes_ordered = __NEW( _vector_nodes );
									*tempQuality.d_pAllocated_bundled_nodes_ordered = bundled_nodes_ordered;
									tempQuality.d_pAllocated_bundled_nodes_shuffled = __NEW( _map_nodes );
									*tempQuality.d_pAllocated_bundled_nodes_shuffled = bundled_nodes_shuffled;

									candidate_qualities.push_back( CPointer< VMQuality >( tempQuality ) );

									tempQuality.d_pAllocated_bundled_nodes_ordered = NULL; // shared_ptr keep it
									tempQuality.d_pAllocated_bundled_nodes_shuffled = NULL; // shared_ptr keep it

									////vector< shared_ptr< VMQuality > > new_list;
									////for ( auto & quality : candidate_qualities )
									////{
									////	if ( quality->d_occupied_vm )
									////	{
									////		new_list.push_back(quality);
									////	}
									////}
									////new_list.push_back( make_shared< VMQuality >( tempQuality ) );

									////tempQuality.d_quality_bundled_nodes_ordered = NULL;
									////tempQuality.d_quality_bundled_nodes_shuffled = NULL;

									//////XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
									////candidate_qualities.clear();
									////candidate_qualities = new_list;
								}
							}
							////else
							////{
							////	if ( SETTINGS_FAST()->d_echo__scheduleOnVM__try_to_find_Provisioned_vm_calculate_return_false )
							////	{
							////		XBT_INFO( "#(%d)  add provisioned vm error msg()" , __LINE__ );
							////	}
							////}
							////XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
						}
					}
				}
				//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

				if ( SETTINGS_FAST()->d_wait_policy_ver == 3 )
				{
					if ( one_wait_quality_is_in_queue ) // اگر یک ماشین در حال کار کوالیتی مثبتی داشت خوب دیگر کار تمام است و هر چه باندل را ادامه دهیم بازهم نود اول دچار ویت می شود
					{
						break;
					}
				}

				// find suitable vm in UNprovisioned
				if ( 1 )
				{
					////SECTION_TIME("");
					// find one copy of any vm type to assign
					vector< CPointer< ClassVM > > each_type_in_each_region;

					//map< int , map< int , int > > control_region_type;
					for ( const auto & preg : d_allocated_regions )
					{
						//control_region_type[ reg->d_regionID ];
						for ( auto & eachVMType : VMachine::get_type_instances() )
						{
							if ( !SETTINGS_FAST()->d_use_least_price_vm_to_lease_new_vm )
							{
								_ONE_TIME_VAR();
								_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
							}

							// اگر هیچ ماشین اجاره شده ای در دسترس نبود سعی کن کف ماشین را طبق خواسته ددلاین دگ در نظر بگیری
							// اگر ماشین های اجاره شده در دسترس بود خب بزار ماشین های ارزان تر هم رقابت کنند
							if ( SETTINGS_FAST()->d_use_least_price_vm_to_lease_new_vm && isSuitableForAnyProvisionedVM == false )
							{
								if ( eachVMType.d_type < ref_Node->d_pOwnerDAG->d_least_price_vmp_type )
								{
									continue;
								}
							}

							CPointer< ClassVM > new_temp_vm;
							new_temp_vm.allocate();
							new_temp_vm.allocated_ptr->d_pAlias_owner_region = preg.allocated_ptr;
							new_temp_vm.allocated_ptr->d_vmType = &eachVMType;
							new_temp_vm.allocated_ptr->d_leased_cycle_mgr.d_pAliasVM = new_temp_vm.allocated_ptr;

							each_type_in_each_region.push_back( new_temp_vm );

						}
					}

					//XBT_INFO_EMPTY_LINE();
					//XBT_INFO( "#(%d) each_type_in_each_region(%lu)" , __LINE__ , each_type_in_each_region.size() );
					for ( auto & temp_unprovisionedVM : each_type_in_each_region )
					{
						// حداقل اجرای باندل روی ماشین به زمان ددلاین میخوره یا نه
						// یکی بیاد حساب کنه ماشین های مشغول بعدی باید حداقل کار را بتوانند تا قبل ار زمان ددلاین باندل انجام دهند
						// branch and bound
						{
							////bool one_candidate_is_positive = false;
							////for ( auto & candidate_quality : candidate_qualities ) // اگر یک کاندیدی زودتر از ددلاین به پایان رساند تمام ماشین هایی که حداقل را نمی توانند رودتر از ددلاین به پایان برسانند رد می شوند
							////{
							////	if ( candidate_quality->d_estimate_quality_ratio >= 0 )
							////	{
							////		one_candidate_is_positive = true;
							////		break;
							////	}
							////}
							// اگر یک مثبت دیدی درب و داغون ها را بریز دور
							if ( one_candidate_is_positive )
							{
								bool this_vm_is_not_suitable = false;
								for ( auto & candidate_quality : candidate_qualities )
								{
									// اگر از همین باندل
									if ( candidate_quality.allocated_ptr->d_bundle_id == bundle_id ) // اگر یکی هست که می تونه باندل را قیل از ددلاین انجام بده دیگر همه ماشین هایی که نمی توانند حداقل را انجام دهند رد صلاحیت می شوند
									{
										double minimum_time_need = 0;

										minimum_time_need += temp_unprovisionedVM.allocated_ptr->d_vmType->get_duration_datatransfer_on_wan( candidate_quality.allocated_ptr->d_estimate_TotalExportNodeOutputToGS_size_byte ); // خروجی لازم منتقل شود به گلوبال استوریج
										minimum_time_need += temp_unprovisionedVM.allocated_ptr->d_vmType->get_duration_datatransfer_on_wan( candidate_quality.allocated_ptr->d_estimate_TotalTaskSize_byte ); // تسک ها بیاید در ماشین
										minimum_time_need += temp_unprovisionedVM.allocated_ptr->d_vmType->get_execution_duration( candidate_quality.allocated_ptr->d_estimate_TotalProcessFlops );
										// نوشتن و خواندن روی حافظه اینقدر ناچیز است که نادیده گرفتم

										minimum_time_need += ref_Node->d_pOwnerDAG->d_dag_config.d_container_initialization_time; // اینیکی کانتینرش بخواهد بیاید
										minimum_time_need += SETTINGS_FAST()->d_VIRTUALMACHINE_WARMUP_TIME_SEC;

										if ( SIMULATION_NOW_TIME + minimum_time_need > candidate_quality.allocated_ptr->calculate_bundle_softdeadline() )
										{
											this_vm_is_not_suitable = true;
										}

										break;
									}
								}
								if ( this_vm_is_not_suitable )
								{
									d_reject_vm_by_branch_and_bound++; // 3
									continue;
								}
							}
						} // ~branch and bound

						if ( SETTINGS_FAST()->d_echo__scheduleOnVM__try_to_find_UNprovisioned_vm )
						{
							XBT_INFO_EMPTY_LINE();
							XBT_INFO( "----------------------------------------" , "" );
							XBT_INFO( "#(%d)  try to find UNprovisioned vm(%d) type(%s) at region(%s)" ,
									  __LINE__ , temp_unprovisionedVM.allocated_ptr->d_vm_uniq_id , temp_unprovisionedVM.allocated_ptr->d_vmType->get_vmtypeName() , temp_unprovisionedVM.allocated_ptr->d_pAlias_owner_region->d_region_name.c_str() );
						}

						VMQuality tempQuality;
						tempQuality.d_bundle_id = bundle_id;
						tempQuality.d_quality_dag_config = ref_Node->d_pOwnerDAG->d_dag_config;
						tempQuality.d_pAliasVM = temp_unprovisionedVM.allocated_ptr;
						tempQuality.d_pAliasVmOwnerRegion = temp_unprovisionedVM.allocated_ptr->d_pAlias_owner_region;
						tempQuality.d_vm_type = temp_unprovisionedVM.allocated_ptr->d_vmType;
						tempQuality.d_pAlias_bundled_nodes_ordered = &bundled_nodes_ordered;
						tempQuality.d_pAlias_bundled_nodes_shuffled = &bundled_nodes_shuffled;
						//tempQuality.d_quality_bundled_nodes_ordered = make_shared< vector< shared_ptr< DAGNode > > >( bundled_nodes_ordered );
						//tempQuality.d_quality_bundled_nodes_shuffled = make_shared< map < eNodeID , shared_ptr< DAGNode > > >( bundled_nodes_shuffled );
						tempQuality.d_stablish_vm = true;
						tempQuality.d_stablish_container = true;
						tempQuality.d_occupied_vm = false; // if necessary fxn true it

						//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
						if ( tempQuality.d_pAliasVM->calculate_quality_ratio_can_fit_in_idle_slot( &tempQuality ) )
						{
							if ( tempQuality.d_estimate_quality_ratio >= 0 )
							{
								one_candidate_is_positive = true;
							}

							//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
							if ( SETTINGS_FAST()->d_isSuitableForAnyMachine__ver == 0 )
							{
								isSuitableForAnyMachine = true;
							}
							else if ( SETTINGS_FAST()->d_isSuitableForAnyMachine__ver == 1 )
							{
								if ( tempQuality.d_estimate_quality_ratio >= 0 )
								{
									isSuitableForAnyMachine = true;
								}
							}

							//if ( SETTINGS_FAST()->d_echo__scheduleOnVM__dump_calc_ratio )
							//{
							//	XBT_INFO( "#(%d) calc quality ratio in UNprovisioned vm(%d)" , __LINE__ , pQuality->d_pVM->d_vm_uniq_id );
							//	pQuality->dump( true , false );
							//}

							bool b_better_quality_is_in_list = false;
							for ( auto & qual : candidate_qualities )
							{
								if ( qual.allocated_ptr->d_estimate_quality_ratio > tempQuality.d_estimate_quality_ratio ||
									 ( tempQuality.d_estimate_quality_ratio == qual.allocated_ptr->d_estimate_quality_ratio &&
									   qual.allocated_ptr->d_estimate_TotalCost < tempQuality.d_estimate_TotalCost ) )
								{
									b_better_quality_is_in_list = true;
								}
							}
							if ( !b_better_quality_is_in_list )
							{
								if ( SETTINGS_FAST()->d_isSuitableForAnyMachine__ver == 2 )
								{
									isSuitableForAnyMachine = true;
								}
								else if ( SETTINGS_FAST()->d_isSuitableForAnyMachine__ver == 3 )
								{
									if ( tempQuality.d_estimate_quality_ratio >= 0 )// تا جایی توسعه می دهد باندل را که هم افزایش کوالیتی داشته باشیم و هم ددلاین حفظ شود
									{
										isSuitableForAnyMachine = true;
									}
								}
								else if ( SETTINGS_FAST()->d_isSuitableForAnyMachine__ver == 4 )
								{
									if ( tempQuality.d_estimate_quality_ratio >= 0 )// تا جایی توسعه می دهد باندل را که هم افزایش کوالیتی داشته باشیم و هم ددلاین حفظ شود
									{
										isSuitableForAnyMachine = true;
									}
									// داریم میگیم اگر که باندل داره رشد میکنه حتما باید ددلاین رو رعایت کنه زیرا اگر رعایت نکنه هم همروندی اجرا را کم کردیم و هم در رعایت ددلاین مسامحه کردیم
									if ( bundled_nodes_ordered.size() > 1 && candidate_qualities.size() > 0 && tempQuality.d_estimate_quality_ratio < 0 )
									{
										continue;
									}
								}

								tempQuality.d_pAliasVM = NULL; // caution
								// اصلا می گذارم آخر سر اگر ماشین قرار بود اجاره شود و وجود نداشت ارور می دهم

								tempQuality.d_pAlias_bundled_nodes_ordered = NULL;
								tempQuality.d_pAlias_bundled_nodes_shuffled = NULL;

								tempQuality.d_pAllocated_bundled_nodes_ordered = __NEW( _vector_nodes );
								*tempQuality.d_pAllocated_bundled_nodes_ordered = bundled_nodes_ordered;
								tempQuality.d_pAllocated_bundled_nodes_shuffled = __NEW( _map_nodes );
								*tempQuality.d_pAllocated_bundled_nodes_shuffled = bundled_nodes_shuffled;

								////bool b_find_unprivision_vm = false;
								////for ( auto & unprovisionvm : pQuality->d_pVM->d_owner_region->d_local_vms )
								////{
								////	if ( unprovisionvm->is_provisioned() == false && unprovisionvm->d_vmType->d_type == pQuality->d_pVM->d_vmType->d_type )
								////	{
								////		pQuality->d_pVM = unprovisionvm;
								////		//candidate_qualities.push_back( pQuality );
								////		//b_find_unprivision_vm = true;
								////		break;
								////	}
								////}
								////tempQuality.d_pVM = NULL;
								candidate_qualities.push_back( CPointer<VMQuality>( tempQuality ) );

								tempQuality.d_pAllocated_bundled_nodes_ordered = NULL; // shared_ptr keep it
								tempQuality.d_pAllocated_bundled_nodes_shuffled = NULL; // shared_ptr keep it
								////if ( !b_find_unprivision_vm )
								////{
								////	if ( SETTINGS()->d_warn_if_some_vm_type_are_not_avaiable_for_leased )
								////	{
								////		XBT_INFO( "some vm type are not available for lease" , "" );
								////		XBT_INFO( "region id(%d) , vmtype(%s) not exist" , pQuality->d_pVM->d_owner_region->d_regionID , pQuality->d_pVM->d_vmType->get_vmtypeName().c_str() );
								////	}
								////}
							}
						}
						////else
						////{
						////	if ( SETTINGS_FAST()->d_echo__scheduleOnVM__try_to_find_UNprovisioned_vm )
						////	{
						////		XBT_INFO( "#(%d)  cannot(%s)" , __LINE__ , tempQuality.d_ret_message.c_str() );
						////	}
						////}
						////XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
						////}
					}
				}
				//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
			}

			//XBT_INFO( "#(%d)  " , __LINE__  );
			//XBT_INFO( "#(%d)  , forward_and_seen_node_weight %f" , __LINE__  , forward_and_seen_node_weight[ ref_Node->d_node_uniq_id ] );

			if ( SETTINGS_FAST()->d_max_bundle_capacity > 0 && (int)bundled_nodes_shuffled.size() >= SETTINGS_FAST()->d_max_bundle_capacity )
			{
				_ONE_TIME_VAR();
				_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
				break;
			}

			if ( !isSuitableForAnyMachine ) // agar in node ghabeleh mohasebe nist hazf mi shavad vali digar masir hayeh dijkestra baz hast va anha edameh dahand
			{
				if ( SETTINGS_FAST()->d_break_after_bundle_not_suitable )
				{
					if ( candidate_qualities.size() > 0 )
					{
						break; // اگر حداقل یک ماشین یافت شد برای ین باندل و دیگر ادامه دادن و افزایش سایز باندل نمی تواند به یافتن کوالیبی با در نظر گرفتن ددلاین منجر شود خب دیگه ادامه نده
						// تا جایی ادامه می دهیم که اولین شکست در یافت باندل قابل درج در یک اسلات خالی پیدا شود
						// در گراف های بزرگ زیر باندل بزرگ ضربدر تعداد ماشین ها حالات زیادی می شود که یاعث پریدن بیرون برنامه می شود
					}
				}
				else
				{
					_ONE_TIME_VAR();
					_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
				}

				bundled_nodes_shuffled.erase( current_node->d_node_uniq_id );
				bundled_nodes_ordered.pop_back(); // remove last insertion
			}
			else
			{
				////SECTION_TIME();
				//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
				for ( auto & ref_lnk : current_node->d_links )
				{
					if ( !( ref_lnk->d_pDestination )->d_pRuntime->d_taskProccessed ) // parent task in under process so child task is not scheduled
					{
						bool bParentIsProcessedOrInQueue = true; // اگر همه پدر ها پروسس شده بود
						for ( auto & ref_parent_node : ( ref_lnk->d_pDestination )->d_parents )
						{
							if ( !ref_parent_node->d_pRuntime->d_taskProccessed && bundled_nodes_shuffled.count( ref_parent_node->d_node_uniq_id ) == 0/*parent not in bundle*/ )
							{
								// agar az node dakhele bundle yeki child e dasht keh parent 
								// digary kharej bundle dasht on child ghabele exec nist
								bParentIsProcessedOrInQueue = false;
								break;
							}
						}
						//XBT_INFO( "#(%d)    has non processed parent(%d)" , __LINE__ , bParentIsProcessedOrInQueue );
						if ( bParentIsProcessedOrInQueue )
						{
							// agar kast 1 / network_usage kheily kochach mi shavad mi tavanam 
							// barayeh edge ha yek vazn normal shodeh bedast biavaram keh yali vazn kamtar 
							// begirad keh edge weight bishtary darad injory aadad kheily kochak nemi shavad
							if ( forward_and_seen_node_weight.count( ( ref_lnk->d_pDestination )->d_node_uniq_id ) == 0 )
							{
								forward_and_seen_node_weight[ ( ref_lnk->d_pDestination )->d_node_uniq_id ] = forward_and_seen_node_weight[ current_node->d_node_uniq_id ]/*current weight*/ + ( ref_lnk->normalized_inverse_network_usage )/*path weight*/;
							}
							else
							{
								XBT_INFO( "#(%d)    second arrive to node  prev weight(%f)  link weight(%f)  old wight(%f)" , __LINE__ ,
										  forward_and_seen_node_weight[ current_node->d_node_uniq_id ] , ( ref_lnk->normalized_inverse_network_usage ) , forward_and_seen_node_weight[ ( ref_lnk->d_pDestination )->d_node_uniq_id ] );
								if ( forward_and_seen_node_weight[ current_node->d_node_uniq_id ] + ( ref_lnk->normalized_inverse_network_usage ) < forward_and_seen_node_weight[ ( ref_lnk->d_pDestination )->d_node_uniq_id ] )
								{
									forward_and_seen_node_weight[ ( ref_lnk->d_pDestination )->d_node_uniq_id ] = forward_and_seen_node_weight[ current_node->d_node_uniq_id ] + ( ref_lnk->normalized_inverse_network_usage );
								}
								XBT_INFO( "#(%d)    new weight(%f)" , __LINE__ , forward_and_seen_node_weight[ ( ref_lnk->d_pDestination )->d_node_uniq_id ] );
							}
							node_sort_order[ ref_lnk->d_pDestination->d_node_uniq_id ] = (int)node_sort_order.size();
							p_que.push( ref_lnk->d_pDestination );
						}
					}
				}
				//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
			}
		}
	}

	//SECTION_TIME("");

	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	//SECTION_ALIVE_CHECK("");

	// تعداد ماشین هایی که درشان می گردیم ممکن است زیاد شود زیرا در هر مرحله دایکسترا همه انواع در همه ریجن ها چک می شود
	//if ( SETTINGS_FAST()->d_echo__scheduleOnVM__details )
	//{
	//	int privisionedcount = 0;
	//	int unprovisionedcount = 0;
	//	for ( shared_ptr<VMQuality> & qqq : candidate_qualities )
	//	{
	//		if ( qqq->d_pVM->is_provisioned() ) privisionedcount++;
	//		if ( !qqq->d_pVM->is_provisioned() ) unprovisionedcount++;
	//	}
	//
	//	XBT_INFO_EMPTY_LINE();
	//	XBT_INFO( "----------------------------------------" , "" );
	//	XBT_INFO( "#(%d)  find_qualities provisioned(%d) , unprovisioned(%d)" , __LINE__ , privisionedcount , unprovisionedcount );
	//}
	//if ( SETTINGS_FAST()->d_dump_all_qualities_in_schedule_node )
	//{
	//	XBT_INFO_EMPTY_LINE();
	//	XBT_INFO( "----------------------------------------" , "" );
	//	for ( const shared_ptr< VMQuality > & qu : candidate_qualities )
	//	{
	//		if ( qu->d_pVM->is_idle_vm() )
	//		{
	//			XBT_INFO( "vm(%s)(%d)(%s)  %s  qr(%f)  x10000qr(%f) cost(%f)" ,
	//					  qu->d_pVM->d_vmType->get_vmtypeName().c_str() , qu->d_pVM->d_vm_uniq_id , qu->d_pVM->d_owner_region->d_region_name.c_str() ,
	//					  qu->d_stablish_vm ? "unprovision" : "provision" , qu->d_estimate_quality_ratio , qu->d_estimate_quality_ratio * 10000 ,
	//					  qu->d_estimate_TotalCost );
	//			//qu->dump( false , true );
	//		}
	//	}
	//	//XBT_INFO( "----------------------------------------" , "" );
	//}

	if ( candidate_qualities.size() == 0 )
	{
		if ( SETTINGS_FAST()->d_warn_impossible_condition )
		{
			_ONE_TIME_VAR();
			_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
		}
		return;
	}

	if ( d_last_candidate_quality_count > d_max_candidate_quality_count )
	{
		d_max_candidate_quality_count = d_last_candidate_quality_count;
	}
	d_last_candidate_quality_count = candidate_qualities.size();

	// برای مواردی که کیفیت صفر است نتظار داریم در بین صفر ها اونی انتخاب بشه که هزینه را کم کنه
	if ( SETTINGS_FAST()->d_shuffle_candidate_qualities )
	{
		std::random_device rd;
		std::mt19937 generator( rd() );
		std::shuffle( candidate_qualities.begin() , candidate_qualities.end() , generator );

		// این شافل زیر خراب کار می کند و بیشتر روی ریجن آخر موارد را می ریزد
	}
	else
	{
		_ONE_TIME_VAR();
		_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
	}

	if ( SETTINGS_FAST()->d_sort_candidate_qualities_ver == 1 )
	{
		std::sort( candidate_qualities.begin() , candidate_qualities.end() , []( const CPointer< VMQuality > & a , const CPointer< VMQuality > & b )
		{
			if ( a.allocated_ptr->d_estimate_quality_ratio == b.allocated_ptr->d_estimate_quality_ratio )
			{
				return a.allocated_ptr->d_estimate_TotalCost < b.allocated_ptr->d_estimate_TotalCost;
			}
			return a.allocated_ptr->d_estimate_quality_ratio > b.allocated_ptr->d_estimate_quality_ratio;
		} );
	}
	else if ( SETTINGS_FAST()->d_sort_candidate_qualities_ver == 0 )
	{
		_ONE_TIME_VAR();
		_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
	}
	else
	{
		_ONE_TIME_VAR();
		_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
	}

	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	list< CPointer< VMQuality > > wait_until_next_schedule_time;
	list< CPointer< VMQuality > > other_scheduled_quality;

	for ( auto & qual : candidate_qualities )
	{
		switch ( SETTINGS_FAST()->d_wait_policy_ver )
		{
			case 1:
			case 2:
			case 3: // wait quality priority is first
			{
				_ONE_TIME_VAR();
				_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
				if ( qual.allocated_ptr->d_occupied_vm )
				{
					if ( SETTINGS_FAST()->d_use_wait_policy /* && qual->d_estimate_quality_ratio > 0*/ )
					{
						wait_until_next_schedule_time.push_back( qual ); // فقط مواردی که با ویت کردن زمان ددلاین گذر نمی کند
					}
				}
				else
				{
					other_scheduled_quality.push_back( qual );
				}
				break;
			}
			case 4: // all quality race in ratio . if wait win so waiting . این یعنی یک ماشنی قراره آزاد بشه که میتونه خیلی خوب باندل را هندل کند
			{
				other_scheduled_quality.push_back( qual );
				break;
			}
			default:
			{
				_ONE_TIME_VAR();
				_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
				break;
			}
		}
	}

	CPointer< VMQuality > top_quality;

	////if ( SETTINGS_FAST()->d_use_wait_policy ) // khob ageh ghablan wait policy bodeh basheh wait quality ha entekhab mi shavad
	{
		if ( top_quality.allocated_ptr == NULL && wait_until_next_schedule_time.size() > 0 )
		{
			top_quality = wait_until_next_schedule_time.front();
		}
	}
	if ( top_quality.allocated_ptr == NULL && other_scheduled_quality.size() > 0 )
	{
		top_quality = other_scheduled_quality.front();
	}

	if ( top_quality.allocated_ptr == NULL )
	{
		//if ( CNFG()->d_warn_impossible_condition )
		{
			XBT_INFO_EMPTY_LINE();
			XBT_INFO( "#(%d)  assert . fault . error . impossible . top quality is null" , __LINE__ );
		}
		return; // not possible . DAGGenerator retru again later .. warning . may cause trap in obe bundle
	}

	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	if ( top_quality.allocated_ptr->d_occupied_vm )
	{
		if ( SETTINGS_FAST()->d_echo__scheduleOnVM__top_quality_with_wait_policy_details )
		{
			XBT_INFO_EMPTY_LINE();
			XBT_INFO( "#(%d) top_quality wait policy" , __LINE__ );
			top_quality.allocated_ptr->dump( true , SETTINGS_FAST()->d_echo__scheduleOnVM__top_quality_with_wait_policy_details_full_dag_dump );
		}

		this->d_statistics[ AeStatisticElement::stt_bundle_scheduling_result ][ STATISTICS_bundle_wait_count ].bundle_wait_count += 1; // کلا یک باندل ویت خورد
		//top_quality->d_quality_bundled_nodes_ordered->front()->d_pOwnerDAG->d_schedul_bundle_wait_count++;

		this->d_statistics[ AeStatisticElement::stt_dag_waiting_count ][ STATISTICS_INT( ref_Node->d_pOwnerDAG->d_dag_uniq_id ) ].dag_waiting_count += 1; // دگ خاصی ویت خورد

		return; // scheduled on next time
	}

	this->d_statistics[ AeStatisticElement::stt_bundle_task_count_frequency ][ STATISTICS_INT( top_quality.allocated_ptr->d_pAllocated_bundled_nodes_ordered->size() ) ].bundle_task_count += 1;
	this->d_statistics[ AeStatisticElement::stt_quality_factor_sign_count ][ STATISTICS_INT( ( top_quality.allocated_ptr->d_estimate_quality_ratio > 0 ) ? 1 : ( ( top_quality.allocated_ptr->d_estimate_quality_ratio < 0 ) ? -1 : 0 ) ) ].quality_factor_sign_count += 1;

	//if ( 1 )
	//{
	//	bool sign_top_quality = top_quality->d_estimate_quality_ratio >= 0;
	//	bool is_there_another_positive_quality = false;
	//	bool is_there_another_provisioned_quality = false;
	//	bool choosed_quality_stablish_vm = top_quality->d_stablish_vm;
	//	for ( auto & qual : other_scheduled_quality )
	//	{
	//		if ( qual->d_quality_id != top_quality->d_quality_id )
	//		{
	//			if ( qual->d_estimate_quality_ratio >= 0 )
	//			{
	//				is_there_another_positive_quality = true;
	//			}
	//			if ( !qual->d_stablish_vm )
	//			{
	//				is_there_another_provisioned_quality = true;
	//			}
	//		}
	//	}
	//	d_top_quality_vm_stat[ string_format( "topsign(%d),anotherposqual(%d),anotherprovqual(%d),stbvm(%d)" , sign_top_quality , is_there_another_positive_quality , is_there_another_provisioned_quality , choosed_quality_stablish_vm ) ] += 1;
	//}

	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

	// assign real vm to new leased vm
	while ( top_quality.allocated_ptr->d_pAliasVM == NULL ) // new vm allocation
	{
		for ( auto & unprovisionvm : d_allocated_available_VMs )
		{
			if ( !unprovisionvm->is_provisioned() )
			{
				top_quality.allocated_ptr->d_pAliasVM = unprovisionvm;
				assert( top_quality.allocated_ptr->d_pAliasVM->d_vmType == NULL );
				assert( top_quality.allocated_ptr->d_pAliasVM->d_pAlias_owner_region == NULL );
				//top_quality->d_pVM->d_vmType = top_quality->d_vm_type;
				break;
			}
		}
		if ( top_quality.allocated_ptr->d_pAliasVM == NULL )
		{
			//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
			if ( SETTINGS_FAST()->d_warn_if_some_vm_type_are_not_avaiable_for_leased )
			{
				int occopied_vm = 0;
				for ( auto & reg : this->d_allocated_regions )
				{
					occopied_vm += reg.allocated_ptr->d_alias_local_vms.size();
				}


				XBT_INFO_EMPTY_LINE();
				XBT_INFO( "#(%d)!!!!no free vm available at region(%d) occopied_vm(%d)" , __LINE__ , top_quality.allocated_ptr->d_pAliasVmOwnerRegion->d_regionID , occopied_vm );
				return;
			}
		}
	}

	assert( top_quality.allocated_ptr->d_pAliasVM != NULL ); // 1402 08 14

	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

	// مسئله همزمانی برای استفاده از یک ماشین پیش نمی اید زیرا تابع اسکجول بصورت سری صدا زده می شود
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	if ( !top_quality.allocated_ptr->d_pAliasVM->is_provisioned() )
	{
		top_quality.allocated_ptr->d_pAliasVM->configureVM_ForProvisioning( top_quality.allocated_ptr );
	}
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	if ( top_quality.allocated_ptr->d_stablish_container )
	{
		if ( top_quality.allocated_ptr->d_pAliasVM->d_pLastContainer.allocated_ptr != NULL )
		{
			top_quality.allocated_ptr->d_pAliasVM->d_pLastContainer.allocated_ptr->d_obsolete_container_time = SIMULATION_NOW_TIME; // زدن تاریخ اتمام قبلی
			this->d_statistics[ AeStatisticElement::stt_container_stat ][ STATISTICS_change_container_count ].change_container_count += 1;
		}
		else
		{
			this->d_statistics[ AeStatisticElement::stt_container_stat ][ STATISTICS_first_time_container_count ].first_time_container_count += 1;
		}

		// old container going to hell
		top_quality.allocated_ptr->d_pAliasVM->d_pLastContainer.allocate();
		top_quality.allocated_ptr->d_pAliasVM->d_pLastContainer.allocated_ptr->d_container_dag_config = ref_Node->d_pOwnerDAG->d_dag_config;
		top_quality.allocated_ptr->d_pAliasVM->d_pLastContainer.allocated_ptr->d_dag_id_assigned = ref_Node->d_pOwnerDAG->d_dag_uniq_id; // very temp
		static int _containerID = 0;
		top_quality.allocated_ptr->d_pAliasVM->d_pLastContainer.allocated_ptr->d_container_uniq_id = ++_containerID;
		//d_all_container.push_back( top_quality->d_pVM->d_pLastContainer );
	}
	else
	{
		this->d_statistics[ AeStatisticElement::stt_container_stat ][ STATISTICS_keep_container_count ].keep_container_count += 1;
	}

	VMQuality * pQuality = top_quality.allocated_ptr;
	top_quality.allocated_ptr->d_pAliasVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality = top_quality;
	this->d_statistics[ AeStatisticElement::stt_bundle_scheduling_result ][ STATISTICS_bundle_scheduled_count ].bundle_scheduled_count += 1;
	//top_quality->d_quality_bundled_nodes_ordered->front()->d_pOwnerDAG->d_schedul_bundle_scheduled_count++;

	//top_quality->start_scheduling_quality( top_quality ); // set d_taskScheduled_in_vm to true

	if ( SETTINGS_FAST()->d_echo__scheduleOnVM__top_quality_non_wait_policy_details )
	{
		XBT_INFO_EMPTY_LINE();
		XBT_INFO( "top_quality non wait quality vm(%s)(%d)(%s)  %s  qr(%f)  x10000qr(%f) cost(%f)" ,
				  pQuality->d_pAliasVM->d_vmType->get_vmtypeName() , pQuality->d_pAliasVM->d_vm_uniq_id , pQuality->d_pAliasVM->d_pAlias_owner_region->d_region_name.c_str() ,
				  pQuality->d_stablish_vm ? "unprovision" : "provision" , pQuality->d_estimate_quality_ratio , pQuality->d_estimate_quality_ratio * 10000 ,
				  pQuality->d_estimate_TotalCost );

		pQuality->dump( true , SETTINGS()->d_echo__scheduleOnVM__top_quality_non_wait_policy_details_full_dag_dump );
	}

	// TOCHEKC . زمان بین استارت تسک در اینجا با استارت در داخل اکتور نباید تفاوت زیادی بکند اصلا باید صفر باشد
	// یک نکته اگر شروع تسک را اینجا بگیرم خب زمان اجاره ماشین و کانتینر می افتد گردن تسک
	// آیا اشکال دارد و ایا باید زمان اجاره ماشین و کانتینر و انتقال دیتا گردن تسک باشد
	// راجع بهش باید فکر کنم
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

	if ( SETTINGS_FAST()->d_echo__scheduleOnVM__top_quality_non_wait_policy_details )
	{
		//static map< int , map< int , int > > _count_region;
		//_count_region[ top_quality->d_pVM->d_owner_region->d_regionID ][ top_quality->d_pVM->d_vmType->d_type ] += 1;
		////XBT_INFO( "----------------------------" );
		//for ( auto & aa : _count_region )
		//{
		//	for ( auto & bb : aa.second )
		//	{
		//		XBT_INFO( "region(%d) type(%d) count(%d)" , aa.first , bb.first , bb.second );
		//	}
		//}
		//XBT_INFO( "----------------------------" );
	}

	std::function<void()> f = std::bind( &ClassVM::callback_execute_bundle_on_container , pQuality->d_pAliasVM );

	if ( pQuality->d_pAliasVM->d_pVmActorKeeper != NULL )
	{
		pQuality->d_pAliasVM->d_pVmActor = pQuality->d_pAliasVM->d_pVmActorKeeper;
		
		pQuality->d_pAliasVM->d_pVmActor->restart();
	}
	else
	{
		pQuality->d_pAliasVM->d_pVmActor = sg4::Actor::create( string_format( "a%03d" , ( *pQuality->d_pAllocated_bundled_nodes_ordered )[ 0 ]->d_node_uniq_id ) , pQuality->d_pAliasVM->d_pVmHost , f );
	}

	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	pQuality->start_scheduling_quality( pQuality ); // set d_taskScheduled_in_vm to true

	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

	CATCH_TRY
}

void Vmm::EPSM_algorithm_scheduleOnVM( DAGNode * ref_Node )
{
	BEGIN_TRY
	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

	std::lock_guard lock(*d_critical_section_entered_mutex);

	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );

	int idle_has_container_has_input_before_deadline__count = 0;
	int idle_has_container_before_deadline__count = 0;
	int idle_before_deadline__count = 0;
	//int new_vm_can_finish_before_deadline__count = 0;
	//int new_vm_cant_finish_before_deadline__count = 0;

	if ( SETTINGS()->d_echo__scheduleOnVM__schedule_new_node )
	{
		//XBT_INFO_EMPTY_LINE();
		XBT_INFO( "----------------------------------------" , "" );
		XBT_INFO( "#(%d) schedule node(%s) @%s" , __LINE__ , ref_Node->d_allocated_node_name ,
					ref_Node->d_pOwnerDAG->get_name().c_str()
		);
	}

	vector< DAGNode * > d_pTemp_bundled_nodes_ordered;
	d_pTemp_bundled_nodes_ordered.push_back(ref_Node);
	map< eNodeID , DAGNode * > d_pTemp_bundled_nodes_shuffled;
	d_pTemp_bundled_nodes_shuffled[ref_Node->d_node_uniq_id] = ref_Node;

	list< CPointer< VMQuality > > candidate_qualities;
	map< int , int > _vm_type_count_in_qualities; // statistics

	bool find_idle_has_container_has_input_before_deadline = false;
	bool find_idle_has_container_before_deadline = false;

	for ( auto & provisionedVM : d_allocated_available_VMs )
	{
		if ( provisionedVM->is_provisioned() && provisionedVM->is_idle_vm() )
		{
			// branch and bound
			if ( find_idle_has_container_has_input_before_deadline || find_idle_has_container_before_deadline )
			{
				if ( provisionedVM->d_pLastContainer.allocated_ptr->d_container_dag_config.d_dag_type != ref_Node->d_pOwnerDAG->d_dag_config.d_dag_type )
				{
					d_reject_vm_by_branch_and_bound++; // 4
					continue;
				}
				if ( find_idle_has_container_has_input_before_deadline )
				{
					bool branch_and_bound_vm_on_lack_of_parent_data = false;
					for ( auto & parents : ref_Node->d_parents )
					{
						if ( provisionedVM->d_pLastContainer.allocated_ptr->d_in_ram_task_output.count( parents->d_node_uniq_id ) == 0 )
						{
							branch_and_bound_vm_on_lack_of_parent_data = true;
							break;
						}
					}
					if ( branch_and_bound_vm_on_lack_of_parent_data )
					{
						d_reject_vm_by_branch_and_bound++; // 5
						continue;
					}
				}
			}

			_vm_type_count_in_qualities[provisionedVM->d_vmType->d_type] += 1;

			VMQuality tempQuality;
			tempQuality.d_quality_dag_config = ref_Node->d_pOwnerDAG->d_dag_config;
			tempQuality.d_pAliasVM = provisionedVM;
			tempQuality.d_pAliasVmOwnerRegion = provisionedVM->d_pAlias_owner_region;
			tempQuality.d_vm_type = provisionedVM->d_vmType;

			tempQuality.d_pAlias_bundled_nodes_ordered = &d_pTemp_bundled_nodes_ordered;
			tempQuality.d_pAlias_bundled_nodes_shuffled = &d_pTemp_bundled_nodes_shuffled;

			//pQuality->d_quality_bundled_nodes_ordered = new vector< DAGNode * >();
			//pQuality->d_quality_bundled_nodes_ordered->push_back( ref_Node );
			//pQuality->d_quality_bundled_nodes_shuffled = new map< eNodeID , DAGNode * >();
			//( *pQuality->d_quality_bundled_nodes_shuffled )[ ref_Node->d_node_uniq_id ] = ref_Node;

			tempQuality.d_stablish_vm = false;
			tempQuality.d_stablish_container = false; // if necessary fxn true it
			tempQuality.d_occupied_vm = false; // if necessary fxn true it
			tempQuality.d_EPSM_algorithm = true;
			if ( tempQuality.d_pAliasVM->calculate_quality_for_EPSM_algorithm( &tempQuality ) )
			{
				if ( !tempQuality.d_stablish_container )
				{
					find_idle_has_container_before_deadline = true;
					if ( tempQuality.d_estimate_kasr_TotalResidentInputs_size_byte > 0 )
					{
						find_idle_has_container_has_input_before_deadline = true;
					}
				}
				tempQuality.d_pAlias_bundled_nodes_ordered = NULL;
				tempQuality.d_pAlias_bundled_nodes_shuffled = NULL;
				
				candidate_qualities.push_back( CPointer<VMQuality>(tempQuality) );
			}
			else
			{
				////if ( pQuality->d_pVM->d_pLastContainer->d_container_dag_type.d_dag_type == ref_Node->d_pOwnerDAG->d_dag_type.d_dag_type )
				////{
				////	for ( auto & inram : pQuality->d_pVM->d_pLastContainer->d_in_ram_task_output )
				////	{
				////		if ( inram.second.second->d_pOwnerDAG->d_dag_uniq_id == ref_Node->d_pOwnerDAG->d_dag_uniq_id )
				////		{
				////			ref_Node->dump( false , false );
				////			
				////			pQuality->dump( false , false );

				////			break;
				////		}
				////	}
				////}
			}
		}
	}

	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );
	//if ( SETTINGS()->d_echo__scheduleOnVM__details )
	//{
	//	XBT_INFO_EMPTY_LINE();
	//	//XBT_INFO( "----------------------------------------" , "" );
	//	XBT_INFO( "#(%d)  find_qualities provisioned(%d) available(s-%d m-%d l-%d el-%d)" , __LINE__ , (int)candidate_qualities.size() ,
	//			  _vm_type_count_in_qualities[0] , _vm_type_count_in_qualities[1] , _vm_type_count_in_qualities[2] , _vm_type_count_in_qualities[3] );
	//}
	//if ( SETTINGS()->d_dump_all_qualities_in_schedule_node && candidate_qualities.size() > 0 )
	//{
	//	XBT_INFO_EMPTY_LINE();
	//	//XBT_INFO( "----------------------------------------" , "" );
	//	for ( const shared_ptr< VMQuality > & qu : candidate_qualities )
	//	{
	//		XBT_INFO( "provision vm_%s cost(%f) totaltime(%f)" ,
	//				  qu->d_pVM->d_vmType->get_vmtypeName().c_str() , qu->d_estimate_TotalCost , qu->d_estimate_TotalTimeConsume );
	//		//qu->dump( false , true );
	//	}
	//	//XBT_INFO( "----------------------------------------" , "" );
	//}

	candidate_qualities.sort( []( const CPointer< VMQuality > & a , const CPointer< VMQuality > & b )
	{
		////if ( a->d_estimate_TotalCost == b->d_estimate_TotalCost && a->d_estimate_TotalCost == 0 )
		////{
		////	return a->d_vm_type->d_dollar_cost_per_cycle < b->d_vm_type->d_dollar_cost_per_cycle;
		////}
		////if ( a->d_estimate_TotalCost == b->d_estimate_TotalCost )
		////{
		////	return a->d_estimate_TotalTimeConsume < b->d_estimate_TotalTimeConsume;
		////}
		if ( a.allocated_ptr->d_estimate_TotalCost == b.allocated_ptr->d_estimate_TotalCost ) // این یعنی اینکه اگر قیمت ها یکسان شد و جفتشان قبل از ددلاین تمام می کنند اونی انتخاب شود که ماشین ارزانتری است
		{
			return a.allocated_ptr->d_vm_type->get_cost_duration( a.allocated_ptr->d_estimate_TotalTimeConsume ) < 
				b.allocated_ptr->d_vm_type->get_cost_duration( b.allocated_ptr->d_estimate_TotalTimeConsume );
		}
		return a.allocated_ptr->d_estimate_TotalCost < b.allocated_ptr->d_estimate_TotalCost;
	} );

	list< CPointer< VMQuality > > idle_has_container_has_input_before_deadline;
	list< CPointer< VMQuality > > idle_has_container_before_deadline;
	list< CPointer< VMQuality > > idle_before_deadline;
	for ( auto & qual : candidate_qualities )
	{
		if ( qual.allocated_ptr->d_stablish_container == false )
		{
			if ( qual.allocated_ptr->d_estimate_kasr_TotalResidentInputs_size_byte > 0 )
			{
				idle_has_container_has_input_before_deadline.push_back( qual );
			}
			else
			{
				idle_has_container_before_deadline.push_back( qual );
			}
		}
		else
		{
			idle_before_deadline.push_back( qual );
		}
	}

	CPointer< VMQuality > top_quality;
	{
		if ( idle_has_container_has_input_before_deadline.size() > 0 && top_quality.allocated_ptr == NULL )
		{
			top_quality = idle_has_container_has_input_before_deadline.front();
			idle_has_container_has_input_before_deadline__count++;
		}
		if ( idle_has_container_before_deadline.size() > 0 && top_quality.allocated_ptr == NULL )
		{
			top_quality = idle_has_container_before_deadline.front();
			idle_has_container_before_deadline__count++;
		}
		if ( idle_before_deadline.size() > 0 && top_quality.allocated_ptr == NULL )
		{
			top_quality = idle_before_deadline.front();
			idle_before_deadline__count++;
		}
		if ( SETTINGS()->d_echo__scheduleOnVM__details && top_quality.allocated_ptr != NULL )
		{
			XBT_INFO_EMPTY_LINE();
			XBT_INFO( "#(%d) on old vm vm_%s (%d,%d,%d) cost(%f) totaltime(%f) %s" ,
					  __LINE__ , top_quality.allocated_ptr->d_vm_type->get_vmtypeName() ,
					  idle_has_container_has_input_before_deadline__count , idle_has_container_before_deadline__count , idle_before_deadline__count ,
					  top_quality.allocated_ptr->d_estimate_TotalCost , top_quality.allocated_ptr->d_estimate_TotalTimeConsume ,
					  ( SIMULATION_NOW_TIME + top_quality.allocated_ptr->d_estimate_TotalTimeConsume <= *top_quality.allocated_ptr->d_pAlias_bundled_nodes_ordered->front()->d_task_deadline.get_absolute_soft_deadline( true ) ) ? "before" : "after" );
		}
	}

	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );

	// wait policy
	// اگر هیچ ماشین بیکاری قبل از ددلاین نتوانست تمامش کند
	if ( SETTINGS()->d_use_wait_policy && top_quality.allocated_ptr == NULL )
	{
		// according to paper . if scheduling the task on the next cycle on a VM of the slowest available type still leads to the task finishing by its deadline
		VMachine * slowest_available_machine = NULL;
		for ( auto & provisioned_vm : d_allocated_available_VMs )
		{
			if ( provisioned_vm->is_provisioned() )
			{
				if ( provisioned_vm->is_idle_vm() )
				{
					if ( slowest_available_machine == NULL || slowest_available_machine->d_type < provisioned_vm->d_vmType->d_type )
					{
						slowest_available_machine = provisioned_vm->d_vmType;
					}
				}
				else
				{
					if ( ( provisioned_vm->d_pLastContainer.allocated_ptr != NULL &&
						   provisioned_vm->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr != NULL ) )
					{
						assert( provisioned_vm->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr->d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() != NULL );

						if ( *provisioned_vm->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr->d_pAllocated_bundled_nodes_ordered->front()->get_actual_triggered_time() +
							 provisioned_vm->d_pLastContainer.allocated_ptr->d_pAssignedQuality.allocated_ptr->d_estimate_TotalTimeConsume < SIMULATION_NOW_TIME + SETTINGS_FAST()->d_schedule_periode_delay_time )
						{
							if ( slowest_available_machine == NULL || slowest_available_machine->d_type < provisioned_vm->d_vmType->d_type )
							{
								slowest_available_machine = provisioned_vm->d_vmType;
							}
						}
					}
				}
				//if ( slowest_available_machine != NULL && slowest_available_machine->d_type == eVM_Type::vm_small )
				//{
				//	break;
				//}
			}
		}

		if ( slowest_available_machine != NULL && slowest_available_machine->d_type == eVM_Type::vm_small )
		{
			for ( auto & provisioned_vm : d_allocated_available_VMs )
			{
				// اگر خالی بودند که در بالا فرصت داشتند تسک را جذب کنند
				if ( provisioned_vm->is_provisioned() && !provisioned_vm->is_idle_vm() && provisioned_vm->d_vmType->d_type == slowest_available_machine->d_type )
				{
					{
						VMQuality pQualityCalcOn_slowestvm;
						{
							// زمان اجرا روی یک ماشین ضعیف
							pQualityCalcOn_slowestvm.d_quality_dag_config = ref_Node->d_pOwnerDAG->d_dag_config;
							pQualityCalcOn_slowestvm.d_pAliasVM = provisioned_vm;
							////pQualityCalcOn_slowestvm->d_pVM->d_owner_region = d_vm_regions.front();
							pQualityCalcOn_slowestvm.d_pAliasVmOwnerRegion = d_allocated_regions.front().allocated_ptr;
							
							pQualityCalcOn_slowestvm.d_pAlias_bundled_nodes_ordered = &d_pTemp_bundled_nodes_ordered;
							pQualityCalcOn_slowestvm.d_pAlias_bundled_nodes_shuffled = &d_pTemp_bundled_nodes_shuffled;

							//pQualityCalcOn_slowestvm.d_quality_bundled_nodes_ordered = new vector< DAGNode * >();
							//pQualityCalcOn_slowestvm.d_quality_bundled_nodes_ordered->push_back( ref_Node );
							//pQualityCalcOn_slowestvm.d_quality_bundled_nodes_shuffled = new map< eNodeID , DAGNode * >();
							//( *pQualityCalcOn_slowestvm.d_quality_bundled_nodes_shuffled )[ ref_Node->d_node_uniq_id ] = ref_Node;

							pQualityCalcOn_slowestvm.d_stablish_vm = false;
							pQualityCalcOn_slowestvm.d_stablish_container = false; // if necessary fxn true it
							pQualityCalcOn_slowestvm.d_occupied_vm = false; // if necessary fxn true it
							pQualityCalcOn_slowestvm.d_EPSM_algorithm = true;
							pQualityCalcOn_slowestvm.d_pAliasVM->calculate_quality_for_EPSM_algorithm( &pQualityCalcOn_slowestvm );

							assert( pQualityCalcOn_slowestvm.d_estimate_exec_tasks_time_consume > 0 );
						}

						// اگر زمانی که از تسک قبلی مانده به علاوه زمان تا دور اسکجول بعدی به علاوه زمان لازم برای پردازش تسک روی این ماشین کمتر از زمان ددلاین تسک است می توان منتظر بود
						// اگر ماشینی پیدا شد که با وجود اسم فعلی و تا دور بعدی اسکجول بتواند به اندازه زمان اجرای تسک روی کند ترین ماشین زمان داشت هباشد و ددیلان تسک عبور نکند پس م یتوان منتظر بود
						if ( ( SIMULATION_NOW_TIME +
							   pQualityCalcOn_slowestvm.d_estimate_TotalTimeConsume <=
							   ////d_estimate_finished_prev_running_job_time_consume +
							   ////d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time +
							   ////pQualityCalcOn_provisioned_vm->d_estimate_StablishContainer_time_consume + /*اگر تغییر کانتینر لازم است*/
							   //SETTINGS_FAST()->d_schedule_periode_delay_time +
							   //pQualityCalcOn_slowestvm->d_estimate_read_tasks_from_global_storage_time_consume +
							   //pQualityCalcOn_slowestvm->d_estimate_transfer_tasks_from_global_storage_to_vm_time_consume +
							   //pQualityCalcOn_slowestvm->d_estimate_write_tasks_to_vm_time_consume +
							   //pQualityCalcOn_slowestvm->d_estimate_read_parent_output_from_local_vm_memory_time_consume +
							   //pQualityCalcOn_slowestvm->d_estimate_read_parent_output_from_global_storage_memory_time_consume +
							   //pQualityCalcOn_slowestvm->d_estimate_TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume +
							   //pQualityCalcOn_slowestvm->d_estimate_write_output_parent_from_global_storage_on_vm +
							   //pQualityCalcOn_slowestvm->d_estimate_exec_tasks_time_consume +
							   //pQualityCalcOn_slowestvm->d_estimate_write_output_to_vm_time_consume +
							   //pQualityCalcOn_slowestvm->d_estimate_transfer_output_to_global_storage_time_consume +
							   //pQualityCalcOn_slowestvm->d_estimate_write_output_to_global_storage_time_consume ) <=
							 *ref_Node->d_task_deadline.get_absolute_soft_deadline( true ) ) )
						{

							if ( SETTINGS()->d_echo__scheduleOnVM__top_quality_with_wait_policy_details )
							{
								XBT_INFO_EMPTY_LINE();
								XBT_INFO( "#(%d) top_quality wait policy " , __LINE__ );

								//XBT_INFO( "SIMULATION_NOW_TIME(%f)" , SIMULATION_NOW_TIME );
								//XBT_INFO( "d_estimate_finished_prev_running_job_time_consume(%f)" , d_estimate_finished_prev_running_job_time_consume );
								//XBT_INFO( "d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time(%f)" , d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time );
								//XBT_INFO( "pQualityCalcOn_provisioned_vm->d_estimate_StablishContainer_time_consume(%f)" , pQualityCalcOn_provisioned_vm->d_estimate_StablishContainer_time_consume );
								//XBT_INFO( "pQualityCalcOn_slowestvm->d_estimate_TotalTimeConsume(%f)" , pQualityCalcOn_slowestvm->d_estimate_TotalTimeConsume );
								//XBT_INFO( " + + + + (%f)" , SIMULATION_NOW_TIME +
								//		  d_estimate_finished_prev_running_job_time_consume +
								//		  d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time +
								//		  pQualityCalcOn_provisioned_vm->d_estimate_StablishContainer_time_consume +
								//		  pQualityCalcOn_slowestvm->d_estimate_TotalTimeConsume );
								//XBT_INFO( "absolute_soft_deadline(%f)" , *ref_Node->d_task_deadline.get_absolute_soft_deadline( true ) );

								//pQualityCalcOn_provisioned_vm->dump( true , CNFG()->d_echo__scheduleOnVM__top_quality_with_wait_policy_details_full_dag_dump );
							}
							this->d_statistics[ AeStatisticElement::stt_bundle_scheduling_result ][ STATISTICS_bundle_wait_count ].bundle_wait_count += 1;
							//pQualityCalcOn_slowestvm->d_quality_bundled_nodes_ordered->front()->d_pOwnerDAG->d_schedul_bundle_wait_count++;

							this->d_statistics[ AeStatisticElement::stt_dag_waiting_count ][ STATISTICS_INT( ref_Node->d_pOwnerDAG->d_dag_uniq_id ) ].dag_waiting_count += 1;

							//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );

							return; // wait
						}
					}
				}
			}
		}
	}

	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );

	// least price new machine
	if ( top_quality.allocated_ptr == NULL )
	{
		list< CPointer< VMQuality > > new_vm_of_each_type;
		//if ( ref_Node->d_pOwnerDAG->d_dag_deadline.isDagScheduled() )
		//{
			for ( auto & vm_type : VMachine::get_type_instances() )
			{
				switch ( SETTINGS_FAST()->d_EPSM_algorithm_ver )
				{
					case 3:
					{
						_ONE_TIME_VAR();
						_ONE_TIME( XBT_INFO( "#(%d) warning" , __LINE__ ) ); // donot comment this
						if ( ref_Node->d_parents.size() == 0 )
						{
							if ( vm_type.d_type != ref_Node->d_pOwnerDAG->d_least_price_vmp_type ) continue;
						}
						break;
					}
				}

				for ( auto & new_unprovisioned_vm : d_allocated_available_VMs )
				{
					if ( !new_unprovisioned_vm->is_provisioned() )
					{
						CPointer< VMQuality > pQuality;
						pQuality.allocate();
						pQuality.allocated_ptr->d_quality_dag_config = ref_Node->d_pOwnerDAG->d_dag_config;
						pQuality.allocated_ptr->d_pAliasVM = new_unprovisioned_vm;
						pQuality.allocated_ptr->d_pAliasVM->d_vmType = &vm_type;
						pQuality.allocated_ptr->d_vm_type = &vm_type;
						//pQuality->d_pVM->d_owner_region = d_vm_regions.front();
						pQuality.allocated_ptr->d_pAliasVmOwnerRegion = d_allocated_regions.front().allocated_ptr;
						
						pQuality.allocated_ptr->d_pAlias_bundled_nodes_ordered = &d_pTemp_bundled_nodes_ordered;
						pQuality.allocated_ptr->d_pAlias_bundled_nodes_shuffled = &d_pTemp_bundled_nodes_shuffled;

						//pQuality->d_quality_bundled_nodes_ordered = new vector< DAGNode * >();
						//pQuality->d_quality_bundled_nodes_ordered->push_back( ref_Node );
						//pQuality->d_quality_bundled_nodes_shuffled = new map< eNodeID , DAGNode * >();
						//( *pQuality->d_quality_bundled_nodes_shuffled )[ ref_Node->d_node_uniq_id ] = ref_Node;
						
						pQuality.allocated_ptr->d_stablish_vm = true;
						pQuality.allocated_ptr->d_stablish_container = true; // if necessary fxn true it
						pQuality.allocated_ptr->d_occupied_vm = false; // if necessary fxn true it
						pQuality.allocated_ptr->d_EPSM_algorithm = true;

						pQuality.allocated_ptr->d_pAliasVM->calculate_quality_for_EPSM_algorithm( pQuality.allocated_ptr );

						pQuality.allocated_ptr->d_pAliasVM->d_vmType = NULL;
						assert( pQuality.allocated_ptr->d_estimate_TotalTimeConsume > 0 );
						//assert( pQuality->d_bundle_duration_need_on_cheapest_vm_plus_spare > 0 );
						assert( pQuality.allocated_ptr->d_estimate_TotalCost > 0 );

						new_vm_of_each_type.push_back( pQuality );
						break;
					}
				}
			}
		//}
		//else
		//{

		//}

		//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );

		if ( SETTINGS()->d_warn_if_some_vm_type_are_not_avaiable_for_leased )
		{
			if ( ref_Node->d_parents.size() != 0 )
			{
				if ( new_vm_of_each_type.size() != VMachine::get_type_instances().size() )
				{
					XBT_INFO_EMPTY_LINE();
					for ( auto & each_type : new_vm_of_each_type )
					{
						XBT_INFO( "only available type %s" , each_type.allocated_ptr->d_vm_type->get_vmtypeName() );
					}
				}
			}
		}

		if ( SETTINGS()->d_dump_all_qualities_in_schedule_node )
		{
			XBT_INFO_EMPTY_LINE();
			//XBT_INFO( "----------------------------------------" , "" );
			for ( auto & qu : new_vm_of_each_type )
			{
				XBT_INFO( "unprovision vm(%s) cost(%f) totaltime(%f) now(%f) deadline(%f)" ,
						  qu.allocated_ptr->d_vm_type->get_vmtypeName() , qu.allocated_ptr->d_estimate_TotalCost , qu.allocated_ptr->d_estimate_TotalTimeConsume ,
						  SIMULATION_NOW_TIME , ref_Node->d_task_deadline.d_task_soft_deadline_time );

				//qu->dump( false , true );
			}
			//XBT_INFO( "----------------------------------------" , "" );
		}

		list< CPointer< VMQuality > > new_vm_can_finish_before_deadline;
		list< CPointer< VMQuality > > new_vm_cannot_finish_before_deadline;
		for ( auto & qual : new_vm_of_each_type )
		{
			if ( SIMULATION_NOW_TIME + qual.allocated_ptr->d_estimate_TotalTimeConsume <=
				 *qual.allocated_ptr->d_pAlias_bundled_nodes_ordered->front()->d_task_deadline.get_absolute_soft_deadline( true ) )
			{
				new_vm_can_finish_before_deadline.push_back( qual ); // ماشین جدید قبل از ددلاین تمومش میکنه
			}
			else
			{
				new_vm_cannot_finish_before_deadline.push_back( qual ); // کم هزینه ترین یعنی سریعترین ماشین که خلاصه تمامش کنه
			}
		}

		new_vm_can_finish_before_deadline.sort( []( const CPointer< VMQuality > & a , const CPointer< VMQuality > & b )
		{
			return a.allocated_ptr->d_estimate_TotalCost < b.allocated_ptr->d_estimate_TotalCost;
		} );
		new_vm_cannot_finish_before_deadline.sort( []( const CPointer< VMQuality > & a , const CPointer< VMQuality > & b )
		{
			return a.allocated_ptr->d_estimate_TotalTimeConsume < b.allocated_ptr->d_estimate_TotalTimeConsume;
		} );

		if ( top_quality.allocated_ptr == NULL && new_vm_can_finish_before_deadline.size() > 0 )
		{
			top_quality = new_vm_can_finish_before_deadline.front();
			//new_vm_can_finish_before_deadline__count++;
		}
		if ( top_quality.allocated_ptr == NULL && new_vm_cannot_finish_before_deadline.size() > 0 )
		{
			top_quality = new_vm_cannot_finish_before_deadline.front();
			//new_vm_cant_finish_before_deadline__count++;
		}

		if ( top_quality.allocated_ptr != NULL )
		{
			if ( SETTINGS()->d_echo_target_to_allocate_new_vm_in_EPSM )
			{
				XBT_INFO_EMPTY_LINE();
				XBT_INFO( "#(%d) allocate new vm_%s cost(%f) totaltime(%f) %s" ,
						  __LINE__ , top_quality.allocated_ptr->d_vm_type->get_vmtypeName() , top_quality.allocated_ptr->d_estimate_TotalCost , top_quality.allocated_ptr->d_estimate_TotalTimeConsume ,
						  ( SIMULATION_NOW_TIME + top_quality.allocated_ptr->d_estimate_TotalTimeConsume <= *top_quality.allocated_ptr->d_pAlias_bundled_nodes_ordered->front()->d_task_deadline.get_absolute_soft_deadline( true ) ) ? "before" : "after" );


				if ( SETTINGS()->d_echo_target_to_allocate_new_vm_in_EPSM_dump_quality )
				{
					top_quality.allocated_ptr->dump( true , SETTINGS()->d_echo_target_to_allocate_new_vm_in_EPSM_dump_quality_full_dag_dump );
				}
			}
		}
	}

	if ( top_quality.allocated_ptr == NULL )
	{
		if ( SETTINGS()->d_warn_if_some_vm_type_are_not_avaiable_for_leased )
		{
			//assert( false );
			XBT_INFO_EMPTY_LINE();
			XBT_INFO( "#(%d)  assert . top quality is null" , __LINE__ );
		}
		//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
		return; // not possible . DAGGenerator retru again later .. warning . may cause trap in obe bundle
	}

	if ( !top_quality.allocated_ptr->d_pAliasVM->is_provisioned() )
	{
		//XBT_INFO( "#(%d) " , __LINE__ );
		top_quality.allocated_ptr->d_pAliasVM->configureVM_ForProvisioning(top_quality.allocated_ptr);
		//XBT_INFO( "#(%d) " , __LINE__ );
	}

	if ( top_quality.allocated_ptr->d_stablish_container )
	{
		if ( top_quality.allocated_ptr->d_pAliasVM->d_pLastContainer.allocated_ptr != NULL )
		{
			top_quality.allocated_ptr->d_pAliasVM->d_pLastContainer.allocated_ptr->d_obsolete_container_time = SIMULATION_NOW_TIME;
			this->d_statistics[ AeStatisticElement::stt_container_stat ][ STATISTICS_change_container_count ].change_container_count += 1;
		}
		else
		{
			this->d_statistics[ AeStatisticElement::stt_container_stat ][ STATISTICS_first_time_container_count ].first_time_container_count += 1;
		}

		// old container going to hell
		top_quality.allocated_ptr->d_pAliasVM->d_pLastContainer.allocate();
		top_quality.allocated_ptr->d_pAliasVM->d_pLastContainer.allocated_ptr->d_container_dag_config = ref_Node->d_pOwnerDAG->d_dag_config;
		static int _containerID = 0;
		top_quality.allocated_ptr->d_pAliasVM->d_pLastContainer.allocated_ptr->d_container_uniq_id = ++_containerID;
		//d_all_container.push_back( top_quality->d_pVM->d_pLastContainer );
	}
	else
	{
		this->d_statistics[ AeStatisticElement::stt_container_stat ][ STATISTICS_keep_container_count ].keep_container_count += 1;
	}

	top_quality.allocated_ptr->d_pAlias_bundled_nodes_ordered = NULL;
	top_quality.allocated_ptr->d_pAlias_bundled_nodes_shuffled = NULL;

	top_quality.allocated_ptr->d_pAllocated_bundled_nodes_ordered = __NEW( _vector_nodes );
	top_quality.allocated_ptr->d_pAllocated_bundled_nodes_ordered->push_back( ref_Node );
	top_quality.allocated_ptr->d_pAllocated_bundled_nodes_shuffled = __NEW( _map_nodes );
	( *top_quality.allocated_ptr->d_pAllocated_bundled_nodes_shuffled )[ ref_Node->d_node_uniq_id ] = ref_Node;

	VMQuality * pQuality = top_quality.allocated_ptr; 
	top_quality.allocated_ptr->d_pAliasVM->d_pLastContainer.allocated_ptr->d_pAssignedQuality = top_quality;
	this->d_statistics[ AeStatisticElement::stt_bundle_scheduling_result ][ STATISTICS_bundle_scheduled_count ].bundle_scheduled_count += 1;
	//top_quality->d_quality_bundled_nodes_ordered->front()->d_pOwnerDAG->d_schedul_bundle_scheduled_count++;

	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );

	////top_quality->start_scheduling_quality( top_quality ); // set d_taskScheduled_in_vm to true

	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );

	if ( SETTINGS()->d_echo__scheduleOnVM__top_quality_non_wait_policy_details )
	{
		XBT_INFO_EMPTY_LINE();
		XBT_INFO( "#(%d) top_quality non wait" , __LINE__ );
		pQuality->dump( true , SETTINGS()->d_echo__scheduleOnVM__top_quality_non_wait_policy_details_full_dag_dump );
	}

	// TOCHEKC . زمان بین استارت تسک در اینجا با استارت در داخل اکتور نباید تفاوت زیادی بکند اصلا باید صفر باشد
	// یک نکته اگر شروع تسک را اینجا بگیرم خب زمان اجاره ماشین و کانتینر می افتد گردن تسک
	// آیا اشکال دارد و ایا باید زمان اجاره ماشین و کانتینر و انتقال دیتا گردن تسک باشد
	// راجع بهش باید فکر کنم

	//top_quality->d_pVM->d_idle_has_container_has_input_before_deadline__count += idle_has_container_has_input_before_deadline__count;
	//top_quality->d_pVM->d_idle_has_container_before_deadline__count += idle_has_container_before_deadline__count;
	//top_quality->d_pVM->d_idle_before_deadline__count += idle_before_deadline__count;
	//top_quality->d_pVM->d_new_vm_can_finish_before_deadline__count += new_vm_can_finish_before_deadline__count;
	//top_quality->d_pVM->d_new_vm_cant_finish_before_deadline__count += new_vm_cant_finish_before_deadline__count;

	//if ( top_quality->d_estimate_TotalCost == 0.0 )
	//{
	//	top_quality->d_pVM->d_place_task_on_idle_slot_machine__count++;
	//}
	//else
	//{
	//	top_quality->d_pVM->d_place_task_on_new_cycle_machine__count++;
	//}

	//XBT_INFO_OVERRIDEABLE__( AeDumpLevel::full_diagnose_lvl_low , "#(%d) " , __LINE__ );

	std::function<void()> f = std::bind( &ClassVM::callback_execute_bundle_on_container , pQuality->d_pAliasVM );
	
	if ( pQuality->d_pAliasVM->d_pVmActorKeeper != NULL )
	{
		pQuality->d_pAliasVM->d_pVmActor = pQuality->d_pAliasVM->d_pVmActorKeeper;
		pQuality->d_pAliasVM->d_pVmActor->restart();
	}
	else
	{
		pQuality->d_pAliasVM->d_pVmActor = sg4::Actor::create( string_format( "a%03d" , ( *pQuality->d_pAllocated_bundled_nodes_ordered )[ 0 ]->d_node_uniq_id ) , pQuality->d_pAliasVM->d_pVmHost , f );
	}
	
	////pQuality->d_pAliasVM->d_pVmActor = sg4::Actor::create( string_format( "a%03d" , ( *pQuality->d_pAllocated_bundled_nodes_ordered )[ 0 ]->d_node_uniq_id ) , pQuality->d_pAliasVM->d_pVmHost , f );
	
	pQuality->start_scheduling_quality( pQuality ); // set d_taskScheduled_in_vm to true

	//XBT_INFO_ALIVE_CHECK( "#(%d) " , __LINE__ );
	
	CATCH_TRY
}

void Vmm::dump()
{
	BEGIN_TRY

	XBT_INFO_EMPTY_LINE();
	XBT_INFO( "#(%d) dump Vmm" , __LINE__ );

	//LOG_TR( "INSERT INTO [dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval],[decval]) VALUES()" , "" );
	//LOG_TR( "INSERT INTO [dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval]) VALUES(@config_iteration_pack_id,'comment','%s')" , CNFG()->d_comment.c_str() );

	//----------------------------------------------------------------------------------------------------
	XBT_INFO( "--------------------" , "" );
	XBT_INFO( "#(%d) leased vm statistics" , __LINE__ );
	map< int/*reg id*/ , map< int/* vm type*/ , int/*count*/ > > _reg_vmtype_activvm_cnt;
	map< int/* vm type*/ , int/*count*/ > _vmtype_activvm;
	map< int/* region id*/ , int/*count*/ > _per_region_activvm;
	int vm_count = 0;
	for ( auto & reg : d_statistic_leased_vm )
	{
		for ( auto & vm_type : reg.second )
		{
			vm_count += vm_type.second;
			_reg_vmtype_activvm_cnt[ reg.first ][ vm_type.first ] += vm_type.second;
			_vmtype_activvm[ vm_type.first ] += vm_type.second;
			_per_region_activvm[ reg.first ] += vm_type.second;
		}
	}
	XBT_INFO( "vm count:%ds" , vm_count );
	LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
				VALUES(@config_iteration_pack_id,'leased vm statistics','vm count', %d)" , vm_count );
	for ( auto & regid : _reg_vmtype_activvm_cnt )
	{
		for ( auto & vmtype : regid.second )
		{
			XBT_INFO( "region id:%d  vmtype:%d  vm count:%ds" , regid.first , vmtype.first , vmtype.second );
		
			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
						VALUES(@config_iteration_pack_id,'leased vm statistics','region id:%d  vmtype:%d', %d )" ,
					regid.first/*reg id*/ , vmtype.first/*vmtype*/ , vmtype.second/*count*/ );
		}
	}
	for ( auto & vmtype : _vmtype_activvm )
	{
		XBT_INFO( "vmtype:%d  vm count:%ds" , vmtype.first , vmtype.second );

		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
					VALUES(@config_iteration_pack_id,'leased vm statistics','vmtype:%d', %d)" ,
				vmtype.first/*vmtype*/ , vmtype.second/*count*/ );
	}
	for ( auto & region : _per_region_activvm )
	{
		XBT_INFO( "region id:%d  vm count:%ds" , region.first , region.second );

		LOG_TR( "INSERT INTO [dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
					VALUES(@config_iteration_pack_id,'leased vm statistics','region id:%d', %d)" ,
				region.first/*regid*/ , region.second/*count*/ );
	}
	
	//----------------------------------------------------------------------------------------------------
	XBT_INFO( "--------------------" , "" );
	XBT_INFO( "#(%d) container statistics" , __LINE__ );
	int first_time_container_count = d_statistics[ AeStatisticElement::stt_container_stat ][ STATISTICS_first_time_container_count ].first_time_container_count;
	int change_container_count = d_statistics[ AeStatisticElement::stt_container_stat ][ STATISTICS_change_container_count ].change_container_count;
	int keep_container_count = d_statistics[ AeStatisticElement::stt_container_stat ][ STATISTICS_keep_container_count ].keep_container_count;

	XBT_INFO( "first time container count:%ds" , first_time_container_count );
	XBT_INFO( "container change count:%ds" , change_container_count );
	XBT_INFO( "container keep count:%ds" , keep_container_count );

	LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
				VALUES(@config_iteration_pack_id,'container statistics','first time container count', %d)" ,
			first_time_container_count );
	LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
				VALUES(@config_iteration_pack_id,'container statistics','container change count', %d)" ,
			change_container_count );
	LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
				VALUES(@config_iteration_pack_id,'container statistics','container keep count', %d)" ,
			keep_container_count );

	//----------------------------------------------------------------------------------------------------
	XBT_INFO( "--------------------" , "" );
	XBT_INFO( "#(%d) payment statistics" , __LINE__ );
	//map< int/*reg id*/ , map< int/* vm type*/ , double/*paid*/ > > _reg_vmtype_paid;
	map< int/* vm type*/ , double/*paid*/ > _vmtype_paid;
	//map< int/* region id*/ , double/*paid*/ > _per_region_paid;
	for ( const auto & vm : d_allocated_available_VMs )
	{
		for ( auto & cycle_cost : vm->d_leased_cycle_mgr.d_leased_cycles_cost )
		{
	//		_reg_vmtype_paid[ vm->d_owner_region->d_regionID ][ vm->d_vmType->d_type ] +=
	//			vm->d_leased_cycle_mgr.calc_num_cycle_paid() * vm->d_vmType->d_dollar_cost_per_cycle;

			_vmtype_paid[ std::get<1>(cycle_cost)/*vmtype*/ ] += std::get<0>(cycle_cost)/*cycle count*/ * std::get<2>(cycle_cost)/*cycle cost*/;

	//		_per_region_paid[ vm->d_owner_region->d_regionID ] += vm->d_leased_cycle_mgr.calc_num_cycle_paid() * vm->d_vmType->d_dollar_cost_per_cycle;
		}
	}
	double total_paid = 0;
	//for ( auto & regid : _reg_vmtype_paid )
	//{
	//	for ( auto & vmtype : regid.second )
	//	{
	//		total_paid += vmtype.second;
	//		XBT_INFO( "region id:%d  vmtype:%d  paid:%s $" , regid.first , vmtype.first , trim_floatnumber( vmtype.second ).c_str() );

	//		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
	//					VALUES(@config_iteration_pack_id,'payment statistics','region id:%d  vmtype:%d', %f)" ,
	//				regid.first , vmtype.first , vmtype.second );
	//	}
	//}
	for ( auto & vmtype : _vmtype_paid )
	{
		total_paid += vmtype.second;

		XBT_INFO( "vmtype:%d  paid:%s $" , vmtype.first , trim_floatnumber( vmtype.second ).c_str() );

		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
					VALUES(@config_iteration_pack_id,'payment statistics','vmtype:%d', %f)" ,
				vmtype.first/*vmtype*/ , vmtype.second/*payment*/ );
	}
	//for ( auto & region : _per_region_paid )
	//{
	//	XBT_INFO( "per region:%d  paid:%s $" , region.first , trim_floatnumber( region.second ).c_str() );

	//	LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
	//				VALUES(@config_iteration_pack_id,'payment statistics','per region:%d', %f)" ,
	//			region.first , region.second );
	//}
	XBT_INFO( "total paid:%s $" , trim_floatnumber( total_paid ).c_str() );
	LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
				VALUES(@config_iteration_pack_id,'payment statistics','total paid', %f)" ,
			total_paid );


	//----------------------------------------------------------------------------------------------------
	//XBT_INFO( "--------------------" , "" );
	//XBT_INFO( "#(%d) vm usage in schedule statistics" , __LINE__ );
	//int idle_has_container_has_input_before_deadline__count = 0;
	//int idle_has_container_before_deadline__count = 0;
	//int idle_before_deadline__count = 0;
	//int new_vm_can_finish_before_deadline__count = 0;
	//int new_vm_cant_finish_before_deadline__count = 0;
	//int place_task_on_idle_slot_machine__count = 0;
	//int place_task_on_new_cycle_machine__count = 0;
	//for ( const auto & vm : d_available_VMs )
	//{
	//	idle_has_container_has_input_before_deadline__count += vm->d_idle_has_container_has_input_before_deadline__count;
	//	idle_has_container_before_deadline__count += vm->d_idle_has_container_before_deadline__count;
	//	idle_before_deadline__count += vm->d_idle_before_deadline__count;
	//	new_vm_can_finish_before_deadline__count += vm->d_new_vm_can_finish_before_deadline__count;
	//	new_vm_cant_finish_before_deadline__count += vm->d_new_vm_cant_finish_before_deadline__count;
	//	place_task_on_idle_slot_machine__count += vm->d_place_task_on_idle_slot_machine__count;
	//	place_task_on_new_cycle_machine__count += vm->d_place_task_on_new_cycle_machine__count;
	//}
	//XBT_INFO( "idle_has_container_has_input_before_deadline__count(%s)" , trim_floatnumber( idle_has_container_has_input_before_deadline__count ).c_str() );
	//XBT_INFO( "idle_has_container_before_deadline__count(%s)" , trim_floatnumber( idle_has_container_before_deadline__count ).c_str() );
	//XBT_INFO( "idle_before_deadline__count(%s)" , trim_floatnumber( idle_before_deadline__count ).c_str() );
	//XBT_INFO( "new_vm_can_finish_before_deadline__count(%s)" , trim_floatnumber( new_vm_can_finish_before_deadline__count ).c_str() );
	//XBT_INFO( "new_vm_cant_finish_before_deadline__count(%s)" , trim_floatnumber( new_vm_cant_finish_before_deadline__count ).c_str() );
	//XBT_INFO( "place_task_on_idle_slot_machine__count(%s)" , trim_floatnumber( place_task_on_idle_slot_machine__count ).c_str() );
	//XBT_INFO( "place_task_on_new_cycle_machine__count(%s)" , trim_floatnumber( place_task_on_new_cycle_machine__count ).c_str() );
	

	// bandwidth usage statistics
	{
		XBT_INFO( "--------------------" , "" );
		XBT_INFO( "bandwidth usage statistics" , "" );
		
		int executing_static__read_from_global_storage_count = d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
			[ STATISTICS_executing_static__read_from_global_storage_count ].read_from_global_storage_count;
		int executing_static__read_from_local_region_count = d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
			[ STATISTICS_executing_static__read_from_local_region_count ].read_from_local_region_count;
		int executing_static__access_local_memory_count = d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
			[ STATISTICS_executing_static__access_local_memory_count ].access_local_memory_count;
		int executing_static__write_to_global_storage_count = d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
			[ STATISTICS_executing_static__write_to_global_storage_count ].write_to_global_storage_count;
		
		double executing_static__read_from_global_storage_size = d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
			[ STATISTICS_executing_static__read_from_global_storage_size ].read_from_global_storage_size;
		double executing_static__read_from_local_region_size = d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
			[ STATISTICS_executing_static__read_from_local_region_size ].read_from_local_region_size;
		double executing_static__access_local_memory_size = d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
			[ STATISTICS_executing_static__access_local_memory_size ].access_local_memory_size;
		double executing_static__write_to_global_storage_size = d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
			[ STATISTICS_executing_static__write_to_global_storage_size ].write_to_global_storage_size;

		XBT_INFO( "read_from_global_storage_count(%d)" , executing_static__read_from_global_storage_count );
		XBT_INFO( "read_from_local_region_count(%d)" , executing_static__read_from_local_region_count );
		XBT_INFO( "access_local_memory_count(%d)" , executing_static__access_local_memory_count );
		XBT_INFO( "write_to_global_storage_count(%d)" , executing_static__write_to_global_storage_count );

		XBT_INFO( "access_WAN_count(%d)" , executing_static__read_from_global_storage_count + executing_static__write_to_global_storage_count );
		XBT_INFO( "access_LAN_count(%d)" , executing_static__read_from_local_region_count );
		XBT_INFO( "access_Mem_count(%d)" , executing_static__access_local_memory_count );
		
		XBT_INFO( "read_from_global_storage_size(%s)" , trim_floatnumber( executing_static__read_from_global_storage_size ).c_str() );
		XBT_INFO( "read_from_local_region_size(%s)" , trim_floatnumber( executing_static__read_from_local_region_size ).c_str() );
		XBT_INFO( "access_local_memory_size(%s)" , trim_floatnumber( executing_static__access_local_memory_size ).c_str() );
		XBT_INFO( "write_to_global_storage_size(%s)" , trim_floatnumber( executing_static__write_to_global_storage_size ).c_str() );

		XBT_INFO( "access_WAN_size(%s)" , trim_floatnumber( executing_static__read_from_global_storage_size + executing_static__write_to_global_storage_size ).c_str() );
		XBT_INFO( "access_LAN_size(%s)" , trim_floatnumber( executing_static__read_from_local_region_size ).c_str() );
		XBT_INFO( "access_Mem_size(%s)" , trim_floatnumber( executing_static__access_local_memory_size ).c_str() );

		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','read_from_global_storage_count',%d)" ,
				executing_static__read_from_global_storage_count );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','read_from_local_region_count',%d)" ,
				executing_static__read_from_local_region_count );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','access_local_memory_count',%d)" ,
				executing_static__access_local_memory_count );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','write_to_global_storage_count',%d)" ,
				executing_static__write_to_global_storage_count );

		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','access_WAN_count',%d)" ,
				executing_static__read_from_global_storage_count + executing_static__write_to_global_storage_count );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','access_LAN_count',%d)" ,
				executing_static__read_from_local_region_count );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','access_Mem_count',%d)" ,
				executing_static__access_local_memory_count );

		
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','read_from_global_storage_size',%f)" ,
				executing_static__read_from_global_storage_size );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','read_from_local_region_size',%f)" ,
				executing_static__read_from_local_region_size );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','access_local_memory_size',%f)" ,
				executing_static__access_local_memory_size );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','write_to_global_storage_size',%f)" ,
				executing_static__write_to_global_storage_size );

		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','access_WAN_size',%f)" ,
				executing_static__read_from_global_storage_size + executing_static__write_to_global_storage_size );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','access_LAN_size',%f)" ,
				executing_static__read_from_local_region_size );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','access_Mem_size',%f)" ,
				executing_static__access_local_memory_size );

		double executing_static__access_WAN_cost = d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
			[ STATISTICS_executing_static__access_WAN_cost ].access_WAN_cost;
		double executing_static__access_LAN_cost = d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
			[ STATISTICS_executing_static__access_LAN_cost ].access_LAN_cost;
		double executing_static__access_Mem_cost = d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
			[ STATISTICS_executing_static__access_Mem_cost ].access_Mem_cost;

		XBT_INFO( "access_WAN_cost(%s)" , trim_floatnumber( executing_static__access_WAN_cost ).c_str() );
		XBT_INFO( "access_LAN_cost(%s)" , trim_floatnumber( executing_static__access_LAN_cost ).c_str() );
		XBT_INFO( "access_Mem_cost(%s)" , trim_floatnumber( executing_static__access_Mem_cost ).c_str() );

		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','access_WAN_cost',%f)" ,
				executing_static__access_WAN_cost );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','access_LAN_cost',%f)" ,
				executing_static__access_LAN_cost );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','access_Mem_cost',%f)" ,
				executing_static__access_Mem_cost );

		double executing_static__access_WAN_duration = d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
			[ STATISTICS_executing_static__access_WAN_duration ].access_WAN_duration;
		double executing_static__access_LAN_duration = d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
			[ STATISTICS_executing_static__access_LAN_duration ].access_LAN_duration;
		double executing_static__access_Mem_duration = d_statistics[ AeStatisticElement::stt_total_bandwidth_usage_statistics ]
			[ STATISTICS_executing_static__access_Mem_duration ].access_Mem_duration;

		XBT_INFO( "access_WAN_duration(%s)" , trim_floatnumber( executing_static__access_WAN_duration ).c_str() );
		XBT_INFO( "access_LAN_duration(%s)" , trim_floatnumber( executing_static__access_LAN_duration ).c_str() );
		XBT_INFO( "access_Mem_duration(%s)" , trim_floatnumber( executing_static__access_Mem_duration ).c_str() );

		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','access_WAN_duration',%f)" ,
				executing_static__access_WAN_duration );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','access_LAN_duration',%f)" ,
				executing_static__access_LAN_duration );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'bandwidth usage statistics','access_Mem_duration',%f)" ,
				executing_static__access_Mem_duration );

		for ( auto & row : d_statistics[ AeStatisticElement::stt_ccr_bandwidth_usage_statistics_per_WLM ] )
		{
			XBT_INFO( "ccr WLM usage ccr(%s) WLM(%s) value(%s)" , trim_floatnumber( row.first.nc.dd ).c_str() ,
					  row.first.nc.pp ,  trim_floatnumber( row.second.aliase_double ).c_str() );

			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'ccr bandwidth usage statistics','%s %f',%f)" ,
					row.first.nc.pp/*WLM*/ , row.first.nc.dd/*ccr*/ , row.second.aliase_double/*bandwidth_usage*/ );
		}


		double workload_execution_cost = d_statistics[ AeStatisticElement::stt_workload_process_transfer_cost_duration ]
			[ STATISTICS_workload_execution_cost ].totally_execution_cost;
		double workload_execution_duration = d_statistics[ AeStatisticElement::stt_workload_process_transfer_cost_duration ]
			[ STATISTICS_workload_execution_duration ].totally_execution_duration;
		double workload_transfer_cost = d_statistics[ AeStatisticElement::stt_workload_process_transfer_cost_duration ]
			[ STATISTICS_workload_transfer_cost ].totally_transfer_cost;
		double workload_transfer_duration = d_statistics[ AeStatisticElement::stt_workload_process_transfer_cost_duration ]
			[ STATISTICS_workload_transfer_duration ].totally_transfer_duration;

		XBT_INFO( "workload_execution_cost(%s)" , trim_floatnumber( workload_execution_cost ).c_str() );
		XBT_INFO( "workload_execution_duration(%s)" , trim_floatnumber( workload_execution_duration ).c_str() );
		XBT_INFO( "workload_transfer_cost(%s)" , trim_floatnumber( workload_transfer_cost ).c_str() );
		XBT_INFO( "workload_transfer_duration(%s)" , trim_floatnumber( workload_transfer_duration ).c_str() );

		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'workload_process_transfer_cost_duration','workload_execution_cost',%f)" ,
				workload_execution_cost );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'workload_process_transfer_cost_duration','workload_execution_duration',%f)" ,
				workload_execution_duration );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'workload_process_transfer_cost_duration','workload_transfer_cost',%f)" ,
				workload_transfer_cost );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'workload_process_transfer_cost_duration','workload_transfer_duration',%f)" ,
				workload_transfer_duration );


		for ( auto & row : d_statistics[ AeStatisticElement::stt_workload_ccr_process_transfer_cost_duration ] )
		{
			XBT_INFO( "ccr workload usage ccr(%s) WLM(%s) value(%s)" , trim_floatnumber( row.first.nc.dd ).c_str() ,
					  row.first.nc.pp ,  trim_floatnumber( row.second.aliase_double ).c_str() );

			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'ccr workload usage statistics','%s %f',%f)" ,
					row.first.nc.pp/*WLM*/ , row.first.nc.dd/*ccr*/ , row.second.aliase_double/*cost_duration*/ );
		}
	}

	// schedule length & schedule time statistics
	{
		XBT_INFO( "--------------------" , "" );
		XBT_INFO( "schedule time statistics" , "" );

		double workload_tasks_process_cost_sum = d_statistics[ AeStatisticElement::stt_workload_tasks_process_duration_cost ]
			[ STATISTICS_workload_tasks_process_cost ].workload_tasks_process_cost_sum;
		double workload_tasks_process_duration_sum = d_statistics[ AeStatisticElement::stt_workload_tasks_process_duration_cost ]
			[ STATISTICS_workload_tasks_process_duration ].workload_tasks_process_duration_sum;

		XBT_INFO( "workload_tasks_process_cost_sum(%s)" , trim_floatnumber( workload_tasks_process_cost_sum ).c_str() );
		XBT_INFO( "workload_tasks_process_duration_sum(%s)" , trim_floatnumber( workload_tasks_process_duration_sum ).c_str() );

		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'schedule time statistics','%s',%f)" ,
				STATISTICS_workload_tasks_process_cost , workload_tasks_process_cost_sum );
		LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'schedule time statistics','%s',%f)" ,
				STATISTICS_workload_tasks_process_duration , workload_tasks_process_duration_sum );


		for ( auto & row : d_statistics[ AeStatisticElement::stt_workload_vmtype_process_statistics ] )
		{
			XBT_INFO( "workload_vmtype_process_statistics vmtype(%d) ct(%s) val(%s)" , row.first.nt.ii ,
					  row.first.nt.pp ,  trim_floatnumber( row.second.aliase_double ).c_str() );

			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval],[relval])\
						VALUES(@config_iteration_pack_id,'workload_vmtype_process_statistics','%s',%d,%f)" ,
					row.first.nt.pp/*duration cost*/ , row.first.nt.ii/*vmtype*/ , row.second.aliase_double/*duration cost*/ );
		}

		for ( auto & row : d_statistics[ AeStatisticElement::stt_workload_vmtype_execution_trasfer_usage_statistics ] )
		{
			XBT_INFO( "workload_vmtype_execution_trasfer_usage_statistics vmtype(%d) ct(%s) val(%s)" , row.first.nt.ii ,
					  row.first.nt.pp ,  trim_floatnumber( row.second.aliase_double ).c_str() );

			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval],[relval])\
						VALUES(@config_iteration_pack_id,'workload_vmtype_execution_trasfer_usage_statistics','%s',%d,%f)" ,
					row.first.nt.pp/*execution_trasfer size cost duration*/ , row.first.nt.ii/*vmtype*/ , row.second.aliase_double/*size cost duration*/ );
		}

	}

	// stuff statistics
	{
		XBT_INFO( "--------------------" , "" );
		XBT_INFO( "stuff statistics" , "" );

		{ // ccr vmtype assignment
			for ( auto & row : d_statistics[ AeStatisticElement::stt_workload_ccr_vmtype_mapped ] )
			{
				XBT_INFO( "workload_ccr_vmtype_mapped ccr(%f) , vmtype(%d) , count(%d)" , row.first.cv.dd ,
						  row.first.cv.ii , row.second.ccr_vmtype_mapped_count );

				LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval],[relval])\
						VALUES(@config_iteration_pack_id,'workload_ccr_vmtype_mapped','%d',%d,%f)" ,
						row.first.cv.ii/*vmtype*/ , row.second.ccr_vmtype_mapped_count/*count*/ , row.first.cv.dd/*ccr*/ );
			}
		}

		{ // quality factor sign count
			for ( auto & row : d_statistics[ AeStatisticElement::stt_quality_factor_sign_count ] )
			{
				XBT_INFO( "quality_factor_sign_count sign(%d) , count(%d)" , row.first.ii , row.second.quality_factor_sign_count );

				LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval],[relval])\
						VALUES(@config_iteration_pack_id,'quality_factor_sign_count','',%d,%d)" ,
						row.first.ii/*sign*/ , row.second.quality_factor_sign_count/*count*/ );
			}
		}
		
		{ // bundle task count
			for ( auto & row : d_statistics[ AeStatisticElement::stt_bundle_task_count_frequency ] )
			{
				XBT_INFO( "bundle_task_count_frequency bundle_size(%d) , count(%d)" , row.first.ii , row.second.bundle_task_count );

				LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval],[relval])\
						VALUES(@config_iteration_pack_id,'bundle_task_count_frequency','',%d,%d)" ,
						row.first.ii/*bundle_size*/ , row.second.bundle_task_count/*count*/ );
			}
		}

		{ // quality statistic
			XBT_INFO( "total_try_quality(%ld) , reject_vm_by_branch_and_bound(%d)" , d_total_try_quality , d_reject_vm_by_branch_and_bound );
			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[decval])\
						VALUES(@config_iteration_pack_id,'total_try_quality',%ld)" ,
					d_total_try_quality );
			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[decval])\
						VALUES(@config_iteration_pack_id,'reject_vm_by_branch_and_bound',%d)" ,
					d_reject_vm_by_branch_and_bound );
		}

		{ // concurrency statistics
			
			double total_concurrency_ratio = 0;
			int count_concurrency_ratio = 0;
			for ( auto & row : d_pIteration->pDGG->d_entry_dags ) // uncomment this
			{
				assert( row->d_MaximumPossibleParallelism >= 0 );
				total_concurrency_ratio += d_parallismCounter[ row->d_dag_uniq_id ].second / row->d_MaximumPossibleParallelism; // uncomment this
				count_concurrency_ratio++;
			}

			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[relval])\
						VALUES(@config_iteration_pack_id,'avg dag concurrency',%f)" ,
					total_concurrency_ratio / count_concurrency_ratio );

			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[decval])\
						VALUES(@config_iteration_pack_id,'workload concurrency',%d)" ,
					d_workload_max_concurrency );
		}

		{ // idle statistics
			double total_idle_time = 0;
			double total_idle_time_cost = 0;
			double total_used_time = 0;
			int actual_vm_count = 0;
			int total_cycle_leaesd = 0;
			map< int /*vm_type*/ , int /*count*/ > vm_type_cycles_leased_count;
			map< int /*region id*/ , int /*count*/ > region_cycles_leased_count;
			for ( auto & vm : d_allocated_available_VMs )
			{
				for ( auto & vm_cycles_cost : vm->d_leased_cycle_mgr.d_leased_cycles_cost )
				{
					int cycle_count = std::get<0>( vm_cycles_cost );
					total_cycle_leaesd += cycle_count;
					int vm_type = std::get<1>( vm_cycles_cost );
					int region_id = std::get<5>( vm_cycles_cost );
					region_cycles_leased_count[ region_id ] += cycle_count;
					vm_type_cycles_leased_count[ vm_type ] += cycle_count;
					double used_duration = std::get<4>( vm_cycles_cost );
					total_used_time += used_duration;
					double idle_duration = cycle_count * std::get<3>( vm_cycles_cost ) - used_duration;
					double idle_duration_cost = VMachine::get_type_instance( ( eVM_Type )std::get<1>( vm_cycles_cost ) ).get_cost_duration( idle_duration );
					total_idle_time += idle_duration;
					total_idle_time_cost += idle_duration_cost;
					actual_vm_count++;
				}
			}

			XBT_INFO( "total_cycle_leaesd(%d) , total_used_time_vm(%f) , total_idle_time_vm(%f) , total_idle_time_vm_cost(%f) , avg_used_time_vm(%f) , avg_idle_time_vm(%f) , avg_idle_time_vm_cost(%f)" ,
					  total_cycle_leaesd , total_used_time , total_idle_time , total_idle_time_cost , total_used_time / actual_vm_count ,
					  total_idle_time / actual_vm_count , total_idle_time_cost / actual_vm_count );

			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval],[relval])\
						VALUES(@config_iteration_pack_id,'vm_efficiency','total_used_time',%d,%f)" ,
					actual_vm_count , total_used_time );
			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval],[relval])\
						VALUES(@config_iteration_pack_id,'vm_efficiency','total_idle_time',%d,%f)" ,
					actual_vm_count , total_idle_time );
			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval],[relval])\
						VALUES(@config_iteration_pack_id,'vm_efficiency','total_idle_time_cost',%d,%f)" ,
					actual_vm_count , total_idle_time_cost );
			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval],[relval])\
						VALUES(@config_iteration_pack_id,'vm_efficiency','avg_used_time_vm',%d,%f)" ,
					actual_vm_count , total_used_time / actual_vm_count );
			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval],[relval])\
						VALUES(@config_iteration_pack_id,'vm_efficiency','avg_idle_time_vm',%d,%f)" ,
					actual_vm_count , total_idle_time / actual_vm_count );
			LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval],[relval])\
						VALUES(@config_iteration_pack_id,'vm_efficiency','avg_idle_time_vm_cost',%d,%f)" ,
					actual_vm_count , total_idle_time_cost / actual_vm_count );

			for ( auto & row : vm_type_cycles_leased_count )
			{
				XBT_INFO( "vm_type(%d) , cycle_leased(%d)" , row.first , row.second );

				LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval],[relval])\
						VALUES(@config_iteration_pack_id,'vm_efficiency','vm_type_cycles_leased',%d,%d)" ,
						row.first/*vm_type*/ , row.second/*cycle_leased count*/ );
			}

			for ( auto & row : region_cycles_leased_count )
			{
				XBT_INFO( "region(%d) , cycle_leased(%d)" , row.first , row.second );

				LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[decval],[relval])\
						VALUES(@config_iteration_pack_id,'vm_efficiency','region_cycles_leased',%d,%d)" ,
						row.first/*regid*/ , row.second/*cycle_leased count*/ );
			}
		}

		{ // provisioning duration & cost
			for ( auto & row : d_statistics[ AeStatisticElement::stt_vm_provisioning_cost_duration ] )
			{
				XBT_INFO( "%s %s val(%f)" , row.first.nn.pp1 , row.first.nn.pp2 , row.second.aliase_double );

				LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'vm_provisioning cost duration','%s %s',%f)" ,
						row.first.nn.pp1/*duration cost*/ , row.first.nn.pp2/*region vmtype total*/ , row.second.aliase_double/*cost duration*/ );
			}
			for ( auto & row : d_statistics[ AeStatisticElement::stt_stablish_container_cost_duration ] )
			{
				XBT_INFO( "%s %s val(%f)" , row.first.nn.pp1 , row.first.nn.pp2 , row.second.aliase_double );

				LOG_TR( "INSERT INTO [SimGrid].[dbo].[Benchmarks]([cnfitrpc_id],[title],[txtval],[relval])\
						VALUES(@config_iteration_pack_id,'stablish_container cost duration','%s %s',%f)" ,
						row.first.nn.pp1/*duration cost*/ , row.first.nn.pp2/*region vmtype total*/ , row.second.aliase_double/*cost duration*/ );
			}
		}

	}

	CATCH_TRY
}

////void Vmm::release_dag_data( DAG * pCompletedDag )
////{
////	for ( auto & node : *pCompletedDag->d_ordered_nodes )
////	{
////		d_in_GS_output.erase( node->d_node_uniq_id );
////	}
////	for ( auto & reg : d_vm_regions )
////	{
////		for ( auto & vm : reg->d_local_vms )
////		{
////			if ( vm->is_provisioned() && vm->d_pLastContainer->d_container_dag_type.d_dag_type == pCompletedDag->d_dag_type.d_dag_type )
////			{
////				for ( auto & node : *pCompletedDag->d_ordered_nodes )
////				{
////					vm->d_pLastContainer->d_in_ram_task_output.erase( node->d_node_uniq_id );
////				}
////			}
////		}
////	}
////}

void Vmm::resumeIteration()
{
	//SECTION_ALIVE_CHECK("");
	////for ( auto & region : d_allocated_regions )
	////{
	////	region.allocated_ptr->d_pAlias_VMM = this;
	////}
	assert( d_allocated_regions.size() == 0 );
	for ( int i = 1 ; i <= SETTINGS_FAST()->d_region_count ; i++ )
	{
		CPointer< VM_Region > new_region;
		new_region.allocate();
		new_region.allocated_ptr->d_pAlias_VMM = this;
		d_allocated_regions.push_back( new_region );
	}

	d_resume_iteration = make_unique< bool >( true );
	//XBT_INFO( "#(%d) " , __LINE__ );
}

void Vmm::flush_cache()
{
	for ( auto & vm : d_allocated_available_VMs )
	{
		vm->deprovisionVM();

		//vm->d_idle_has_container_has_input_before_deadline__count = 0;
		//vm->d_idle_has_container_before_deadline__count = 0;
		//vm->d_idle_before_deadline__count = 0;
		//vm->d_new_vm_can_finish_before_deadline__count = 0;
		//vm->d_new_vm_cant_finish_before_deadline__count = 0;

		//vm->d_place_task_on_idle_slot_machine__count = 0;
		//vm->d_place_task_on_new_cycle_machine__count = 0;
	}
}

void Vmm::IterationFinished( bool b_last_iteration )
{
	//XBT_INFO( "#(%d) " , __LINE__ );
	*d_resume_iteration = false;
	d_pVMMDeprovisioningActor->kill();
	//if ( SETTINGS()->d_dump_each_simulation_iteration_result )
	//{
	//	dump();
	//}
	//d_all_container.clear();

	//d_top_quality_vm_stat.clear();
	d_statistic_leased_vm.clear();
	d_in_GS_output.clear();
	d_statistics.clear();
	d_parallismCounter.clear();

	for ( auto & vm : d_allocated_available_VMs )
	{
		vm->d_leased_cycle_mgr.release_manager_for_next_iteration();
	}

	d_allocated_regions.clear();
	if ( !b_last_iteration )
	{
		d_allocated_available_VMs.clear();
	}
	//XBT_INFO( "#(%d) " , __LINE__ );
	{
		std::lock_guard lock( *d_critical_section_entered_mutex );
	}
	//XBT_INFO( "#(%d) " , __LINE__ );
	d_critical_section_entered_mutex = NULL;
	//XBT_INFO( "#(%d) " , __LINE__ );
}

void Vmm::cleanup_on_last_benchmark_iteration()
{
	for ( auto vm : d_allocated_available_VMs )
	{
		__DELETE( vm );
	}
	d_allocated_available_VMs.clear();
	d_allocated_regions.clear();
}

#pragma endregion

