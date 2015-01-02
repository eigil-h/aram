/*
	ARAM, the audio recorder and music ninja
	Copyright (C) 2014-2015  Eigil Hysvær

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "system.h"
#include <cstdlib>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>

const string aram::System::getHomePath() {
	return ::getenv("HOME");
}

void aram::System::mkdir(const string& path) {
	if(::mkdir(path.c_str(), 0700) != 0 && errno != EEXIST) {
		throw runtime_error(::strerror(errno));
	}
}

//linux only, afaik
vector<string> aram::System::getProgramArguments() {
	vector<string> result;
	pid_t pid = getpid();
	stringstream ss;
	ss << "/proc/" << pid << "/cmdline";
	ifstream f(ss.str(), ios::binary);
	string line;
	while(getline(f, line, '\0')) {
		result.push_back(line);
	}
	return result;
}
