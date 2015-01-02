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

#ifndef ARAM_DATABASE_H
#define ARAM_DATABASE_H

#include <memory>
#include <string>
#include <odb/sqlite/database.hxx>

using namespace std;
using namespace odb::sqlite;

namespace aram {
	namespace service {

		/* Singleton class.
		 * Because there's only one database and it needs to be accessible from several places.
		 * Looks like it's the best way to go.
		 */
		class Database {
			unique_ptr<database> db_;

			Database();
			Database(Database const&);
			void operator=(Database const&);

			void createSchemaNoDrop();
			void initApplication();
			void createApplication();

		public:
			static Database& getInstance();

			/* In order to use aram:::Database just as if it was an odb:::database.
			 */
			unique_ptr<database> const& operator->();
			
			static string generateId();
		};
	}
}

#endif
