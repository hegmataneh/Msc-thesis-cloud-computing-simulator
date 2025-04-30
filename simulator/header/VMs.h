#pragma once

class containerC;
class ClassVM;
struct VM_Leased_Cycle_Mgr;
struct VM_Region;
class Vmm;
class BenchmarkIteration;

struct VMQuality
{
	int d_quality_id;
	ClassVM * d_pAliasVM; // in specific machine
	VMachine * d_vm_type; // for saving memory convert shared_pt into vm*
	VM_Region * d_pAliasVmOwnerRegion;
	DAG_config d_quality_dag_config;
	int d_bundle_id; // use in branch and bound vm selection

	// just local access in schedule on vm fxn
	vector< DAGNode * > * d_pAlias_bundled_nodes_ordered; // لیست کوالیتی ها طولانی میشد گفتم وقتی به لیست اضافه شدند حافظه اشغال کنند در تابع اسکجول روی وی ام
	map< eNodeID , DAGNode * > * d_pAlias_bundled_nodes_shuffled;

	// when pass quality to callback function
	vector< DAGNode * > * d_pAllocated_bundled_nodes_ordered; // وقتی قرار هست کوالیتی ر پاس بدیم به تابع کالبک لازم است لیت نود ها همراهش باشد و یک کپی از روش را داشته باشد
	map< eNodeID , DAGNode * > * d_pAllocated_bundled_nodes_shuffled;

	// actually happend
	bool d_stablish_vm; // true means vm is new
	bool d_stablish_container; // true means stablish container on vm
	bool d_occupied_vm; // if selected quality is occupied by another bundle
	bool d_EPSM_algorithm;

	//DurationTime d_actually_VirtualMachine_Warmup_time_consume;
	//DurationTime d_actually_container_initialization_time_consume;
	//DurationTime d_actually_read_task_from_global_storage_memory_time_consume;
	//DurationTime d_actually_transfer_task_from_global_storage_to_vm_time_consume;
	//DurationTime d_actually_store_task_to_vm_memory_time_consume;
	//DurationTime d_actually_read_parent_output_from_local_region_memory_time_consume;
	//DurationTime d_actually_transfer_parent_output_from_local_to_vm_memory_time_consume;
	//DurationTime d_actually_write_parent_output_from_local_to_vm_memory_time_consume;
	//DurationTime d_actually_read_parent_output_from_global_storage_memory_time_consume;
	//DurationTime d_actually_transfer_parent_output_from_global_storage_to_vm_memory_time_consume;
	//DurationTime d_actually_write_parent_output_global_storage_to_vm_memory_time_consume;
	//DurationTime d_actually_exec_task_on_vm_cpu_time_consume;
	//DurationTime d_actually_write_task_output_to_vm_memory_time_consume;
	//DurationTime d_actually_transfer_output_to_global_storage_time_consume;
	//DurationTime d_actually_write_output_task_on_global_storage_disk_time_consume;
	////string d_actually_local_bundle_parent_ids;
	//string d_actually_local_memory_parent_ids;
	//string d_actually_local_region_parent_ids;
	//string d_actually_globally_parent_ids;
	//string d_actually_in_local_region_memory_resident_ids;


	// estimation
	double d_estimate_quality_ratio;

	MEMORYBYTE d_estimate_kasr_TotalResidentInputs_size_byte; // IMPORTANT . resident bm input
	MEMORYBYTE d_estimate_kasr_TotalTransferedInput_size_byte; // IMPORTANT . necessary input
	MEMORYBYTE d_estimate_kasr_TotalInternalEdgeWeight_size_byte; // IMPORTANT . saved network load by bundling nodes
	MEMORYBYTE d_estimate_kasr_TotalExternalEdgeWeight_size_byte; // IMPORTANT . data transfer out for outer bundle child execution
	MEMORYBYTE d_estimate_TotalExportNodeOutputToGS_size_byte;
	MEMORYBYTE d_estimate_TotalTransferedInputFromLocalRegionSize_byte;
	MEMORYBYTE d_estimate_TotalTransferedInputFromGlobalStorageSize_byte;
	double d_estimate_TotalProcessFlops; // use for process cost and time consume
	MEMORYBYTE d_estimate_TotalTaskSize_byte; // task size
	MEMORYBYTE d_estimate_TotalOutputToVMMemory_size_byte; // list task write to host vm
	double d_estimate_kasr_ProcessCost;
	double d_estimate_kasr_DataTransferCost;
	double d_estimate_kasr_AllocationNewMachineCost;
	double d_estimate_kasr_ContainerStablishmentCost;
	double d_estimate_TotalCost;
	double d_estimate_use_leased_cycle_cost; // هزینه اصتفاده از سیکل های اجاره شده ماشین . منظور فقط قسمت هزینه باندل در سیکل ها
	double d_estimate_use_new_leased_cycle_cost; // فقط قسمت قیمت روی سیکل جدید

