#pragma once

struct DAGNode;
class DAG;
class ClassVM;

struct DAGEdge
{
	int d_edge_id;
	//string d_edge_name; // 40 byte Wow

	double * d_p_network_usage_byte; // each node output fix size to every childs
	double normalized_inverse_network_usage; // chon 1 / yal edge weight kheily kochack mishod va moshkel barayeh dijkestra ijad mikard faleza man yek meghdar normal shodeh barayeh edge weight mohasebeh mikonam

	DAGNode * d_pSource;
	DAGNode * d_pDestination;

	DAGEdge( DAGNode * srcSource , DAGNode * srcDest );
	void init_edge(); // make link and parent both

	void dump( bool inside_another );
};

struct DAGNode
{
	eNodeID d_node_uniq_id; // each node has id
	int d_node_dax_id; // id in DOT
	char * d_allocated_node_name; // allocate and delete by my self

	double d_task_cpu_usage_flops; // cpu cost
	double d_task_size_byte; // task transfer network cost
	double d_task_output_size_byte; // task output network transfer cost to global storage

	DAG * d_pOwnerDAG; // each node has owner dag

	vector< DAGNode * > d_parents; // parents
	vector< DAGEdge * > d_links; // link to childs

	int d_ended_parent; // در موقع محاسبه ماشین مناسب کاربرد دارد ولا غیر . reset it before use

	double d_Katz_value;
	Task_Deadline_Quota d_task_deadline;

	struct runtime_neccessary
	{
		bool d_taskInSchedulList;
		bool d_taskScheduled_in_vm;
		bool d_taskProccessed;

		map< ContainerID , ClassVM * > d_probable_keeper_vm; // برای اینکه پی ببریم هر نود دیتای خروجی را در کدام ماشین و کانتینر دارد از این لیست کانتینر احتمالی استفاده می کنیم
		int d_sort_iteration;

		runtime_neccessary();
		~runtime_neccessary();
		void cleanup();
	} *d_pRuntime; // from moment owner dag inserted in schedule list until execution complete

	// member fxn

	DAGNode( const char * pName );
	~DAGNode();
	void cleanup();

	AbsoluteTime * get_actual_triggered_time();
	AbsoluteTime * get_actual_complete_time();
	void dump( bool inside_another , bool dump_links );
};

struct DAGTag
{
	DAGTag( string sname , string svalue )
	{
		name = sname;
		value = svalue;
	}

	string name;
	string value;
};

class DAG
{
public:
	// data member
	eDAG_ID d_dag_uniq_id;
	int d_dag_dax_id; // پل رابط دگ ها بین دات و دکس 1 و دکس 2
	
	DAG_config d_dag_config;
	double d_ccr;

	DAG_Deadline_Quota d_dag_deadline;
	eVM_Type d_least_price_vmp_type; // اول کاربردش برای آمار کار بود و بعد ازش می توانم برایش کف ماشین جدید قابل اجاره استفاده کنم
	int d_nodes_count; // می خواهیم بعد از تمام شدن کار دگ تمام دیتای اضافی را پاگ کنیم
	int d_MaximumPossibleParallelism; // نهایت موازی سازی ممکن

	struct maketime_data
	{
		DurationTime TotalTimeConsume;
		map< eNodeID , DurationTime /*each node time duration*/> node_exec_time_duration;
		VMachine  base_least_price_vm;
		VMachine * p_least_price_vm;
		//int ExtraParallelVM;

		maketime_data()
		{
			cleanup();
		}
		void cleanup()
		{
			TotalTimeConsume = 0;
			node_exec_time_duration.clear();
			p_least_price_vm = NULL;
			//ExtraParallelVM = 0;
		}
	};

	struct runtime_neccessary
	{
		string d_dag_name;
		string d_dag_short_name;
		vector< DAGTag > d_dagtags;
		vector< DAGNode * > d_roots; // link list
		vector< DAGNode * > * d_ordered_nodes; // access to shared_pt is 6 time slower than access to pointer
		map< eNodeID , DAGNode * > * d_shuffled_nodes; // keep new node
		map< eNodeID , DAGNode * > * d_swap_shuffled_nodes; // صرفا برای اینکه بعد از لود دگ تا زمانیکه نود ها لیست شود اسمارت پوینتر ولید بماند

