


#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Solver
{
public:


	Solver():hole_vec_(nullptr){};
	~Solver()
	{
		clear();
	};

	void clear()
	{
		if(hole_vec_)
		{
			delete []hole_vec_;
			hole_vec_ = nullptr;
		}
	}


	void solve(int road_len, int hole_count, int* hole_vec, int tape_len)
	{
		init(road_len, hole_count, hole_vec, tape_len);

		int* hole_stats = new int[hole_count_];
		
		for(int i = 0; i < hole_count_; i++)
		{
			hole_stats[i] = 1; // the inital hole is not covered.
		}

		std::cout<<"road_len "<<road_len_<<" tape_len " << tape_len_ <<" hole count " << hole_count_ << std::endl;
		for(int i = 0; i < hole_count_; i++)
		{
			std::cout<<"hole [" <<i<<"] = "<< hole_vec_[i] << std::endl;	
		}

		std::cout<<"Solving ... "<<std::endl;


		int max_num_tape = hole_count_;

		max_cache_index_from_holes_ = 1;
		for(int i = 0; i < hole_count_; i++)
		{
			max_cache_index_from_holes_ *= 2;
		}


		cached_costs_ = new int[max_cache_index_from_holes_ * max_num_tape];
		for(long long i = 0; i < max_cache_index_from_holes_ * max_num_tape; i++)
		{
			cached_costs_[i] = -1;
		}
		total_cost_evaluation_ = 0;
		use_cached_cost_ = 0;

		int value = dp_solve(hole_stats, max_num_tape);

		std::cout<<"Result: minimum num of tapes = " << value/cost_per_tape_ << std::endl;

		std::cout<<"Cost evaluation count = " << total_cost_evaluation_ <<std::endl;
		std::cout<<"Times of using cached cost = " << use_cached_cost_ <<std::endl;

		delete []cached_costs_;
		cached_costs_ = nullptr;

		delete []hole_stats;
	}

private:

	long long calc_cache_index(int* hole_stats, int max_num_tape) const
	{
		long long cache_index = calc_cache_index_from_holes(hole_stats) +  max_cache_index_from_holes_ * (max_num_tape - 1);
		return cache_index;
	}


	long long calc_cache_index_from_holes(int* hole_stats) const
	{
		long long v = 0;
		int k = 1;
		for(int i = 0; i < hole_count_; i++)
		{
			v += k * hole_stats[i];
			k = k * 2;
		}

		return v;
	}

	long long calc_penalty_of_holes(int* hole_stats) const
	{
		long long p = 0;
		
		for(int i = 0; i < hole_count_; i++)
		{
			p += hole_stats[i] * peanlty_per_hole_;
		}


		return p;
	}


	int dp_solve(int* hole_stats, int max_num_tape)
	{

		//add memorization
		if (max_num_tape == 0)
		{
			return calc_penalty_of_holes(hole_stats);
		}


		long long cache_index = calc_cache_index(hole_stats, max_num_tape);

		if (cached_costs_[cache_index] != -1)
		{
			use_cached_cost_ ++;
 			return cached_costs_[cache_index];
		}


		


		long long min_cost = hole_count_ * peanlty_per_hole_ + hole_count_ * cost_per_tape_;

		//apply one tape
		for(int i = 0; i < road_len_ - tape_len_ + 1 ; i++)
		{
			int* updated_holes = new int[hole_count_];
			memcpy(updated_holes, hole_stats, sizeof(int) * hole_count_);

			for(int j = 0; j <  hole_count_; j++)
			{
			
				int hole_pos = hole_vec_[j];

				if(hole_pos >= i && hole_pos < i + tape_len_)
				{
					updated_holes[j] = 0;
				}
			}

			
			long long sub_cost = dp_solve(updated_holes, max_num_tape - 1);
			long long cost = cost_per_tape_ + sub_cost ;

			if ( cost < min_cost)
			{
				min_cost = cost;
			}

			delete []updated_holes;
		}

		//not apply this tape
		{
			
			long long sub_cost = dp_solve(hole_stats, max_num_tape - 1);
			long long cost =   sub_cost ; 

			if ( cost < min_cost)
			{
				min_cost = cost;
			}
		}

		total_cost_evaluation_ ++;
		cached_costs_[cache_index] = min_cost;

		return min_cost;
	}




	void init(int road_len, int hole_count, int* holes, int tape_len)
	{
		clear();

		road_len_ = road_len;
		tape_len_ = tape_len;
		hole_count_ = hole_count;

		hole_vec_ = new int[hole_count_];

		for(int i = 0; i < hole_count; i++)
		{
			hole_vec_[i] = holes[i];
		}

		cost_per_tape_ = 1;
		peanlty_per_hole_ = cost_per_tape_ * hole_count_; 
	}

	int road_len_;
	int tape_len_;
	int hole_count_;
	int* hole_vec_ ;

	int peanlty_per_hole_;
	int cost_per_tape_;

	int* cached_costs_;
	long long max_cache_index_from_holes_;

	int total_cost_evaluation_;
	int use_cached_cost_;

};




int main()
{
	
	std::ifstream ifs("data.txt");

	std::string str; 

	int use_case_count = 0;

    while (std::getline(ifs, str))
    {
    	if(str.empty())
    	{
    		continue;
    	}

    	if (str[0] == '#')
    	{
    		continue;
    	}

    	use_case_count = std::stoi(str);
    	break;
    }


    std::cout << "use cases count = "<< use_case_count << std::endl;


    for(int i = 0; i < use_case_count; i++)
    {
    	std::cout << "use case #" <<i<< std::endl;

    	int road_len;
    	int tape_len;
    	int hole_count;

    	ifs >> road_len;
    	ifs >> tape_len;
    	ifs >> hole_count; 
    	
    	int* hole_vec = new int[hole_count];

    	for(int k = 0; k < hole_count; k++)
    	{
    		ifs >> hole_vec[k];
    	}

    	Solver s;
		s.solve(road_len, hole_count, hole_vec, tape_len);	

		delete []hole_vec;
	}

}