#pragma once

#pragma region define

/// <summary>
/// define
/// </summary>

/*
* در بحث شبکه ما یک ارتباط لن داخلی بین ماشین ها داریم که سرعت خیلی بالاست و یک شبکه به خارج داریم که می شود شامل دیگر ریجن ها و گلوبال استوریج
* سرعت گلوبال استوریج اینقدر زیاد است که سرعت محدود می شود به کار کننده بهش
*/

#define CPU_SPEED_POSTFIX( d )   d * 1e6
#define MEMORY_ACCESS_SPEED_POSTFIX( d )   d * 1e5
#define LAN_NETWORK_ACCESS_SPEED_POSTFIX( d )   d * 1e5
#define WAN_NETWORK_ACCESS_SPEED_POSTFIX( d )   d * 1e3

#define LAN_BANDWIDTH LAN_NETWORK_ACCESS_SPEED_POSTFIX( 1 )

//#define NETWORK_LATENCY_POSTFIX( d )   d##e##-##4

//#define MINIMUM_VM_CYCLE_PRICE 2.0 /*قیمت یک باشد در لوگاریتم به مشکل می خوریم*/

//#define CYCLE_PRICE( p ) pow( MINIMUM_VM_CYCLE_PRICE , p )

#define LOW_FAT_DAG_MIN_VALUE 1
#define LOW_FAT_DAG_MAX_VALUE 100

#define HI_FAT_DAG_MIN_VALUE 100
#define HI_FAT_DAG_MAX_VALUE 1000


/// <summary>
/// typedef
/// </summary>

typedef int VMID;
typedef int ContainerID;

typedef double SimulationAbsoluteTime;
typedef SimulationAbsoluteTime AbsoluteTime;

typedef double SimulationDurationTime;
typedef SimulationDurationTime DurationTime;

typedef double SystemDurationTime;
typedef double SystemAbsoluteTime;

typedef double MEMORY_SIZE_BYTE;
typedef MEMORY_SIZE_BYTE MEMORYBYTE;

typedef string _GUID;

/// <summary>
/// enum
/// </summary>

enum eVM_Type // cheap to expensive in order
{
	vm_small = 0 ,
	vm_medium = 1 ,
	vm_large = 2 ,
	vm_extra_large = 3 ,

	invalid_vm_type = 99
};

enum eNodeID
{
	INVALID_NODE_ID = -1 ,
	first_node_id = 0
};

enum eDAG_ID
{
	INVALID_DAG_ID = -1 ,
	first_dag_id = 0
};

//enum eDAGType
//{
//	INVALID_DAG_TYPE = -1 ,
//
//	linear_dag = 0,
//	diamond_dag = 1 ,
//	complex1_dag = 2 ,
//	complex2_dag = 3 ,
//	complex3_dag = 4 ,
//	complex4_dag = 5 ,
//
//	temp_dag = 6 , // use in filtered dag
//
//	last_dagtype = temp_dag
//
//};


enum AeDumpLevel
{
	no_dump = 0,

	to_always_dump = 1,
	to_dump_impossible_condition = 2,
	to_critical_diagnose = 3,

	dump_until_first_item = 4 ,

	dump_until_kernel_app_mode = dump_until_first_item ,
	dump_until_simulation_app_mode = 5 ,
	dump_until_pre_scheduling_app_mode = 6,
	dump_until_first_bundle_scheduled_app_mode = 7 ,
	dump_until_first_scheduler_iteration_app_mode = 8 ,
	dump_until_first_dag_finished_app_mode = 9 ,

	dump_until_last_item = dump_until_first_dag_finished_app_mode ,

	to_mandatory_diagnose = 20,

	full_diagnose_lvl_high = 92,
	full_diagnose_lvl_medium = 93 ,
	full_diagnose_lvl_low = 94 ,

	to_see_system_is_alive = 95 ,

	full_diagnose = 99
};

enum AeAppMode
{
	apmd_none = 0,
	apmd_kernel_mode = AeDumpLevel::dump_until_kernel_app_mode ,
	apmd_simulation_mode = AeDumpLevel::dump_until_simulation_app_mode ,
	apmd_pre_scheduling_mode = AeDumpLevel::dump_until_pre_scheduling_app_mode ,
	apmd_first_bundle_scheduled_mode = AeDumpLevel::dump_until_first_bundle_scheduled_app_mode,
	apmd_first_scheduler_iteration_mode = AeDumpLevel::dump_until_first_scheduler_iteration_app_mode,
	apmd_first_dag_finished_mode = AeDumpLevel::dump_until_first_dag_finished_app_mode
};

// execution time
// transfer time
// process time = init vm + init container + execution time + transfer time

enum AeStatisticElement
{
	stt_bundle_scheduling_result , // آمار زمانبندی شدن و ویت خوردن باندل
	stt_dag_waiting_count , // آمار اینکه هر دگ ویت خورده
	stt_bundle_task_count_frequency , // هر باندل سایز چند بار تکرار شده
	stt_quality_factor_sign_count , // تعداد تکرار مثبت منفی کوالیتی ها
	stt_container_stat , // آمار کانتینر جدید و تعویض آن
	stt_online_vm_count , // each vm type , i1=leased count , i2=busy or idle count , i3= max lease count
	stt_total_bandwidth_usage_statistics , // مقدار مجموع دیتا که در کل دگ ها منتقل می شود محاسبه می کند
	//stt_ccr_adaptive_statistics ,
	//stt_dag_bandwidth_usage , // مصرف پهنای باند در هر دگ
	stt_ccr_bandwidth_usage_statistics_per_WLM , // هر سی سی ار آمار مصرفش چجوری است
	stt_workload_process_transfer_cost_duration , // هر سی سی ار چقدر در کل مصرف کرده
	stt_workload_ccr_process_transfer_cost_duration , // هر سی سی ار چقدر در کل مصرف کرده
	stt_workload_tasks_process_duration_cost , // مجموع هزینه و زمان تک تک تسک های یک وورک لود
	stt_workload_vmtype_process_statistics , // در کل نوع ماشین ها چه آماری دارند
	stt_workload_vmtype_execution_trasfer_usage_statistics , // به تفکیک اجرا و انتقال نوع ماشین ها چه آماری دارند
	stt_workload_ccr_vmtype_mapped , // برای هر سی سی ار چه نوع ماشین هایی تخصیص یافته