		runtime_neccessary();
		~runtime_neccessary();
		void cleanup();

	} *d_pRuntime; // from moment dag created until dag execution done
	

	/// statistic
	struct statistic_neccessary
	{
		double d_dag_communication_byte_need; // بایت لازم برای اجرای این دگ
		DurationTime d_dag_communication_duration; // زمانی که بایت انتقالی گرفته . انتظار دارم ر چه دگ ارتباط محور باشد زمان کمتر شود

		double d_dag_expectation_communication_byte_on_slow_link; // مقدار بایتی که انتظار می رود بر روی شبکه کند منتقل شود
		double d_dag_reality_communication_byte_on_slow_link; // مقداری که واقعا بر روی شبکه کند جابجا می شود

		double d_dag_computation_time_on_avg_machine; // زمان میانگین مجموه اجرای تسک ها
		double d_dag_reality_computation_time; // زمان مجموع اجرای تسک ها

		statistic_neccessary();

	} *d_pStatistic; // from moment dag inserted in schedule list untill iteration dump and finished


	///////////////////////////////////////////////////////////////////////////
	//memnber fxn
	DAG();
	~DAG();
	void cleanup();
	void clear_unnecessary_data_after_dag_complete();

	static eDAG_ID get_uniq_id();
	bool doesAllTasksCompleted()
	{
		return d_dag_deadline.isDagFinishedExecution();
	}
	bool hasUnprocessedTask();
	//int64_t countProcessedTask();
	void make_ordered_nodes();
	void make_shuffled_nodes();
	bool calculate_nodes_sort_order();
	void dump( bool inside_another , bool b_short );
	
	//DurationTime execute_serially_on_one_machine_time( VMachine machine );
	DurationTime execute_parallel_time( VMachine machine , int extra_vm_leased = -1 , int * pMaximumExtraParallelVM = NULL ,
										int * pMaximumPossibleParallelVM = NULL , map< eNodeID , DurationTime /*each node time duration*/> * pStoreDuration = NULL );

	DurationTime shortest_time();
	DurationTime longest_time();

	//DurationTime time_comsume_to_process_on( int * pMaximumExtraParallelVM = NULL );
	DurationTime time_comsume_to_process_on();
	//map< int /*edge id*/ , DAGEdge * > get_all_edges();

	void calc_edge_weight_normalize();
	bool complete_dag_structure( maketime_data * pmaketime_data = NULL , tuple_dag_configure * pcache = NULL );
	void check_dag();
	double calculate_ccr();
	void reset_values_with_ccr( bool b_append_ccr_into_name ); // ccr must be set before
	string get_tag( string name );
	string get_name();
};

#pragma region convert daxDag 2 Dag

struct DAX1Node;
struct DAX1Dag;

struct DAX1Edge
{
	DAX1Node * d_src;
	DAX1Node * d_dst;

	DAX1Edge()
	{
		d_src = NULL;
		d_dst = NULL;
	}
	DAX1Edge( DAX1Node * psrc , DAX1Node * pdst )
	{
		d_src = psrc;
		d_dst = pdst;
	}
};

struct DAX1Node
{
	int d_daxnode_nodeid;
	DAX1Dag * d_owner;

	vector< DAX1Edge * > d_links;
	map< int , DAX1Node * > d_parents;

	double d_task_cpu_usage_flops; // cpu cost
	double d_task_size_byte; // task transfer network cost
	double d_task_output_size_byte; // task output network transfer cost to global storage

	DAX1Node( int id );
	~DAX1Node();
	void cleanup();
};

struct DAX1Dag
{
	int d_daxdag_dagid;
	string d_daxdag_name;
	//int d_daxdag_dagtype;
	vector< DAGTag > d_daxtags;
	map< int , DAX1Node * > d_roots;
	map< int , DAX1Node * > d_nodes;

