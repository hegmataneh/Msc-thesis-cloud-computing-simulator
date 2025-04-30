#pragma once

class CBenchmark;

struct BenchmarkIteration
{
	int d_config_index;
	int d_iteration_index;

	shared_ptr< DGG > pDGG;
	shared_ptr< Vmm > pVMM;
	shared_ptr< BenchmarkIteration > pNextIteration;
	shared_ptr< BenchmarkIteration > pPrevIteration;

	CBenchmark * pBenchmark;

	BenchmarkIteration()
	{
		d_config_index = -1;
		d_iteration_index = -1;
		pNextIteration = NULL;
		pPrevIteration = NULL;
		pDGG = NULL;
		pVMM = NULL;
		pBenchmark = NULL;
	}

	void init_iteration( sg4::NetZone * pZone );
	void startIteration();
	void IterationFinished( bool b_last_iteration );
	void cleanup_last_iteration();
};

class CBenchmark
{
public:
	CBenchmark();
	~CBenchmark();

	NetZone * d_pZone;
	vector< shared_ptr< BenchmarkIteration > > d_iterations;
	static shared_ptr< BenchmarkIteration > d_current_benchmark;

	void add_one_iteration( int config_index );
	void startNextIteration( DGG * pFinishedDGG = NULL );
};