	stt_vm_provisioning_cost_duration , // مجموع زمان و هزینه اجاره ماشین
	stt_stablish_container_cost_duration , // مجموع زمان و هزینه استقرار کانتینر
};

union Statistics_Key_type
{
	//long double ld; // 16 byte

	char * pp; // 8 byte . gcc
	int ii;
	//double ff; // 8 byte . gcc

	struct name_ccr
	{
		char * pp;
		double dd;
	} nc;

	struct name_vmtype
	{
		char * pp;
		int64_t ii;
	} nt;

	struct ccr_vmtype
	{
		double dd;
		int64_t ii;
	} cv;

	struct name_name
	{
		char * pp1;
		char * pp2;
	} nn;

	//struct i64i64
	//{
	//	int64_t ii1;
	//	int64_t ii2;
	//} ii64;

	Statistics_Key_type( char * src_pp ) : Statistics_Key_type()
	{
		pp = src_pp;
	}
	Statistics_Key_type( int src_ii ) : Statistics_Key_type()
	{
		ii = src_ii;
	}
	Statistics_Key_type( char * src_pp , double src_dd ) : Statistics_Key_type()
	{
		nc.pp = src_pp;
		nc.dd = src_dd;
	}
	Statistics_Key_type( char * src_pp , int64_t src_ii ) : Statistics_Key_type()
	{
		nt.pp = src_pp;
		nt.ii = src_ii;
	}
	Statistics_Key_type( double src_dd , int64_t src_ii ) : Statistics_Key_type()
	{
		cv.dd = src_dd;
		cv.ii = src_ii;
	}
	Statistics_Key_type( const char * src_pp1 , const char * src_pp2 ) : Statistics_Key_type()
	{
		nn.pp1 = (char *)src_pp1;
		nn.pp2 = (char *)src_pp2;
	}
	//Statistics_Key_type( double src_ff ) : Statistics_Key_type()
	//{
	//	ff = src_ff;
	//}
	//Statistics_Key_type( float src_f , int src_i ) : Statistics_Key_type()
	//{
	//	fi.f1 = src_f;
	//	fi.i2 = src_i;
	//}
	
	bool operator<( const Statistics_Key_type& r) const
	{
		return memcmp( this , &r , sizeof( r ) ) < 0;
	}

	Statistics_Key_type() { memset( this, 0, sizeof( Statistics_Key_type ) ); }
};

union Statistics_Value_type
{
	double aliase_double;

	int bundle_wait_count;
	int bundle_scheduled_count;
	int dag_waiting_count;
	int bundle_task_count;
	int quality_factor_sign_count;

	int change_container_count;
	int first_time_container_count;
	int keep_container_count;

	tuple< int /*leased count*/ , int /*busy or idle count*/ , int /*max lease count*/ > online_vm_count;

	int read_from_global_storage_count;
	double read_from_global_storage_size;

	int read_from_local_region_count;
	double read_from_local_region_size;

	int access_local_memory_count;
	double access_local_memory_size;

	int write_to_global_storage_count;
	double write_to_global_storage_size;

	double access_WAN_size;
	double access_LAN_size;
	double access_Mem_size;

	double access_WAN_cost;
	double access_LAN_cost;
	double access_Mem_cost;

	double access_WAN_duration;
	double access_LAN_duration;
	double access_Mem_duration;

	double totally_execution_cost;
	double totally_execution_duration;
	double totally_transfer_cost;
	double totally_transfer_duration;

	double workload_tasks_process_cost_sum;
	double workload_tasks_process_duration_sum;

	double workload_vmtype_process_size;
	double workload_vmtype_process_cost;
	double workload_vmtype_process_duration;

	int ccr_vmtype_mapped_count;

	double vm_provision_cost;
	double vm_provision_duration;

	double stablish_container_cost;
	double stablish_container_duration;

	Statistics_Value_type() { memset( this, 0, sizeof( Statistics_Value_type ) ); }
};

#define STATISTICS_POINTER( pointer ) ((char *)pointer) /*Statistics_Key_type((char *)pointer).pp*/
#define STATISTICS_INT( intt ) ((int)intt) /*Statistics_Key_type((int)intt).ii*/
//#define STATISTICS_DOUBLE( doublee ) (doublee) /*Statistics_Key_type((float)floatt).ff*/
//#define STATISTICS_SIZE( sizee )  Statistics_Key_type((size_t)sizee).sz
#define STATISTICS_NAME_CCR( name , ccr ) Statistics_Key_type( name , ccr )
#define STATISTICS_NAME_NUMBER( name , vmtype ) Statistics_Key_type( name , vmtype )
#define STATISTICS_NAME_NAME( name1 , name2 ) Statistics_Key_type( name1 , name2 )
#define STATISTICS_NAME_VMTYPE( name , vmtype ) STATISTICS_NAME_NUMBER( name , vmtype )
#define STATISTICS_CCR_VMTYPE( ccr , vmtype ) Statistics_Key_type( ccr , vmtype )
#define STATISTICS_NAME_REGIONID( name , regionid ) STATISTICS_NAME_NUMBER( name , regionid )


#define STATISTICS_bundle_wait_count  STATISTICS_POINTER("bundle_wait_count") /*چند تا ویت کلا در این پکیج دگ خورده*/
#define STATISTICS_bundle_scheduled_count  STATISTICS_POINTER("bundle_scheduled_count") /*چند تا باندل زمانبندی شدن */

