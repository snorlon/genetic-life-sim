/*
 * FileReader.h
 *
 *  Created on: Jan 3, 2018
 *      Author: root
 */

#ifndef SUPPORT_FILEREADER_H_
#define SUPPORT_FILEREADER_H_

#include <string>
#include <fstream>
#include "Config.h"
#include "Parameters.h"

using namespace std;

class FileReader {
public:
	FileReader();
	virtual ~FileReader();

	void readParameters(Parameters* parameters, string filename = "files/parameters.config");
	void readConfig(Config* config);
};

#endif /* SUPPORT_FILEREADER_H_ */