	DurationTime d_estimate_VirtualMachine_Warmup_time_consume;
	DurationTime d_estimate_StablishContainer_time_consume;
	DurationTime d_estimate_read_tasks_from_global_storage_time_consume;
	DurationTime d_estimate_transfer_tasks_from_global_storage_to_vm_time_consume;
	DurationTime d_estimate_write_tasks_to_vm_time_consume;
	DurationTime d_estimate_read_parent_output_from_local_vm_memory_time_consume;
	DurationTime d_estimate_read_parent_output_from_global_storage_memory_time_consume;
	DurationTime d_estimate_TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume;
	DurationTime d_estimate_write_output_parent_from_global_storage_on_vm;
	DurationTime d_estimate_TotalTransfered_ReadMemory_InputFromLocalRegionTimeConsume;
	DurationTime d_estimate_TotalTransfered_OnNetwork_InputFromLocalRegionTimeConsume;
	DurationTime d_estimate_write_output_parent_from_local_region_on_vm;
	DurationTime d_estimate_exec_tasks_time_consume;
	DurationTime d_estimate_write_output_to_vm_time_consume;
	DurationTime d_estimate_transfer_output_to_global_storage_time_consume;
	DurationTime d_estimate_write_output_to_global_storage_time_consume;
	DurationTime d_estimate_TotalTimeConsume;
	string d_estimate_local_bundle_parent_ids;
	string d_estimate_local_memory_parent_ids;
	string d_estimate_local_region_parent_ids;
	string d_estimate_globally_parent_ids;
	string d_estimate_in_local_region_memory_resident_ids;

	// wait policy
	DurationTime d_estimate_finished_prev_running_job_time_consume;
	DurationTime d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time;

	//string d_ret_message;