	void serializeDAX2( std::ostream & os )
	{
		os << "\n" << "[dag]" << "[daxid][" << d_daxdag_dagid << "][dagtype][" << -1 << "][dagname][" << d_daxdag_name << "][~dagname]";
		if ( d_daxtags.size() > 0 )
		{
			os << "\n" << "[tags]";
			for ( auto & tag : d_daxtags )
			{
				os << "[" << tag.name << "=" << tag.value << "]";
			}
			os << "[~tags]";
		}
		
		os << "\n" << "[node]" << "[nodecount][" << d_nodes.size() << "]";
		for ( auto & node : d_nodes )
		{
			os << "[nodeid][" << node.first << "]";
			os << "[task_cpu_usage_flops][" << node.second->d_task_cpu_usage_flops << "]";
			os << "[task_size_byte][" << node.second->d_task_size_byte << "]";
			os << "[task_output_size_byte][" << node.second->d_task_output_size_byte << "]";
		}
		os << "[~nodecount][~node]";

		os << "\n" << "[parents]";
		for ( auto & node : d_nodes )
		{
			for ( auto & parent : node.second->d_parents )
			{
				os << "[childnodeid][" << node.first << "][parentnodeid][" << parent.first << "]";
			}
		}
		os << "[~parents]";

		os << "\n" << "[rootcount][" << d_roots.size() << "]";
		for ( auto & node : d_roots )
		{
			os << "[noderootid][" << node.first << "]";
		}
		os << "[~rootcount][~dag]";
	}
	//void serializeDAX1( std::ostream & os )
	//{
	//	os << "\n" << "[dag]" << "[daxid][" << d_daxdag_dag_id << "][dagtype][" << d_dag_node_type << "][dagname][" << d_dax_dag_name << "][~dagname]";
	//	if ( d_daxtags.size() > 0 )
	//	{
	//		os << "\n" << "[tags]";
	//		for ( auto & tag : d_daxtags )
	//		{
	//			os << "[" << tag.name << "=" << tag.value << "]";
	//		}
	//		os << "[~tags]";
	//	}
	//	os << "\n" << "[node]" << "[nodecount][" << d_nodes.size() << "]";
	//	for ( auto & node : d_nodes )
	//	{
	//		os << "[nodeid][" << node.first << "]";
	//	}
	//	os << "[~nodecount][~node]";
	//	os << "\n" << "[parents]";
	//	for ( auto & node : d_nodes )
	//	{
	//		for ( auto & parent : node.second->d_parents )
	//		{
	//			os << "[childnodeid][" << node.first << "][parentnodeid][" << parent.first << "]";
	//		}
	//	}
	//	os << "[~parents]";
	//	os << "\n" << "[rootcount][" << d_roots.size() << "]";
	//	for ( auto & node : d_roots )
	//	{
	//		os << "[noderootid][" << node.first << "]";
	//	}
	//	os << "[~rootcount][~dag]";
	//}
	DAX1Dag();
	~DAX1Dag();
	void cleanup();
};

class DAGstorage;
struct DAX1Dags
{
	string d_daxver;
	vector< DAX1Dag * > d_dax_dags;
	DAGstorage * pstorage;

	DAX1Dags();
	~DAX1Dags();
	void cleanup();


	void serializeDAX2( std::ostream & os , string pname )
	{
		os << "[dags]" << "[dagcount][" << d_dax_dags.size() << "][pname][" << pname << "][DAX_VER][standard2.0]";
		for ( auto & dax_dag : d_dax_dags )
		{
			dax_dag->serializeDAX2( os );
		}
		os << "[~dags]";
	}
	//void serializeDAX1( std::ostream & os )
	//{
	//	os << "[dags]" << "[dagcount][" << d_dax_dags.size() << "][DAX_VER][standard1.0]";
	//	for ( auto & dax_dag : d_dax_dags )
	//	{
	//		dax_dag->serializeDAX1( os );
	//	}
	//	os << "[~dags]";
	//}
	void read_dax1_from_file();
	void read_dax2_from_file();
};

#pragma endregion

class DAGstorage
{
public:
	DAGstorage();
	~DAGstorage();
	void cleanup();

	string d_pname;
	DAX1Dags d_DAXdags;
	vector< DAG * > d_dags; // shared_pt is 6 times sllower to access

	//void make_random_graphs();

	void read_and_convert_dax1_to_dags();
	//void convert_dags_to_dax2(); // دیگه 2 را نمی سازم برای رندوم ها زیرا باید گرفا ها مقادیر رندوم داشته باشند

	void read_and_convert_dax2_to_dags();
	//void convert_dags_to_dax1();
	
	//void convert_dags_to_DOT1();
	//void convert_dax1_to_DOT1();
};

typedef map< eNodeID , DAGNode * > _map_nodes;
typedef vector< DAGNode * > _vector_nodes;