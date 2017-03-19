#include "prefnet.hxx"
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
using namespace std;

//#define INF ULLONG_MAX
#define INF INT_MAX

string zerobasedOutcome(string onebasedOutcome);

PrefNet::PrefNet(CPNet net)
{
//printf("PrefNet constructor: %p\n", net.cpnet);
//net.displayCPNet();
//delete net.cpnet;
//exit(1);
	this->net = net;
//printf("PrefNet constructor: %p\n", this->net.cpnet);
	setNumNodes();

	//adj = new vull[num_nodes];
	adj = new vull[10];  // not really use it
	setAdj();
	//updateAdjToTransClosure();
}

PrefNet::~PrefNet()
{
	delete[] adj;
}

void PrefNet::setNumNodes()
{
	ull mult = 1;
	for (int i=0; i<net.vars_domains.size(); i++) {
		mult *= net.vars_domains.at(i).second.size();
	}
	num_nodes = mult;
}

ull PrefNet::getNumNodes()
{
	return num_nodes;
}

void PrefNet::setAdj()
{
	int num_vars = net.vars_domains.size();
	int num_edges = 0;
	
  cpnet_node* walk = net.cpnet;
  while (walk->next) {
    // the variable name
		string var_name = (walk->variable_name).substr(1);
		//cout << "For variable " << var_name << ":" << endl;
   
    // the preference
		int num_edges_from_cpt = 0;
		int max_num_edges_per_rule = MAX_NUM_GENERATED_EXAMPLES_PER_TABLE / walk->cpt.size();
    for (vpVsS::iterator i=walk->cpt.begin(); i!=walk->cpt.end(); i++) {
			int num_edges_from_rule = 0;
			vpss conditions;
      for (int j=0; j<(*i).first.size(); j++) {
				string condition = (*i).first.at(j);
				string parent_var = condition.substr(1, condition.find('=')-1);
				string parent_val = condition.substr(condition.find('=')+1);
				conditions.push_back(make_pair(parent_var,parent_val));
      }
			//for (int n=0; n<conditions.size(); n++) {
			//	cout << conditions[n].first << "(" << conditions[n].second << ") ";
			//}
			string pref = (*i).second;
			string more_pref = pref.substr(0,pref.find(':'));
			string less_pref = pref.substr(pref.find(':')+1);
			//cout << " : " << more_pref << ">" << less_pref << endl;

			addEdgesForPrefRule(var_name, conditions, pref, &num_edges_from_rule, &num_edges, max_num_edges_per_rule);
    }
    //cout << "==========================" << endl;
    
    walk = walk->next;
  }
}

void PrefNet::addEdgesForPrefRule(string var_name, vpss conditions, string pref, int* num_edges_from_rule, 
																	int* num_edges, int max_num_edges_per_rule)
{
	addEdgesForPrefRule_Recur(0, var_name, conditions, pref, "", "", num_edges_from_rule, num_edges, max_num_edges_per_rule);
}

void PrefNet::addEdgesForPrefRule_Recur(int pos, string var_name, vpss conditions, string pref, 
																				string first_outcome, string second_outcome, int* num_edges_from_rule, 
																				int* num_edges, int max_num_edges_per_rule)
{
	if (*num_edges_from_rule == max_num_edges_per_rule) {return;}

	if (pos == net.vars_domains.size()) {
		//cout << first_outcome << ">" << second_outcome << endl;
		outcomes.insert({first_outcome, *num_edges*2+1});
		outcomes.insert({second_outcome, *num_edges*2+2});
		//ull f_outcome = stoull(zerobasedOutcome(first_outcome), NULL, net.vars_domains[0].second.size());
		//ull s_outcome = stoull(zerobasedOutcome(second_outcome), NULL, net.vars_domains[0].second.size());
		//adj[f_outcome].push_back(s_outcome);
		//edges.push_back({f_outcome,s_outcome});
		edges.push_back({outcomes[first_outcome],outcomes[second_outcome]});
		(*num_edges_from_rule)++;
		(*num_edges)++;
		return;
	}

	// check if pos is the position of a parent node
	bool isParentPos = false;
	int parentInd = -1;
	for (int i=0; i<conditions.size(); i++) {
		if (net.vars_domains[pos].first.substr(1) == conditions[i].first) {
			isParentPos = true;
			parentInd = i;
		}
	}

	if (net.vars_domains[pos].first.substr(1) == var_name) {  // for the var that pref concerns
		addEdgesForPrefRule_Recur(pos+1, var_name, conditions, pref,
			first_outcome+pref.substr(0,pref.find(':')),
			second_outcome+pref.substr(pref.find(':')+1), num_edges_from_rule, num_edges, max_num_edges_per_rule);
	} else if (isParentPos) {  // for the parent vars
		addEdgesForPrefRule_Recur(pos+1, var_name, conditions, pref,
			first_outcome+conditions[parentInd].second,
			second_outcome+conditions[parentInd].second, num_edges_from_rule, num_edges, max_num_edges_per_rule);
	} else {  // for every other var
		for (int i=0; i<net.vars_domains[pos].second.size(); i++) {  // for all values in domain of the pos-th var
			addEdgesForPrefRule_Recur(pos+1, var_name, conditions, pref, 
				first_outcome+to_string(net.vars_domains[pos].second.at(i)), 
				second_outcome+to_string(net.vars_domains[pos].second.at(i)), num_edges_from_rule, num_edges, max_num_edges_per_rule);
		}
	}
}