	VMQuality()
	{
		static int _id = -1;
		d_quality_id = ++_id;
		d_pAliasVM = NULL;
		d_vm_type = NULL;
		d_pAliasVmOwnerRegion = NULL;
		d_bundle_id = -1;
		
		d_pAlias_bundled_nodes_ordered = NULL;
		d_pAlias_bundled_nodes_shuffled = NULL;

		d_pAllocated_bundled_nodes_ordered = NULL;
		d_pAllocated_bundled_nodes_shuffled = NULL;

		d_estimate_quality_ratio = -1;
		d_stablish_vm = false;
		d_stablish_container = false;
		d_occupied_vm = false;
		d_EPSM_algorithm = false;

		d_estimate_kasr_TotalResidentInputs_size_byte = 0;
		d_estimate_kasr_TotalTransferedInput_size_byte = 0;
		d_estimate_kasr_TotalInternalEdgeWeight_size_byte = 0;
		d_estimate_kasr_TotalExternalEdgeWeight_size_byte = 0;
		d_estimate_TotalExportNodeOutputToGS_size_byte = 0;
		d_estimate_TotalTransferedInputFromLocalRegionSize_byte = 0;
		d_estimate_TotalTransferedInputFromGlobalStorageSize_byte = 0;
		d_estimate_TotalProcessFlops = 0;
		d_estimate_TotalTaskSize_byte = 0;
		d_estimate_TotalOutputToVMMemory_size_byte = 0;
		d_estimate_kasr_ProcessCost = 0;
		d_estimate_kasr_DataTransferCost = 0;
		d_estimate_kasr_AllocationNewMachineCost = 0;
		d_estimate_kasr_ContainerStablishmentCost = 0;
		d_estimate_TotalCost = 0;

		d_estimate_VirtualMachine_Warmup_time_consume = 0;
		d_estimate_StablishContainer_time_consume = 0;
		d_estimate_read_tasks_from_global_storage_time_consume = 0;
		d_estimate_transfer_tasks_from_global_storage_to_vm_time_consume = 0;
		d_estimate_write_tasks_to_vm_time_consume = 0;
		d_estimate_read_parent_output_from_local_vm_memory_time_consume = 0;
		d_estimate_read_parent_output_from_global_storage_memory_time_consume = 0;
		d_estimate_TotalTransfered_OnNetwork_InputFromGlobalStorageTimeConsume = 0;
		d_estimate_write_output_parent_from_global_storage_on_vm = 0;
		d_estimate_TotalTransfered_ReadMemory_InputFromLocalRegionTimeConsume = 0;
		d_estimate_TotalTransfered_OnNetwork_InputFromLocalRegionTimeConsume = 0;
		d_estimate_write_output_parent_from_local_region_on_vm = 0;
		d_estimate_exec_tasks_time_consume = 0;
		d_estimate_write_output_to_vm_time_consume = 0;
		d_estimate_transfer_output_to_global_storage_time_consume = 0;
		d_estimate_write_output_to_global_storage_time_consume = 0;
		d_estimate_TotalTimeConsume = 0;

		d_estimate_use_leased_cycle_cost = -1;
		d_estimate_use_new_leased_cycle_cost = -1;

		d_estimate_finished_prev_running_job_time_consume = 0;
		d_estimate_free_time_slot_between_prev_running_job_finished_and_new_schedule_time = 0;

		//d_actually_VirtualMachine_Warmup_time_consume = 0;
		//d_actually_container_initialization_time_consume = 0;
		//d_actually_read_task_from_global_storage_memory_time_consume = 0;
		//d_actually_transfer_task_from_global_storage_to_vm_time_consume = 0;
		//d_actually_store_task_to_vm_memory_time_consume = 0;
		//d_actually_read_parent_output_from_local_region_memory_time_consume = 0;
		//d_actually_transfer_parent_output_from_local_to_vm_memory_time_consume = 0;
		//d_actually_write_parent_output_from_local_to_vm_memory_time_consume = 0;
		//d_actually_read_parent_output_from_global_storage_memory_time_consume = 0;
		//d_actually_transfer_parent_output_from_global_storage_to_vm_memory_time_consume = 0;
		//d_actually_write_parent_output_global_storage_to_vm_memory_time_consume = 0;
		//d_actually_exec_task_on_vm_cpu_time_consume = 0;
		//d_actually_write_task_output_to_vm_memory_time_consume = 0;
		//d_actually_transfer_output_to_global_storage_time_consume = 0;
		//d_actually_write_output_task_on_global_storage_disk_time_consume = 0;
	}
	~VMQuality();
	void cleanup();

	bool pre_scheduling_quality() const
	{
		return d_pAlias_bundled_nodes_ordered != NULL;
	}
	
	void start_scheduling_quality( const VMQuality * this_quality );
	void finish_executing_quality( const VMQuality * this_quality );

	AbsoluteTime calculate_bundle_softdeadline();

	Vmm * get_vmm() const;
	DAG * get_DAG() const;

	string bundle_nodes_text() const;
	void dump( bool inside_another , bool bshort ) const;
};

struct Leased_Cycle_Used
{
	AbsoluteTime d_use_start_time;
	AbsoluteTime d_use_finish_time;
	//< VMQuality > d_used_to_exec_bundle;

	Leased_Cycle_Used()
	{
		d_use_finish_time = d_use_start_time = -1;
		//d_used_to_exec_bundle = NULL;
	}
	void dump( bool inside_another ) const;
	DurationTime used_time() const
	{
		assert( d_use_finish_time != -1 );
		assert( d_use_start_time != -1 );
		return d_use_finish_time - d_use_start_time;
	}
};

struct Leased_Cycle
{
	int d_index; // if cycle is prev cycle countinuity
	Leased_Cycle * d_prev_cycle; // first cycle of machine is null
	AbsoluteTime d_start_cycle_time;
	AbsoluteTime d_end_cycle_time;
	vector< Leased_Cycle_Used > * d_pAllocated_list_cycle_used;
	string d_request_by;