#define STATISTICS_first_time_container_count  STATISTICS_POINTER("first_time_container_count")
#define STATISTICS_change_container_count  STATISTICS_POINTER("change_container_count")
#define STATISTICS_keep_container_count  STATISTICS_POINTER("keep_container_count")

#define STATISTICS_executing_static__read_from_global_storage_count  STATISTICS_POINTER("executing_static__read_from_global_storage_count")
#define STATISTICS_executing_static__read_from_global_storage_size  STATISTICS_POINTER("executing_static__read_from_global_storage_size")

#define STATISTICS_executing_static__access_local_memory_count  STATISTICS_POINTER("STATISTICS_executing_static__access_local_memory_count")
#define STATISTICS_executing_static__access_local_memory_size  STATISTICS_POINTER("STATISTICS_executing_static__access_local_memory_size")

#define STATISTICS_executing_static__read_from_local_region_count  STATISTICS_POINTER("executing_static__read_from_local_region_count")
#define STATISTICS_executing_static__read_from_local_region_size  STATISTICS_POINTER("executing_static__read_from_local_region_size")

#define STATISTICS_executing_static__write_to_global_storage_count  STATISTICS_POINTER("executing_static__write_to_global_storage_count")
#define STATISTICS_executing_static__write_to_global_storage_size  STATISTICS_POINTER("executing_static__write_to_global_storage_size")

#define STATISTICS_executing_static__access_WAN_size  STATISTICS_POINTER("STATISTICS_executing_static__access_WAN_size")
#define STATISTICS_executing_static__access_LAN_size  STATISTICS_POINTER("STATISTICS_executing_static__access_LAN_size")
#define STATISTICS_executing_static__access_Mem_size  STATISTICS_POINTER("STATISTICS_executing_static__access_Mem_size")

#define STATISTICS_executing_static__access_WAN_cost  STATISTICS_POINTER("STATISTICS_executing_static__access_WAN_cost")
#define STATISTICS_executing_static__access_LAN_cost  STATISTICS_POINTER("STATISTICS_executing_static__access_LAN_cost")
#define STATISTICS_executing_static__access_Mem_cost  STATISTICS_POINTER("STATISTICS_executing_static__access_Mem_cost")

#define STATISTICS_executing_static__access_WAN_duration  STATISTICS_POINTER("STATISTICS_executing_static__access_WAN_duration")
#define STATISTICS_executing_static__access_LAN_duration  STATISTICS_POINTER("STATISTICS_executing_static__access_LAN_duration")
#define STATISTICS_executing_static__access_Mem_duration  STATISTICS_POINTER("STATISTICS_executing_static__access_Mem_duration")

#define STATISTICS_workload_execution_cost  STATISTICS_POINTER("STATISTICS_workload_execution_cost")
#define STATISTICS_workload_execution_duration  STATISTICS_POINTER("STATISTICS_workload_execution_duration")
#define STATISTICS_workload_transfer_cost  STATISTICS_POINTER("STATISTICS_workload_transfer_cost")
#define STATISTICS_workload_transfer_duration  STATISTICS_POINTER("STATISTICS_workload_transfer_duration")

#define STATISTICS_workload_tasks_process_cost  STATISTICS_POINTER("STATISTICS_workload_tasks_process_cost")
#define STATISTICS_workload_tasks_process_duration  STATISTICS_POINTER("STATISTICS_workload_tasks_process_duration")

#define STATISTICS_workload_vmtype_process_cost  STATISTICS_POINTER("STATISTICS_workload_vmtype_process_cost")
#define STATISTICS_workload_vmtype_process_duration  STATISTICS_POINTER("STATISTICS_workload_vmtype_process_duration")

#define STATISTICS_workload_vmtype_datatransfer_size  STATISTICS_POINTER("STATISTICS_workload_vmtype_datatransfer_size")
#define STATISTICS_workload_vmtype_datatransfer_cost  STATISTICS_POINTER("STATISTICS_workload_vmtype_datatransfer_cost")
#define STATISTICS_workload_vmtype_datatransfer_duration  STATISTICS_POINTER("STATISTICS_workload_vmtype_datatransfer_duration")

#define STATISTICS_workload_vmtype_execution_size  STATISTICS_POINTER("STATISTICS_workload_vmtype_execution_size")
#define STATISTICS_workload_vmtype_execution_cost  STATISTICS_POINTER("STATISTICS_workload_vmtype_execution_cost")
#define STATISTICS_workload_vmtype_execution_duration  STATISTICS_POINTER("STATISTICS_workload_vmtype_execution_duration")

#define STATISTICS_workload_vm_provision_cost  STATISTICS_POINTER("STATISTICS_workload_vm_provision_cost")
#define STATISTICS_workload_vm_provision_duration  STATISTICS_POINTER("STATISTICS_workload_vm_provision_duration")

#define STATISTICS_workload_vm_provision_cost_per_vm_type  STATISTICS_POINTER("STATISTICS_workload_vm_provision_cost_per_vm_type")
#define STATISTICS_workload_vm_provision_duration_per_vm_type  STATISTICS_POINTER("STATISTICS_workload_vm_provision_duration_per_vm_type")

#define STATISTICS_workload_vm_provision_cost_per_region  STATISTICS_POINTER("STATISTICS_workload_vm_provision_cost_per_region")
#define STATISTICS_workload_vm_provision_duration_per_region  STATISTICS_POINTER("STATISTICS_workload_vm_provision_duration_per_region")

#define STATISTICS_workload_stablish_container_cost  STATISTICS_POINTER("STATISTICS_workload_stablish_container_cost")
#define STATISTICS_workload_stablish_container_duration  STATISTICS_POINTER("STATISTICS_workload_stablish_container_duration")

#define STATISTICS_workload_stablish_container_cost_per_vm_type  STATISTICS_POINTER("STATISTICS_workload_stablish_container_cost_per_vm_type")
#define STATISTICS_workload_stablish_container_duration_per_vm_type  STATISTICS_POINTER("STATISTICS_workload_stablish_container_duration_per_vm_type")

