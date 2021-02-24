#include <bits/stdc++.h>

using namespace std;
typedef vector<pair<int *, size_t>> v_load;

// Estrutura que armazena trocas, comparacoes e tempo
struct loginfo_t
{
	unsigned long long int trocas, comparacoes;
	double elapsed_time;

	loginfo_t()
	{
		trocas = comparacoes = elapsed_time = 0;
	}
};

class Benchmark
{
private:
	vector<void (*)(int *, size_t, loginfo_t *)> functions; // funcoes para teste
	vector<string> function_names;							// nomes das funcoes
	vector<int> array_sizes = {100, 1000, 10000, 100000};	// tamanhos das cargas de teste
	loginfo_t ***results;									// array 3-dimensional para armazenar resultados
	vector<v_load> loads;									// armazenamento das cargas <int arr*, size_t tamanho_arr>
	bool verbose = false;
	int input_types = 3;

	void allocate_results(int size_functions);
	void deallocate_results();
	void copy_arrays(int *src, int *dest, size_t arr_size);
	void deallocate_load();
	void generate_load();
	void print_array(int *arr, size_t array_size);

public:
	Benchmark(vector<void (*)(int *, size_t, loginfo_t *)> funcs, vector<string> func_names) : functions(funcs), function_names(func_names)
	{
		allocate_results((int)funcs.size()); // aloca memória para a matriz de resultados
	};

	~Benchmark()
	{
		deallocate_results();
	};

	void set_input_types(int ninputs) { input_types = ninputs; }
	void set_verbose() { verbose = true; }
	void set_array_load_sizes(vector<int> &sizes);
	void results_to_csv();
	void test_functions();
	vector<string> load_labels = {"crescente", "decrescente", "aleatório"};
};

void Benchmark::copy_arrays(int *src, int *dest, size_t arr_size)
{
	for (int i = 0; i < (int)arr_size; i++)
	{
		dest[i] = src[i];
	}
}

void Benchmark::test_functions()
{
	generate_load();

	for (int f = 0; f < (int)functions.size(); f++)
	{
		if (verbose)
			cout << "\n-------------------------------------------------- " << function_names[f] << " --------------------------------------------------\n";

		for (int l = 0; l < (int)loads.size(); l++)
		{
			for (int o = 0; o < input_types; o++)
			{
				loginfo_t loginfo;

				size_t curr_size = loads[l][o].second;

				int *arr = new int[curr_size];
				copy_arrays(loads[l][o].first, arr, curr_size);

				if (verbose)
				{
					cout << "\n\nUNORDERED - " << load_labels[o] << "\n";
					print_array(arr, curr_size);
				}

				// for each array size, there is 3 types (i.e. inc, dec, random)
				auto start = chrono::steady_clock::now(); // start clock

				functions[f](arr, curr_size, &loginfo);

				auto finish = chrono::steady_clock::now(); // enc clock

				if (verbose)
				{
					cout << "\n\nORDERED\n";
					print_array(arr, curr_size);
				}

				chrono::duration<double> elapsed = finish - start;
				loginfo.elapsed_time = elapsed.count();

				results[f][l][o] = loginfo;
				delete[] arr;
			}
		}
	}

	deallocate_load();
}

//default order is: increasing, decreasing and random
void Benchmark::generate_load()
{
	mt19937 rng;									// Gerador de números aleatórios do tipo Mersenne Twister Random Generator
	rng.seed(0);									// inicializa semente de geração de números aleatórios
	uniform_int_distribution<> distrib(0, INT_MAX); // cria gerador com distribuição uniforme entre 0 e MAX_INT

	for (int s = 0; s < (int)array_sizes.size(); s++)
	{

		loads.emplace_back();
		int curr_size = array_sizes[s];

		//increasing
		int *increasing_arr = new int[curr_size];
		for (int i = 0; i < curr_size; i++)
			increasing_arr[i] = i;

		//decreasing
		int *decreasing_arr = new int[curr_size];
		for (int i = 0; i < curr_size; i++)
			decreasing_arr[i] = curr_size - i - 1;

		//random
		int *random_arr = new int[curr_size];
		for (int i = 0; i < curr_size; i++)
			random_arr[i] = distrib(rng);

		loads[s].push_back({increasing_arr, (size_t)curr_size});
		loads[s].push_back({decreasing_arr, (size_t)curr_size});
		loads[s].push_back({random_arr, (size_t)curr_size});
	}
}

void Benchmark::deallocate_load()
{
	for (int i = 0; i < (int)loads.size(); i++)
	{
		for (int j = 0; j < input_types; j++)
		{
			delete[] loads[i][j].first;
		}
	}
	loads.clear();
}

void Benchmark::allocate_results(int size_functions)
{
	results = new loginfo_t **[size_functions];
	for (int i = 0; i < size_functions; i++)
	{
		results[i] = new loginfo_t *[(int)array_sizes.size()];

		for (int j = 0; j < (int)array_sizes.size(); j++)
		{
			results[i][j] = new loginfo_t[input_types]; // diferentes tipos de array (increasing, random, etc)
		}
	}
}

void Benchmark::deallocate_results()
{
	for (int i = 0; i < (int)functions.size(); i++)
	{
		for (int j = 0; j < (int)array_sizes.size(); j++)
			delete[] results[i][j];
		delete[] results[i];
	}
	delete[] results;
}

void Benchmark::results_to_csv()
{
	ofstream file;
	file.open("results.csv");

	string quote = "\"";

	file << "\"algoritmo\",\"tipo\",\"quantidade\",\"trocas\",\"comparacoes\",\"tempo\"\n";

	for (int tipo = 0; tipo < input_types; tipo++)
	{
		for (int quant = 0; quant < (int)array_sizes.size(); quant++)
		{
			for (int algo = 0; algo < (int)function_names.size(); algo++)
			{
				file << quote << function_names[algo] << quote << ","	 //algo
					 << quote << load_labels[tipo] << quote << ","		 //tipo
					 << array_sizes[quant] << ","						 //quantidade
					 << results[algo][quant][tipo].trocas << ","		 //trocas
					 << results[algo][quant][tipo].comparacoes << ","	 //comparacoes
					 << results[algo][quant][tipo].elapsed_time << "\n"; //tempo
			}
		}
	}

	file.close();
}

void Benchmark::print_array(int *arr, size_t array_size)
{
	for (int i = 0; i < (int)array_size; i++)
	{
		cout << arr[i] << " \n"[i == (int)array_size];
	}
}

void Benchmark::set_array_load_sizes(vector<int> &sizes)
{
	array_sizes.clear();
	for (auto it : sizes)
	{
		array_sizes.push_back(it);
	}
}