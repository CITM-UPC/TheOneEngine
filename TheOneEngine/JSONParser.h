#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "simdjson.h"

using namespace simdjson;
using namespace std;

ondemand::document GetFile(string filePath)
{
	ondemand::parser parser;
	auto json = padded_string::load(filePath); // load JSON file 'twitter.json'.
	ondemand::document doc = parser.iterate(json); // position a pointer at the beginning of the JSON data

	return doc;
}

//The simdjson library will also accept std::string instances.If the provided reference is non - const, it will allocate padding as needed.
//
//You can copy your data directly on a simdjson::padded_string as follows :
//
//const char* data = "my data"; // 7 bytes
//simdjson::padded_string my_padded_data(data, 7); // copies to a padded buffer
//Or as follows...
//
//std::string data = "my data";
//simdjson::padded_string my_padded_data(data); // copies to a padded buffer