#define STATISTICS_workload_stablish_container_cost_per_region  STATISTICS_POINTER("STATISTICS_workload_stablish_container_cost_per_region")
#define STATISTICS_workload_stablish_container_duration_per_region  STATISTICS_POINTER("STATISTICS_workload_stablish_container_duration_per_region")

#pragma endregion

#pragma region VMachine

struct VMachine
{
	eVM_Type d_type;

	double d_cpu_speed;
	double d_memory_access_speed;
	//double d_memory_read_speed; // avaz hard va ram yek memory migiram vali ba sorat bala
	//double d_memory_write_speed; // avaz hard va ram yek memory migiram vali ba sorat bala
	double d_lan_network_speed;
	double d_wan_network_speed; // bytes/second
	//double d_network_latency;

	double d_dollar_cost_per_cycle;


	VMachine()
	{
		d_type = eVM_Type::invalid_vm_type;
	}
	VMachine( const VMachine & srcvm )
	{
		d_type = srcvm.d_type;
		d_cpu_speed = srcvm.d_cpu_speed;
		d_memory_access_speed = srcvm.d_memory_access_speed;
		d_lan_network_speed = srcvm.d_lan_network_speed;
		d_wan_network_speed = srcvm.d_wan_network_speed;
		//d_network_latency = srcvm.d_network_latency;
		d_dollar_cost_per_cycle = srcvm.d_dollar_cost_per_cycle;
	}
	

	void add_type();
	static vector< VMachine > & get_type_instances();
	static VMachine & get_type_instance( eVM_Type type );
	const char * get_vmtypeName() const;

	double get_cost_duration( double duration ) const;
	double get_duration_datatransfer_on_wan( double datasize ) const;
	double get_duration_datatransfer_on_lan( double datasize ) const;
	double get_duration_datatransfer_on_mem( double datasize ) const;
	double get_execution_duration( double flops ) const;
	double get_execution_cost( double flops ) const;
	double get_cost_datatransfer_on_wan( double datasize ) const;

	static VMachine & weakest_machine();
	static VMachine & strongest_machine();

	void dump( bool inside_another );
};

struct VMachine_Small : VMachine
{
	VMachine_Small( bool badd = false );
};

struct VMachine_Medium : VMachine
{
	VMachine_Medium( bool badd = false );
};

struct VMachine_Large : VMachine
{
	VMachine_Large( bool badd = false );
};

struct VMachine_ExtraLarge : VMachine
{
	VMachine_ExtraLarge( bool badd = false );
};

#pragma endregion

#pragma region DAG_config

struct DAG_config
{
	int d_dag_type;

	double d_container_initialization_time;

	DAG_config()
	{
		d_dag_type = -1;
		d_container_initialization_time = -1;
	}

	void init_dag_config( int dag_type );
	//string get_dagtypename() const;
};

#pragma endregion

#ifdef uses_Deadline_Quota

struct DAGNode;
class DAG;

class DAG_Deadline_Quota
{
public:
	DAG * d_paired_dag;

	DurationTime d_dag_requestor_deadline_duration; // مدت زمانیکه درخواست کننده وورک فلو برای اجرا در نظر گرفته
	// هر وورک فلو به ترتیب از کمترین قیمت و ضعیف ترین تا قوی ترین ماشین بررسی می شود
	// و در یکی از این ماشین ها می تواند تا قبل از ددلاین مشخص شده به پایان برسد
	// این زمان در این فیلد ذخیره می شود
	DurationTime d_dag_process_duration_in_cheapest_and_feasible_machine; // in dag and in task means زمان لازم برای پروسس دگ

	AbsoluteTime d_dag_actual_recieved_time; // وقتی دگ در لیست دگ های موجود درج می شود در واقع وارد سیستم می شود
	AbsoluteTime d_dag_deadline_time; // تمام دگ یک ددلاین به لحاظ دقیق زمانی خواهد داشت
	AbsoluteTime d_dag_actual_first_schedule_time; // وقتی دگ شروع به زمانبندی می شود
	AbsoluteTime d_dag_actual_finish_time;


	////////////////////
	// member fxn

	DAG_Deadline_Quota();
	void clear();

	void declare_dag_arrival_time();
	void declare_dag_start_schedule();
	void declare_dag_finish_time();

	bool isDagArrivedToBeSchedule() const
	{
		return d_dag_actual_recieved_time >= 0;
	}
	bool isDagScheduled() const
	{
		return d_dag_actual_first_schedule_time >= 0;
	}
	bool isDagFinishedBeforeDeadline() const;
	bool isDagFinishedExecution() const
	{
		return d_dag_actual_finish_time >= 0;
	}

	void dump( bool inside_another );
};

class Task_Deadline_Quota
{
public:
	DAGNode * d_paired_node; // good for traverse to childs

	DurationTime d_task_quota_duration_based_on_process_need; // هر تسک به نسبت کل دگ قسمتی از زمان اجرا را در بر می گیرد و این می شود سهمش از زمان و زمان اضافه
	DurationTime d_task_spare_duration_quota; // از اختلاف زمان ددلاین و زمان پردازش کل وورک فلو در ضعین ترین ماشین ممکن یک زمانیکه اضافه می آید این زمان تقسیم بر هر تسک می شود و می شود سهمیه آنت
	// اگر تسکی زودتر به ÷ایان رسید سهم زمانش تقسیم می شود بین همه زیر مجموعه به نسبت زمان اونها از کل تا همه تسک ها این فرصت را ÷یدا کنند که در ماشین ارزانتر زمانبندی گردند
	// فعلا نمی آیم و ددلاین دقیق زمانی برای تسک ها تعیین کنم و در واقع هر تسک یک مقدار بازه زمانی وقت دارد اجرا شود

	// یک نکته ای هست وقتی یک تسک تمام می شود هرچه زمان اضافه آورده بین زیر دستاش پخش میکنه
	// soft deadline

