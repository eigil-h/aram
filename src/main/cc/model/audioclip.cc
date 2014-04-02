/*
	Warsaw, the audio recorder and music composer
	Copyright (C) 2014  Eigil Hysvær

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

#include "audioclip.h"
#include "../service/database.h"

warsaw::model::AudioClip::AudioClip() {
}

warsaw::model::AudioClip::AudioClip(const string& n) : id_(warsaw::service::Database::generateId()), name_(n) {
}

warsaw::model::AudioClip::~AudioClip() {
}

const string& warsaw::model::AudioClip::id() const {
	return id_;
}

const string& warsaw::model::AudioClip::name() const {
	return name_;
}

void warsaw::model::AudioClip::name(const string& name) {
	name_ = name;
}