	VM_Leased_Cycle_Mgr * d_pAlias_CycleMgr; // use in dump

	Leased_Cycle()
	{
		d_index = -1;
		d_end_cycle_time = d_start_cycle_time = -1;
		d_prev_cycle = NULL;
		d_pAllocated_list_cycle_used = NULL;
	}
	~Leased_Cycle()
	{
		if ( d_pAllocated_list_cycle_used != NULL )
		{
			__DELETE( d_pAllocated_list_cycle_used );
			d_pAllocated_list_cycle_used = NULL;
		}
	}
	vector< Leased_Cycle_Used > * start_to_use_cycle()
	{
		if ( d_pAllocated_list_cycle_used == NULL )
		{
			d_pAllocated_list_cycle_used = __NEW( vector< Leased_Cycle_Used >() );
		}
		return d_pAllocated_list_cycle_used;
	}
	bool isUsed()
	{
		return d_pAllocated_list_cycle_used != NULL;
	}
	DurationTime cycle_duration() const
	{
		assert( d_start_cycle_time != -1 );
		assert( d_end_cycle_time != -1 );
		return d_end_cycle_time - d_start_cycle_time;
	}

	// هر سایکل توسط اولین تسکی که مصرف شود یعنی هزینه برای اون تسک در نظر گرفته می شود و بقیه اسلات خالی زمانی مجانی بحساب می آید
	void dump( bool inside_another ) const;
};

struct VM_Leased_Cycle_Mgr
{
	vector< Leased_Cycle * > d_allocated_continuous_leased_cycles;
	vector< tuple< int /*0cycle count*/ , int /*1vm type*/ , double /*2cycle cost*/ , double /*3one cycle duration*/ , double /*4total used duration*/ , int /*5region id*/ > > d_leased_cycles_cost; // هر کدام از موارد یک ماشین مجزا به حساب می آید . هر ماشین بعد از اینکه خالی شد با مشخصات جدید می تواند بکار گرفته شود
	ClassVM * d_pAliasVM; // use in dump

	VM_Leased_Cycle_Mgr()
	{
		d_pAliasVM = NULL;
	}
	~VM_Leased_Cycle_Mgr();
	void cleanup();

	Leased_Cycle * check_point( bool request_by_register_just_last_point_or_whole_cycle , const char * request_by , AbsoluteTime tnow = -1 , bool b_continue_used_flag = true ); // return last_cycle
	void start_to_use( const VMQuality * this_quality );
	void end_use( const VMQuality * this_quality );

	Leased_Cycle * nowTimeIsInsideCycleOccupiedByOther(); // return cycle

	void release_manager_for_deprovisioning();
	void release_manager_for_next_iteration();

	void dump( bool inside_another ) const;
};

class ClassVM
{
public:
	////////////////////////////////////////
	// data member
	VMID d_vm_uniq_id;
	VMachine * d_vmType; // cpu speed . hard speed

	sg4::Host * d_pVmHost;
	//sg4::Link * d_pLink2GlobalStorage;
	//sg4::Disk * d_pVmDisk;
	
	sg4::ActorPtr d_pVmActor; // if not null means vm is not idle
	sg4::ActorPtr d_pVmActorKeeper;

	VM_Region * d_pAlias_owner_region;

	VM_Leased_Cycle_Mgr d_leased_cycle_mgr;
	CPointer< containerC > d_pLastContainer; // use for dag type change . does no release or maybe release on vm retirement maybe

	//int d_idle_has_container_has_input_before_deadline__count;
	//int d_idle_has_container_before_deadline__count;
	//int d_idle_before_deadline__count;
	//int d_new_vm_can_finish_before_deadline__count;
	//int d_new_vm_cant_finish_before_deadline__count;
	//int d_place_task_on_idle_slot_machine__count;
	//int d_place_task_on_new_cycle_machine__count;

	////////////////////////////////////////
	// member fxn
	ClassVM();
	void init_vm( ClassVM * pVM , Vmm * pVmm , sg4::NetZone * srcpZone );