	// ددلاین اصلی اینست
	AbsoluteTime d_task_soft_deadline_time; // هر تسک زمانی که زمانبندی می شود همانجا زمان ددلاینش مشخص می شود
	//AbsoluteTime d_last_time_deadline_reset; // temp

	//AbsoluteTime d_first_time_deadline; // temp

	AbsoluteTime d_task_actual_triggered_time; // trigger by sys admin to exec on container
	AbsoluteTime d_task_actual_complete_execution_time;
	
	////////////////////
	// member fxn

	Task_Deadline_Quota();
	void clear();

	void declare_task_actual_start_time();
	void declare_task_actual_finish_time(); // get time from system . and do childs calculation automatic

	AbsoluteTime * get_absolute_soft_deadline( bool bIfNUllFromNow );

	void dump( bool inside_another );
};

#endif

#define IS_BETWEEN( a , ffirst , llast ) ( a >= ffirst && a <= llast )
#define NOT_BETWEEN( a , ffirst , llast ) (!IS_BETWEEN(a,ffirst,llast))

#define __DEFAULT_CONFIG_PACKAGE_ROOT "/mnt/c/simgrid/simgrid-3.32/out/build/WSL-GCC-Debug/z__ConfigPackage/template low vm -1-EBSM random nowait"

#ifdef uses_LOG

std::string ___log( const char * fxn , int line_number , bool concattime , bool simulation_time_anouncer , const string & fmt );
void init_log_file( int iteration , const char * proot = __DEFAULT_CONFIG_PACKAGE_ROOT );
void finish_log_file();


std::string ___logTr( string text );
void init_logTr_file();
void finish_logTr_file();

typedef tuple< double /*0cpu usage*/ , double /*1task_size*/ , double /*2task_output_size*/ > t_node_vars; // dax1 graph parameter
typedef map< int/*nodedaxid*/ , t_node_vars > t_map_nodes_vars; // dax1 graph parameter
typedef map< int/*nodedaxid*/ , DurationTime /*each node time duration*/ > t_map_nodes_duration; // nodes quota
typedef tuple< VMachine/*0*/ , double/*1deadline*/ , double /*2TimeConsume*/ , int /*3Parallism*/ , int /*4maxParallism*/ , t_map_nodes_duration/*5*/ > tuple_handler;
typedef map< int/*nodedaxid*/ , double /*katz*/ > t_map_node_katz; // nodes katz
typedef tuple< string/*0createtime*/ , t_map_nodes_vars/*1*/ , tuple_handler/*2*/ , t_map_node_katz/*3*/ , double/*4ccr*/> tuple_dag_configure;
typedef vector< tuple_dag_configure > vector_dag_configures;
typedef pair< vector_dag_configures , int /*count*/> vector_dag_configures_control;
typedef map< _GUID/*dag guid*/ , vector_dag_configures_control > dags_configures;

void init_xml_dag_guid_config_to_write();
void add_dag_guid_config( _GUID uid , tuple_dag_configure dag_configuration , bool baddto_backup );
//void add_dag_guid_katz( _GUID uid , map< int/*nodedaxid*/ , double /*katz*/ > , bool baddto_backup );
void finish_xml_dag_guid_config_for_write();

void load_backup_dag_guid( bool b_on_random_per_guid );

void read_xml_dag_guid_config( bool b_on_random_per_guid );
vector_dag_configures get_xml_dag_guid_config( _GUID uid );
void finish_xml_dag_guid_config_read();

/*empty input means just get active config instance*/

#define SETTINGS(...) Global_config::GetInstance( __VA_ARGS__ )

#define SETTINGS_FAST(...) Global_config::d_pActiveInstance
//#define SETTINGS_COMPOUND(...) ( SETTINGS_FAST(__VA_ARGS__) ?: SETTINGS(__VA_ARGS__) )

#define ___LOG(...) ___log( ( SETTINGS_FAST()->d_write_function_name_in_dump ) ? __FUNCTION__ : "" , __LINE__ , true , false , string_format( __VA_ARGS__ , "" ) )
#define ___LOG_NOTIME(...) ___log( "" , __LINE__ , false , false , string_format( __VA_ARGS__ , "" ) )
#define ___LOG_SIMULATION_TIME_ANOUNCER(...) ___log( ( SETTINGS_FAST()->d_write_function_name_in_dump ) ? __FUNCTION__ : "" , __LINE__ , true , true , string_format( __VA_ARGS__ , "" ) )
//#define ___LOG_F( ... ) do { ___log( __FUNCTION__ , __LINE__ , arg , __VA_ARGS__ ); } while(0)
#define LOG_TR(...) ___logTr( string_format( __VA_ARGS__ , "" ) )

#ifdef XBT_INFO
#undef XBT_INFO
class Global_config;
#define XBT_INFO_CONSOL(...) XBT_LOG( xbt_log_priority_info , ___LOG(__VA_ARGS__).c_str() , "" );

//#define XBT_INFO(...) XBT_INFO_CONSOL(__VA_ARGS__); 
//#define XBT_INFO(...) ___LOG(__VA_ARGS__); 
//#define XBT_INFO_NOTIME(...) ___LOG2(__VA_ARGS__);
#define XBT_INFO(...) do { if ( SETTINGS_FAST()->d_echo_XBT_INFO_OVERRIDEABLE >= AeDumpLevel::to_always_dump ) ___LOG(__VA_ARGS__); } while(false);
#define XBT_INFO_NOTIME(...) do { if ( SETTINGS_FAST()->d_echo_XBT_INFO_OVERRIDEABLE >= AeDumpLevel::to_always_dump ) ___LOG_NOTIME(__VA_ARGS__); } while(false);