void PrefNet::printEdges()
{
/*
	for (ull i=0; i<num_nodes; i++) {
		for (ull j=0; j<adj[i].size(); j++) {
			cout << "(" << i << "," << adj[i][j] << ")" << endl;
		}
	}
*/
	for (ull i=0; i<edges.size(); i++) {
		cout << "(" << edges[i].first << "," << edges[i].second << ")" << endl;
	}
}

ull PrefNet::getNumEdges()
{
/*
	int num = 0;
	for (ull i=0; i<num_nodes; i++) {
		num += adj[i].size();
	}
	return num;
*/
	return edges.size();
}

string zerobasedOutcome(string onebasedOutcome)
{
	string res = onebasedOutcome;
	for (int i=0; i<res.size(); i++) {
		res[i] -= 1;
	}
	return res;
}

void PrefNet::updateAdjToTransClosure()
{
#if 0
	// this is essentially Floyd-Warshall
	//ull dist[num_nodes][num_nodes];  /* use dynamic allocation instead */
	ull** dist = new ull*[num_nodes];
	for (ull i=0; i<num_nodes; i++) {
		dist[i] = new ull[num_nodes];
	}

	for (ull i=0; i<num_nodes; i++) {
		for (ull j=0; j<num_nodes; j++) {
			if (i == j) {dist[i][j] = 0;}
			else {dist[i][j] = INF;}
		}
	}
	for (ull i=0; i<num_nodes; i++) {
		for (ull j=0; j<adj[i].size(); j++) {
			dist[i][adj[i][j]] = 1;
		}
	}

	for (ull k=0; k<num_nodes; k++) {
		for (ull i=0; i<num_nodes; i++) {
			for (ull j=0; j<num_nodes; j++) {
				if (dist[i][k]!=INF && dist[k][j]!=INF && dist[i][k]+dist[k][j] < dist[i][j]) {
					dist[i][j] = dist[i][k]+dist[k][j];
				}
			}
		}
	}

	// reset adj so that it contains all edges indicated in dist.
	for (ull i=0; i<num_nodes; i++) {
		adj[i].clear();
	}
	for (ull i=0; i<num_nodes; i++) {
		for (ull j=0; j<num_nodes; j++) {
			if (dist[i][j] != INF && dist[i][j] != 0) {
				adj[i].push_back(j);
			}
		}
	}

	for (ull i=0; i<num_nodes; i++) {
		delete[] dist[i];
	}
	delete[] dist;
#endif
}

void PrefNet::produceDataOverviewFile(string dir)
{
	string xml_file_name = net.cpnet_xml_file;
	size_t found = xml_file_name.find_last_of(".");
	if (found != string::npos) {
		xml_file_name.erase(xml_file_name.begin()+found, xml_file_name.end());
	}
	string actual_dir = dir + "/" + xml_file_name + "/";
	string comm = "mkdir -p " + actual_dir;
	system(comm.c_str());

	FILE* f = fopen((actual_dir+"data_overview.txt").c_str(), "w");
	if (!f) {
		cerr << "Error opening file " << actual_dir+"data_overview.txt" << endl;
		exit(1);
	}

	fprintf(f, "#attributes:%d\n", net.vars_domains.size());
	//fprintf(f, "#outcomes:%llu\n", num_nodes);
	fprintf(f, "#outcomes:%llu\n", outcomes.size());
	fprintf(f, "#users:1\n");
	fprintf(f, "#strictExamples:%llu\n", getNumEdges());
	fprintf(f, "#eqExamples:0\n");

	if (fclose(f) == EOF) {
		cerr << "Error closing file " << actual_dir+"data_overview.txt" << endl;
		exit(1);
	}
}

