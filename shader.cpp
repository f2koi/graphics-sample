#include <fstream>
#include <sstream>
#include "shader.h"

using namespace std;

string load_shader_source(const string filename) {
	ifstream file(filename);
	return string(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
}