// این پایینی میگه اگر وضعیت دامپ ورودی همونیکه در فایل هست خوب دانپ کن .
// حالا اگر اپلیکیشن در وضعیتی خاصی هست و نمایش گر وضعیت آنلاین بودن داره میگه من روی این وضعیت نشان بده خوب نشان بده
#define XBT_INFO_OVERRIDEABLE__(dumptype,...) do { if ( dumptype <= SETTINGS_FAST()->d_echo_XBT_INFO_OVERRIDEABLE ) ___LOG(__VA_ARGS__); } while(false);
#define XBT_INFO_ALIVE_CHECK(...) do { if ( ( SETTINGS_FAST()->d_system_alive_check <= SETTINGS_FAST()->d_echo_XBT_INFO_OVERRIDEABLE ) || (IS_BETWEEN( SETTINGS_FAST()->d_echo_XBT_INFO_OVERRIDEABLE , AeDumpLevel::dump_until_first_item , AeDumpLevel::dump_until_last_item ) && ( SETTINGS_FAST()->d_app_mode < ( AeAppMode )SETTINGS_FAST()->d_echo_XBT_INFO_OVERRIDEABLE )) ) ___LOG(__VA_ARGS__); } while(false);

#define LINE_SEPERATOR_SIGN "##EMPTYLINE##"
#define XBT_INFO_EMPTY_LINE() XBT_INFO( "" LINE_SEPERATOR_SIGN , "" );

#endif


#endif

#ifdef uses_Global_config

class Global_config
{
	string d_ver;
	//shared_ptr< Global_config > d_prev_loaded_config; // use in exit happend

	void readConfig( string sroot );
	void writeConfig();
	void defaultConfig();
	void post_read_action();
	//void load_prev_config();
	boost::property_tree::ptree make_json();
	void dump_periodicly();

public:

	time_t d_post_initialize_system_time; // do not read or write to or from file . system time different from simulation time
	int d_config_count;
	int d_repeat_each_config;
	int d_simulationIteration;
	
	string d_config_name;
	string d_tags;

	string d_config_file_root; // /mnt/c/simgrid/simgrid-3.32/out/build/WSL-GCC-Debug
	string d_config1_file_path; // /mnt/c/simgrid/simgrid-3.32/out/build/WSL-GCC-Debug/config1.txt

	string d_dax_file_root; // /mnt/c/simgrid/simgrid-3.32/out/build/WSL-GCC-Debug/examples/c/actor-create
	string d_dax1_file_path; // /mnt/c/simgrid/simgrid-3.32/out/build/WSL-GCC-Debug/examples/c/actor-create/DAX1_frombase.txt
	string d_dax2_file_path; // /mnt/c/simgrid/simgrid-3.32/out/build/WSL-GCC-Debug/examples/c/actor-create/DAX2_fromDAGS.txt
	string d_backup_dag_guid_tmp_path; // /mnt/c/simgrid/simgrid-3.32/out/build/WSL-GCC-Debug/examples/c/actor-create/backup_dag_guid.tmp
	


	uint64_t d_config_write_count;
	uint64_t d_config_read_count;

	static AeAppMode d_app_mode;

	//bool d_enable_all_dump_echo_notif_just_in_runtime;
	bool d_force_dump_config;
	bool d_init_Vmm_DGG; // good for making config complete and referesh
	bool d_Mode_MAKE_DAX2_FOR_SIMULATION; // for make dax2
	bool d_make_dag_guid_config;