void PrefNet::produceDomainDescriptionFile(string dir)
{
	string xml_file_name = net.cpnet_xml_file;
	size_t found = xml_file_name.find_last_of(".");
	if (found != string::npos) {
		xml_file_name.erase(xml_file_name.begin()+found, xml_file_name.end());
	}
	string actual_dir = dir + "/" + xml_file_name + "/";
	string comm = "mkdir -p " + actual_dir;
	system(comm.c_str());

	FILE* f = fopen((actual_dir+"domain_description.txt").c_str(), "w");
	if (!f) {
		cerr << "Error opening file " << actual_dir+"domain_description.txt" << endl;
		exit(1);
	}

	for (int i=0; i<net.vars_domains.size(); i++) {
		fprintf(f, "%s:", net.vars_domains[i].first.c_str());
		for (int j=0; j<net.vars_domains[i].second.size(); j++) {
			if (j == net.vars_domains[i].second.size()-1) {
				fprintf(f, "%d", net.vars_domains[i].second[j]);
			} else {
				fprintf(f, "%d,", net.vars_domains[i].second[j]);
			}
		}
		fprintf(f, "\n");
	}

	if (fclose(f) == EOF) {
		cerr << "Error closing file " << actual_dir+"domain_description.txt" << endl;
		exit(1);
	}
}

void PrefNet::produceOutcomesFile(string dir)
{
	string xml_file_name = net.cpnet_xml_file;
	size_t found = xml_file_name.find_last_of(".");
	if (found != string::npos) {
		xml_file_name.erase(xml_file_name.begin()+found, xml_file_name.end());
	}
	string actual_dir = dir + "/" + xml_file_name + "/";
	string comm = "mkdir -p " + actual_dir;
	system(comm.c_str());

	FILE* f = fopen((actual_dir+"outcomes.csv").c_str(), "w");
	if (!f) {
		cerr << "Error opening file " << actual_dir+"outcomes.csv" << endl;
		exit(1);
	}

	/* Output title */
	fprintf(f, "OutcomeID,");
	for (int i=0; i<net.vars_domains.size(); i++) {
		if (i == net.vars_domains.size()-1) {
			fprintf(f, "%s", net.vars_domains[i].first.c_str());
		} else {
			fprintf(f, "%s,", net.vars_domains[i].first.c_str());
		}
	}
	fprintf(f, "\n");

	/* output outcomes */
	//int count = 0;
	//genAllOutcomesToFile(f, 0, "", &count);
	for (umsull::iterator it=outcomes.begin(); it!=outcomes.end(); it++) {
		//fprintf(f, "%llu", stoull(zerobasedOutcome(it->first), NULL, net.vars_domains[0].second.size())+1);
		fprintf(f, "%llu", it->second);
		for (int i=0; i<(it->first).size(); i++) {
			fprintf(f, ",%c", (it->first)[i]);
		}
		fprintf(f, "\n");
	}

	if (fclose(f) == EOF) {
		cerr << "Error closing file " << actual_dir+"outcomes.csv" << endl;
		exit(1);
	}
}

void PrefNet::genAllOutcomesToFile(FILE* f, int pos, string outcome, int* count)
{
	if (pos == net.vars_domains.size()) {
		fprintf(f, "%d%s\n", *count+1, outcome.c_str());
		(*count)++;
		return;
	}

	for (int i=0; i<net.vars_domains[pos].second.size(); i++) {
		genAllOutcomesToFile(f, pos+1, outcome+","+to_string(net.vars_domains[pos].second[i]), count);
	}
}

void PrefNet::produceStrictExamplesFile(string dir)
{
	string xml_file_name = net.cpnet_xml_file;
	size_t found = xml_file_name.find_last_of(".");
	if (found != string::npos) {
		xml_file_name.erase(xml_file_name.begin()+found, xml_file_name.end());
	}
	string actual_dir = dir + "/" + xml_file_name + "/";
	string comm = "mkdir -p " + actual_dir;
	system(comm.c_str());

	FILE* f = fopen((actual_dir+"strict_examples.csv").c_str(), "w");
	if (!f) {
		cerr << "Error opening file " << actual_dir+"strict_examples.csv" << endl;
		exit(1);
	}

	fprintf(f, "UserID,Outcome1ID,Outcome2ID\n");
/*
	for (ull i=0; i<num_nodes; i++) {
		for (ull j=0; j<adj[i].size(); j++) {
			fprintf(f, "1,%llu,%llu\n", i+1, adj[i][j]+1);
		}
	}
*/
	for (ull i=0; i<edges.size(); i++) {
		fprintf(f, "1,%llu,%llu\n", edges[i].first, edges[i].second);
	}

	if (fclose(f) == EOF) {
		cerr << "Error closing file " << actual_dir+"strict_examples.csv" << endl;
		exit(1);
	}
}



