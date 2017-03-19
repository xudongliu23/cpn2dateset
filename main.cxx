#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "cpnet_instance.hxx"
#include "cpnet.hxx"
#include "prefnet.hxx"

using namespace std;

int main (int argc, char* argv[])
{
	if (argc != 3) {
		cerr << "Not enough arguments" << endl;
		return 1;
	}

	//string cpnet_xml_fullpath = "./cpnet_n3c2d3_0000.xml";
	//string cpnet_xml_fullpath = "./cpnet_n5c2d3_0000.xml";
	string cpnet_xml_fullpath(argv[1]);
	string dataset_dir_path(argv[2]);

	CPNet net;
	if (net.buildCPNetFromXML(cpnet_xml_fullpath)) {
		cerr << "ERROR" << endl;
		exit(1);
	}
	//cout << net.cpnet_xml_dir << endl;
	//cout << net.cpnet_xml_file << endl;
	//cout << "CP net:" << endl;
	//net.displayCPNet();

	PrefNet pNet(net);
	//pNet.printEdges();
	//cout << "There are " << pNet.getNumNodes() << " nodes in the pref net." << endl;
	//cout << "There are " << pNet.getNumEdges() << " edges in the pref net." << endl;
	pNet.produceDataOverviewFile(dataset_dir_path);
	pNet.produceDomainDescriptionFile(dataset_dir_path);
	pNet.produceOutcomesFile(dataset_dir_path);
	pNet.produceStrictExamplesFile(dataset_dir_path);

	return 0;
}