	int d_make_dag_n_guid_config;
	bool d_use_dag_guid_config_cache;
	bool d_echo_add_dag;
	bool d_echo_nextTime_add_dag;
	bool d_notify_callback_actor_DGGenerator_finished;
	bool d_order_stop_loop_dag_storage_add;
	bool d_order_stop_loop_dag_schedule;
	static bool d_order_dump_result_and_stop_program; // نتایج تا اینجا را می نویسد و بعد کلا برنامه را می بندد و اجرا های بعدی را رها می کند
	bool d_notify_dag_first_time_scheduled;
	bool d_notify_dag_new_node_to_schedule;
	int d_declare_dag_finish_each_n_iteration;
	bool d_echo_Ready2Schedule_node_list;
	bool d__echo_one_time__Ready2Schedule_node_list;
	bool d_dump_dag_storage_d_dags_in_init_DAGenerator;
	double d_add_one_dag_per_x_second; // default 60.0
	bool d_dump_at_start_to_use_cycle_mgr_report;
	bool d_dump_at_end_use_cycle_mgr_report;
	bool d_echo__calculate_quality_ratio__value_chack;
	bool d_echo__callback_actor_container__start_execute;
	bool d_echo__callback_actor_container__done_execute;
	bool d_echo__callback_actor_container__done_execute_dump_bundle;
	bool d_dump_region_after_creation;
	bool d_notify_init_VManagement_finished;
	bool d_echo__scheduleOnVM__schedule_new_node;
	bool d_echo__scheduleOnVM__nearest_node;
	bool d_echo__scheduleOnVM__try_to_find_Provisioned_vm;
	bool d_echo__scheduleOnVM__try_to_find_Provisioned_vm_calculate_return_false;
	bool d_echo__scheduleOnVM__try_to_find_UNprovisioned_vm;
	bool d_echo__scheduleOnVM__dump_calc_ratio;
	bool d_echo__scheduleOnVM__details;
	SimulationDurationTime d_VIRTUALMACHINE_WARMUP_TIME_SEC;  // 100.0 /*second*/
	SimulationDurationTime d_VM_LEASED_CYCLE_TIME_SEC;   // 3600.0 /*sec*/
	double d_DAG_GENERATOR_HOST_SPEED;   // 1e10 /*not usable for now*/
	double d_VMM_HOST_SPEED;   // 1e10 /*not usable for now*/
	double d_GLOBAL_STORAGE_MEMORY_SPEED;   // 1e10
	SimulationDurationTime d_CONTAINER_INITIALIZATION_DALEY_TIME;   // 10.0 /*sec*/
	SimulationDurationTime d_schedule_periode_delay_time; // simulation time . default 2 * 60
	bool d_enable_DAG_Deadline_Quota_notify__declare_dag_start_time;
	bool d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__finish_declare;
	bool d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__status_dag_to_deadline;
	bool d_enable_DAG_Deadline_Quota_notify__declare_dag_finish_time__dump_dag;
	bool d_enable_Task_Deadline_Quota_notify;
	int d_region_count; // 3
	int d_vm_per_region_count; // 2000
	bool d_add_dax2_graph; // true
	bool d_echo_minimum_maximum_calc_proc_dag_on_vm;
	bool d_echo_average_deadline_calced_for_dag;
	int d_reset_minimum_estimate_time_consumption;
	int d_reset_maximum_estimate_time_consumption;
	bool d_notify_dag_loaded;
	bool d_dump_dag_with_long_name;
	bool d_warn_impossible_condition;
	//bool d_dump_one_uncomplete_dag;
	SystemDurationTime d_elapse_time_between_remain_time_estimation; // second , -1 disable
	bool d_dump_Ready2ScheduleNode_order_priority;
	bool d_echo__time_comsume_to_process_on__deadline;
	bool d_shuffle_candidate_qualities; // true
	bool d_use_poisson_distribution_for_next_add_dag_time;
	bool d_shuffle_dags_after_loaded_to_storage;
	bool d_warn_if_some_vm_type_are_not_avaiable_for_leased;
	int d_sort_schedule_list_policy_ver; // 1 , 2 , 3 , 4
	bool d_dump_each_simulation_iteration_result;
	bool d_notify_callback_actor_scheduler_finished;
	bool d_use_wait_policy;
	int d_wait_policy_ver; // 1 , 2 , 3
	bool d_echo_one_time_qualities_with_wait_policy;
	bool d_dump_all_qualities_in_schedule_node;
	bool d_echo__scheduleOnVM__top_quality_with_wait_policy_details;
	bool d_echo__scheduleOnVM__top_quality_non_wait_policy_details;
	bool d_echo__scheduleOnVM__top_quality_with_wait_policy_details_full_dag_dump;
	bool d_echo__scheduleOnVM__top_quality_non_wait_policy_details_full_dag_dump;
	bool d_dump_node_deadline_spare_time;
	int d_reset_values_type; // 2 = low fat , 4 = hi fat
	double d_dump_bundle_if_estimation_and_real_execution_time_different_larger_than;
	bool d_print_bundle_internal_externals;
	int d_EPSM_algorithm_ver;
	bool d_echo_target_to_allocate_new_vm_in_EPSM;
	bool d_echo_target_to_allocate_new_vm_in_EPSM_dump_quality;
	bool d_echo_target_to_allocate_new_vm_in_EPSM_dump_quality_full_dag_dump;
	double d_deprovision_delay_duration; // فاصله زمانی بین هر دور حذف ماشین های اضافی
	double d_deprovisioning_delay_estimate; // زمان لازم برای لغو اجاره ماشین
	double d_PROV_POLLING_TIME; // زمان پتالتی برای ماشین اجاره شده تا باقی بماند و در صورت نیاز از دور اجاره شده استفاده گردد
	int d_dump_deprovision_count_if_greater_than;
	bool d_write_function_name_in_dump;
	double d_increase_deadline_after_no_machine_match;
	bool d_use_cache_for_XBT_INFO;
	AeDumpLevel d_echo_XBT_INFO_OVERRIDEABLE; // اعداد کمتر از این عدد دامپ می شود
	AeDumpLevel d_system_alive_check; // دامپ هایی که زنده بودن برنامه را نشان می دهد
	SystemDurationTime d_elapsed_time_in_long_loop;
	bool d_use_deprovisioning_vm_policy;
	double d_SectionTimeKeeper_minimum_duration_to_dump_slow_part_in_milisecond;
	//bool d_dump_how_much_vm_leased_for_bundle_scheduling_in_iteration;
	bool d_run_estimator;
	SystemDurationTime d_simulation_time_anouncer_delay;
	double d_lan_bandwidth;
	double d_all_machine_type_memory_access_speed;
	int d_isSuitableForAnyMachine__ver;
	bool d_break_after_bundle_not_suitable;
	bool d_use_least_price_vm_to_lease_new_vm;
	bool d_low_memory_mode;
	bool d_calculate_katz; // very important
	int d_override_cumulative_sleep; // -1
	int d_quality_ratio_calculation_ver; // 1 , 2
	bool d_dump_top_quality_vm_stat;
	int d_estimation_ver; // 1 normal  2 log  3 radical
	int d_calculate_dag_execution_duration_ver; // 1 . serially . 2 parallel

	int d_max_ranje_DAG_Type;
	
	int d_max_bundle_capacity;
	int d_resource_heterogeneity;
	int d_greedy_expantion_ver; // 1 dijkestra . 2 BFS
	int d_sort_candidate_qualities_ver; // 0 no sort , 1 sort desc
	bool d_use_free_slot_policy;
	int d_vm_share_policy_ver; // 1 full , 2 only dag , 3 only type . each machine only process same dag
	
	

	static map<void * , tuple<char * , int , int> > d_allocation_counter;

	// member fxn
	static shared_ptr< Global_config > d_pActiveInstance;
	Global_config( int simulationIteration );
	static shared_ptr< Global_config > GetInstance( int simulationIteration = -1 );
	string make_config_tags();
	string echo_config( bool on_cosole );
	void closeConfig();
};

#endif


#define BEGIN_TRY try {
#define CATCH_TRY }catch(exception exp){ /*XBT_INFO( "#(%d) %s" , __LINE__ , exp.what() );*/ assert( false ); }
#define CATCH_TRY_SILENT }catch(...){ }


#define SIMULATION_NOW_TIME sg4::Engine::get_clock()


class SimulationTimeKeeper
{
public:
	SimulationTimeKeeper( DurationTime & duration , bool reset_duration_or_append ) : d_duration( duration )
	{
		d_start = SIMULATION_NOW_TIME;
		d_reset = reset_duration_or_append;
	}
	~SimulationTimeKeeper()
	{
		d_end = SIMULATION_NOW_TIME;
		if ( d_reset )
		{
			d_duration = d_end - d_start;
		}
		else
		{
			d_duration += d_end - d_start;
		}
	}

