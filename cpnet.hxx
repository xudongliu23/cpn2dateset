#ifndef CPNET_H
#define CPNET_H

#include <vector>
#include <string>
#include <utility>
#include <set>
using namespace std;

typedef vector<string>     vs;
typedef vector<int>        vi;
typedef set<string>        ss;

typedef pair<vs,string>    pVsS;
typedef vector<pVsS>      vpVsS;

typedef pair<string,vi>    pSVi;
typedef vector<pSVi>       vpSVi;

typedef struct cpnet_node {
	string         variable_name;
	vs             par_names;
	vpVsS          cpt;
	cpnet_node*    next;
} cpnet_node;

class CPNet {
public:
	string      cpnet_xml_dir;
	string      cpnet_xml_file;
	vpSVi       vars_domains;
	cpnet_node* cpnet;

public:
	CPNet();
	CPNet(const CPNet& other);
	~CPNet();
	CPNet& operator=(const CPNet& other);

	int buildCPNetFromXML(const string cpnet_xml_path);
	void displayCPNet();
	void deleteNodes();
};


#endif
