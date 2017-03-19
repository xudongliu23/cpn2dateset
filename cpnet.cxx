#include "cpnet.hxx"
#include "cpnet_instance.hxx"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  /* for memcpy */

CPNet::CPNet()
{
	cpnet = new cpnet_node();
}

CPNet::CPNet(const CPNet& other)
{
	cpnet_xml_dir = other.cpnet_xml_dir;
	cpnet_xml_file = other.cpnet_xml_file;
	vars_domains = other.vars_domains;

	cpnet_node* other_walk = other.cpnet;
	cpnet = new cpnet_node();
//printf("CPNet copy constructor: %p\n", cpnet);
//delete cpnet;
//exit(1);
	memcpy(cpnet, other_walk, sizeof(cpnet_node));
	cpnet_node* this_walk = cpnet;
	cpnet_node* this_prev = cpnet;
	other_walk = other_walk->next;
	while (other_walk) {
		cpnet_node* walk = new cpnet_node();
		this_walk = walk;
		memcpy(this_walk, other_walk, sizeof(cpnet_node));
		this_prev->next = this_walk;
		this_prev = this_walk;

		other_walk = other_walk->next;
	}
//printf("%p\n", cpnet);
//displayCPNet();
}

CPNet& CPNet::operator=(const CPNet& other)
{
	if (this != &other) {
		cpnet_xml_dir = other.cpnet_xml_dir;
		cpnet_xml_file = other.cpnet_xml_file;
		vars_domains = other.vars_domains;

		cpnet_node* other_walk = other.cpnet;
		cpnet_node* this_walk = cpnet;
		while (other_walk) {
			memcpy(this_walk, other_walk, sizeof(cpnet_node));

			other_walk = other_walk->next;
			this_walk = this_walk->next;
		}
	}

	return *this;
}

CPNet::~CPNet()
{
//cout << "Shit" << endl;
//	deleteNodes();
//cout << "Fuck" << endl;
}

int CPNet::buildCPNetFromXML(const string cpnet_xml_path)
{
	size_t found = cpnet_xml_path.find_last_of("/");
	if (found == string::npos) {
		cpnet_xml_dir = "";
		cpnet_xml_file = cpnet_xml_path;
	} else {
		cpnet_xml_dir = cpnet_xml_path.substr(0, found+1);;
		cpnet_xml_file = cpnet_xml_path.substr(found+1);;
	}

	try {
		auto_ptr<cpn_t> cpn (PREFERENCE_SPECIFICATION (cpnet_xml_path));

	  // Iterate over preference variable records.
	  //
	  cpn_t::PREFERENCE_VARIABLE_sequence& pvs (cpn->PREFERENCE_VARIABLE ());
	
	  for (cpn_t::PREFERENCE_VARIABLE_iterator i (pvs.begin ()); i != pvs.end (); ++i) {
	    pref_var_t& pv (*i);
	
	    //cout << "variable name:   " << pv.VARIABLE_NAME () << endl;
	
			vi domains;
	    for (pref_var_t::DOMAIN_VALUE_const_iterator ii (pv.DOMAIN_VALUE().begin()); ii != pv.DOMAIN_VALUE().end(); ++ii) {
	      //cout << "domain value:   " << *ii << endl;
				domains.push_back(*ii);
	    }
			vars_domains.push_back(make_pair(pv.VARIABLE_NAME(), domains));
	  }
	
		// Iterate over preference statement records.
		//
		cpnet_node* head = cpnet;
		cpn_t::PREFERENCE_STATEMENT_sequence& pss (cpn->PREFERENCE_STATEMENT ());
		string prev_var_name = "";
	
		for (cpn_t::PREFERENCE_STATEMENT_iterator i (pss.begin ()); i != pss.end (); ++i) {
		  pref_st_t& ps (*i);

			//cout << "variable name:   " << ps.PREFERENCE_VARIABLE () << endl;
			head->variable_name = ps.PREFERENCE_VARIABLE();

			vs conditions;
			for (pref_st_t::CONDITION_const_iterator ii (ps.CONDITION().begin()); ii != ps.CONDITION().end(); ++ii) {
				//cout << "condition:   " << *ii << endl;
				size_t pos = (*ii).find("=");
				head->par_names.push_back((*ii).substr(0, pos));
				conditions.push_back(*ii);
			}

			for (pref_st_t::PREFERENCE_const_iterator ii (ps.PREFERENCE().begin()); ii != ps.PREFERENCE().end(); ++ii) {
				//cout << "preference:   " << *ii << endl;
				head->cpt.push_back(make_pair(conditions, *ii));
			}

			cpnet_node* node;
			if (ps.PREFERENCE_VARIABLE() != prev_var_name) {
				node = new cpnet_node();
				node->next = NULL;
				head->next = node;
				prev_var_name = ps.PREFERENCE_VARIABLE();
			}

			// move head
			cpn_t::PREFERENCE_STATEMENT_iterator i_next = i;
			i_next++;
			if (i_next != pss.end() && (*i_next).PREFERENCE_VARIABLE() != ps.PREFERENCE_VARIABLE()) {
				head = node;
			}
		}
	} catch(const xml_schema::exception& e) {
		cerr << e << endl;
		return 1;
	}

	return 0;
}

void CPNet::displayCPNet()
{
  cpnet_node* walk = cpnet;
  while (walk->next) {
    // the variable name
    cout << walk->variable_name << endl;

    // the parental variables' names, if any
    //for (vs::iterator i=walk->par_names.begin(); i!=walk->par_names.end(); i++) {
    //  cout << *i << " ";
    //}
    //cout << endl;
  
    // the preference
    for (vpVsS::iterator i=walk->cpt.begin(); i!=walk->cpt.end(); i++) {
      for (int j=0; j<(*i).first.size(); j++) {
        cout << (*i).first.at(j) << " ";
      }
      cout << "-> " << (*i).second << endl;
    }
    cout << "==========================" << endl;
    
    walk = walk->next;
  }
}

void CPNet::deleteNodes()
{
//printf("CPNet deleteNodes: %p\n", cpnet);
	cpnet_node* cur = cpnet;
	cpnet_node* next;

	while (cur) {
		next = cur->next;
		delete cur;
		cur = next;
	}
	cpnet = NULL;
}