	SimulationAbsoluteTime d_start , d_end;
	bool d_reset;
	SimulationDurationTime & d_duration;
};

class SystemTimeKeepr
{
public:
	SystemTimeKeepr();
	~SystemTimeKeepr();

	time_t d_start;

	bool peek_time( SystemDurationTime elapsed = 0 , bool reset_time_to_now = true );
};

#ifdef uses_SectionTimeKeeper

class SectionTime;
class SectionTimeKeeper
{
public:
	SectionTimeKeeper( SectionTimeKeeper ** ppGlobalKeeper , double dump_minimum_time_milisecond = -1 /*no dump*/);
	~SectionTimeKeeper();

	void add_section( SectionTime * pSection );

	SectionTimeKeeper ** d_ppGlobalKeeper;
	double d_dump_minimum_time_milisecond;
	std::chrono::_V2::system_clock::time_point d_t1;
	std::chrono::_V2::system_clock::time_point d_t2;
	map< int /*line number*/ , vector< SectionTime > > d_section_benchmark;
};

class SectionTime
{
public:
	SectionTime( SectionTimeKeeper * pKeeper , int line , string snote );
	~SectionTime();

	int d_line_number;
	string d_snote;
	SectionTimeKeeper * d_pKeeper;
	std::chrono::_V2::system_clock::time_point d_t1;
	std::chrono::_V2::system_clock::time_point d_t2;
};

#endif

class SectionAliveCheck
{
public:
	SectionAliveCheck( int line_number , string scomment = "" );
	~SectionAliveCheck();

	int d_line_number;
	string d_comment;
	int d_section_id;
};

#define CONCAT_NAME__( a , b ) a ## b
#define CONCAT_NAME_( a , b ) CONCAT_NAME__(a,b)

#define SECTION_ALIVE_CHECK(...) SectionAliveCheck CONCAT_NAME_( section_alive_check , __LINE__ )( __LINE__ , __VA_ARGS__ ); XBT_INFO_ALIVE_CHECK( "#(%d) section ↓(%d) (%s)" , CONCAT_NAME_( section_alive_check , __LINE__ ).d_line_number , CONCAT_NAME_( section_alive_check , __LINE__ ).d_section_id , CONCAT_NAME_( section_alive_check , __LINE__ ).d_comment.c_str() );

template<typename T> inline T * __new( T * t , const char * file , int line )
{
	if ( Global_config::d_allocation_counter.count( t ) == 0 )
	{
		Global_config::d_allocation_counter[ t ] = make_tuple( (char *)file , line , 0 );
	}

	std::get<2>( Global_config::d_allocation_counter[ t ] )++;

	return t;
};
template<typename T> inline void __delete( T * t )
{
	std::get<2>( Global_config::d_allocation_counter[ t ] )--;
	delete t;
};

//#define __NEW( T ) __new( new T , __FILE__ , __LINE__ ) 
//#define __DELETE( T ) __delete( T )
#define __NEW( T ) new T
#define __DELETE( T ) delete T

template<typename T> class CPointer
{
public:
	CPointer()
	{
		allocated_ptr = NULL;
	}
	CPointer( T & src_p ) : CPointer()
	{
		allocate();
		*allocated_ptr = src_p;
	}
	CPointer( const CPointer<T> & src ) : CPointer()
	{
		this->allocated_ptr = src.allocated_ptr;
		memset( (void *)&src , 0 , sizeof(src));
	}
	void operator = ( const CPointer<T> & src )
	{
		assert(allocated_ptr == NULL);
		this->allocated_ptr = src.allocated_ptr;
		memset( (void *)&src , 0 , sizeof(src));
	}
	void allocate()
	{
		if ( allocated_ptr != NULL )
		{
			cleanup();
		}
		allocated_ptr = __NEW( T() );
	}
	~CPointer()
	{
		cleanup();
	}
	void cleanup()
	{
		if ( allocated_ptr != NULL )
		{
			__DELETE( allocated_ptr );
			allocated_ptr = NULL;
		}
	}

	T * allocated_ptr;
};


class ScopeSemaphore
{
public:
	ScopeSemaphore( map< int , pair< int , int > > * pMap , int key2semaphore )
	{
		d_pMap = NULL;
		d_pConcurrency = NULL;

		d_pMap = pMap;
		d_key2semaphore = key2semaphore;

		if ( d_pMap->count( d_key2semaphore ) == 0 )
		{
			( *d_pMap )[ d_key2semaphore ] = make_pair( 0 , 0 );
		}
		( *d_pMap )[ d_key2semaphore ].first++;
		if ( ( *d_pMap )[ d_key2semaphore ].first > ( *d_pMap )[ d_key2semaphore ].second )
		{
			( *d_pMap )[ d_key2semaphore ].second = ( *d_pMap )[ d_key2semaphore ].first;
		}
	}
	ScopeSemaphore( int * pConcurrency , int * pMaxConcurrency )
	{
		d_pMap = NULL;
		d_pConcurrency = NULL;

		d_pConcurrency = pConcurrency;
		( *d_pConcurrency )++;
		if ( ( *d_pConcurrency ) > ( *pMaxConcurrency ) )
		{
			( *pMaxConcurrency ) = ( *d_pConcurrency );
		}
	}
	~ScopeSemaphore()
	{
		if ( d_pMap != NULL )
		{
			( *d_pMap )[ d_key2semaphore ].first--;
		}
		if ( d_pConcurrency != NULL )
		{
			( *d_pConcurrency )--;
		}
	}

	map< int , pair< int , int > > * d_pMap;
	int d_key2semaphore;

	int * d_pConcurrency;
};

#define _ONE_TIME_VAR() static bool _one_time = false;
#define _ONE_TIME( exp ) if ( !_one_time ) { _one_time = true; exp; }