	inline bool is_idle_vm() const
	{
		return d_pVmActor == NULL;
	}
	inline bool is_provisioned() const
	{
		return d_pLastContainer.allocated_ptr != NULL;
	}

	void callback_execute_bundle_on_container();
	void configureVM_ForProvisioning(VMQuality * pQuality);
	void deprovisionVM();
	bool calculate_quality_ratio_can_fit_in_idle_slot( VMQuality * ref_in_out_quality );
	bool calculate_quality_for_EPSM_algorithm( VMQuality * ref_in_out_quality );
	void dump( bool inside_another , bool bshort ) const;
	Vmm * get_vmm() const;
};

struct VM_Region
{
	int d_regionID;
	string d_region_name;

	vector< ClassVM * > d_alias_local_vms;

	Vmm * d_pAlias_VMM; // global storage for this iteration

	////////////////////////////////////////
	// member fxn
	VM_Region();
	void dump( bool inside_another , bool bshort ) const;
	string serialize_local_memory_resident_node_output();
};

class containerC
{
public:
	ContainerID d_container_uniq_id;
	DAG_config d_container_dag_config;
	eDAG_ID d_dag_id_assigned; // just for config d_vm_share_policy_ver 2

	CPointer<VMQuality> d_pAssignedQuality; // currently exec bundle
	map< eNodeID /*resident node id*/ , pair< AbsoluteTime/*when added*/ , DAGNode * > > d_in_ram_task_output; // in ram task output

	AbsoluteTime d_obsolete_container_time;

	////////////////////////////////////////
	// member fxn
	containerC();
	containerC( DAG_config srcType );
	~containerC();
	void cleanup();

	void dump( bool inside_another , bool bshort ) const;
};

class Vmm
{
public:
	BenchmarkIteration * d_pIteration;
	sg4::Host * d_pVmmGlobalStorageHost;
	//sg4::Disk * d_pVmmGlobalStorageDisk;
	sg4::ActorPtr d_pVMMDeprovisioningActor;
	unique_ptr< bool > d_resume_iteration;
	sg4::MutexPtr d_critical_section_entered_mutex;
	map< eNodeID , DAGNode * > d_in_GS_output; // بدلیل مصرف زیاد حافظه فقط نود آی دی را نگه می دارم
	vector< CPointer<VM_Region> > d_allocated_regions;
	vector< ClassVM * > d_allocated_available_VMs; // include underload and idle and free vm
	//vector< < containerC > > d_all_container;


	int64_t d_total_try_quality; // تعداد دفعات محاسبه کوالیتی
	//int d_last_subbundle_try_quality;
	//int d_max_subbundle_try_quality;

	int d_last_candidate_quality_count;
	int d_max_candidate_quality_count;

	int d_reject_vm_by_branch_and_bound; // تعداد مواردی که از محاسبه کوالیتی معاف شده
	
	//map< string , int > d_top_quality_vm_stat;
	map< int /*region id*/ , map< int /*vm type*/ , int /*count*/ > > d_statistic_leased_vm; // هر ناحیه چند تا از چه نوعی ماشین درش اجاره شده
	map< AeStatisticElement , map< Statistics_Key_type /*title*/ , Statistics_Value_type /*count sum etc.*/> > d_statistics;
	map< int /*dag id*/ , pair< int /*scope concurrency*/ , int /*max scope concurrency*/> > d_parallismCounter; // محاسبه میزان موازی سازی برای یک دگ
	int d_workload_concurrency , d_workload_max_concurrency;

	int d_online_vm_count , d_max_online_vm_count;

	////////////////////////////////////////
	// member fxn
	Vmm();
	void cleanup_on_last_benchmark_iteration();
	void flush_cache();

	void init_VManagement( sg4::NetZone * pZone , int simulationIteration , Vmm * prev_vmm );
	void callback_resource_manager();
	void bundling_and_scheduleOnVM( DAGNode * ref_Node );
	void EPSM_algorithm_scheduleOnVM( DAGNode * ref_Node );
	void dump();
	//void release_dag_data( DAG * pCompletedDag );
	
	void resumeIteration();
	void IterationFinished( bool b_last_iteration );
};

