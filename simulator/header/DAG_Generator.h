#pragma once

class BenchmarkIteration;
struct VMQuality;

class DGG // DAGGenerator
{
public:
	int d_DGG_Id;

	BenchmarkIteration * d_pIteration;
	unique_ptr< bool > d_resume_iteration;

	sg4::Host * d_pDGGeneratorHost;
	sg4::ActorPtr d_pDGGeneratorActor;

	sg4::Host * d_pDGSchdulerHost;
	sg4::ActorPtr d_pDGSchdulerActor;

	sg4::Host * d_pEstimateHost;
	sg4::ActorPtr d_pEstimateActor;

	vector< DAG * > d_entry_dags; // نهایتا برای ارزیابی بدرد می خورد
	vector< DAGNode * > d_lstReady2ScheduleNode;
	int d_finished_dag_count;

	sg4::MutexPtr d_critical_section_entered_mutex;

	DAGstorage d_dag_storage;
	int d_sort_iteration;
	//int d_in_each_sort_try_schedule;
	//int d_in_each_sort_try_schedule_max;

	long double d_total_proccess_node_edge;
	long double d_proccessed_node_edge;

	////////////////////////////////////////
	// member fxn
	DGG();
	void flush_cache();

	void init_DAGenerator( sg4::NetZone * srcpZone , int simulationIteration );
	void init_DAGStorage();
	
	void resumeIteration();
	void IterationFinished( bool b_last_iteration );
	
	void callback_actor_scheduler();
	void callback_actor_dag_generator();
	void callback_actor_estimator();
	
	void addDAG( DAG * refDAG );
	bool hasUnprocessedDAGinReady2ScheduleList();
	int sort_schedule_list();

	void post_process_DAG( VMQuality * pQuality );
	void echo_time_estimation();
	void dump();
};

