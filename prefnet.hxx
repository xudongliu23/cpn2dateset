#ifndef PREFNET_H
#define PREFNET_H

#include "cpnet.hxx"
#include <vector>
#include <set>
#include <string>
#include <utility>
#include <unordered_map>

#define MAX_NUM_GENERATED_EXAMPLES_PER_TABLE 400

typedef unsigned long long           ull;
typedef vector<ull>                  vull;
typedef pair<string,string>          pss;
typedef vector<pss>                  vpss;
typedef set<string>                  ss;
typedef unordered_map<string,ull>    umsull;
typedef pair<ull,ull>                puu;
typedef vector<puu>                  vpuu;

class PrefNet {

public:
	CPNet net;
	ull   num_nodes;  // number of outcomes
	vull* adj;
	umsull  outcomes;   // outcomes that show up in edges
	vpuu  edges;

public:
	PrefNet(CPNet net);
	~PrefNet();

	void setNumNodes();
	ull getNumNodes();
	void setAdj();
	void updateAdjToTransClosure();
	void printEdges();
	ull getNumEdges();

	void addEdgesForPrefRule(string var_name, vpss conditions, string pref, int* num_edges_from_rule, 
																	int* num_edges, int max_num_edges_per_rule);
	void addEdgesForPrefRule_Recur(int pos, string var_name, vpss conditions, string pref, 
															 string first_outcome, string second_outcome, int* num_edges_from_cpt, int* num_edges);
	void addEdgesForPrefRule_Recur(int pos, string var_name, vpss conditions, string pref, 
																				string first_outcome, string second_outcome, int* num_edges_from_rule, 
																				int* num_edges, int max_num_edges_per_rule);

	void produceDataOverviewFile(string dir);
	void produceDomainDescriptionFile(string dir);
	void produceOutcomesFile(string dir);
	void genAllOutcomesToFile(FILE* f, int pos, string outcome, int* count);
	void produceStrictExamplesFile(string dir);
};

#endif
