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

#include "buffers.h"
#include <iostream>
#include <exception>

/*
 *  warsaw::service::double_buffer
 */

warsaw::service::DoubleBuffer::DoubleBuffer(int buffer_size) : b_idx(0), bbuf_ready(true) {
	dbuf[0] = new buf_t(buffer_size);
	dbuf[1] = new buf_t(buffer_size);
	swap(); //Initialize the iterators!
}

warsaw::service::DoubleBuffer::~DoubleBuffer() {
	delete dbuf[1];
	delete dbuf[0];
}

warsaw::service::buf_t* warsaw::service::DoubleBuffer::frontBuffer() {
	return dbuf[b_idx];
}

warsaw::service::buf_t* warsaw::service::DoubleBuffer::backBuffer() {
	return dbuf[(b_idx + 1)&1];
}

void warsaw::service::DoubleBuffer::swap() {
	lock_guard<mutex> guard(swap_safe);
	++b_idx &= 1;

	fbuf_itr = frontBuffer()->begin();
	bbuf_ready = true;
}

/*
 *  warsaw::service::load_and_read_buffer
 */
warsaw::service::LoadAndReadBuffer::LoadAndReadBuffer(int siz) : DoubleBuffer(siz) {
}

bool warsaw::service::LoadAndReadBuffer::readFrontBuffer(warsaw::service::sample_t* to_buf, int len) {
	bool did_swap;
	buf_itr_t end_itr = frontBuffer()->end();
	if (fbuf_itr + len < end_itr) {
		copy(fbuf_itr, fbuf_itr + len, to_buf);
		fbuf_itr += len;
		did_swap = false;
	} else {
		int cutlen = end_itr - fbuf_itr;
		int remaining = len - cutlen;
		copy(fbuf_itr, end_itr, to_buf);
		swap();
		copy(fbuf_itr, fbuf_itr + remaining, &to_buf[cutlen]);
		fbuf_itr += remaining;
		did_swap = true;
	}
	return did_swap;
}

int warsaw::service::LoadAndReadBuffer::loadBackBuffer(istream& istr) {
	if (!bbuf_ready) {
		return 0;
	}
	buf_itr_t end_itr;
	buf_itr_t bbuf_itr;
	{
		lock_guard<mutex> guard(swap_safe);
		end_itr = backBuffer()->end();
		bbuf_itr = backBuffer()->begin();
	}
	int stream_pos = istr.tellg();
	istr.seekg(0, ios::end);
	int stream_end = istr.tellg();
	istr.seekg(stream_pos, ios::beg);

	int len = end_itr - bbuf_itr;
	if (len > (stream_end - stream_pos) / sizeof (sample_t)) {
		len = (stream_end - stream_pos) / sizeof (sample_t);
	}

	istr.read(reinterpret_cast<char*> (&(*bbuf_itr)), len * sizeof (sample_t));
	bbuf_itr += len;
	int fillsize = end_itr - bbuf_itr;
	bbuf_itr = fill_n(bbuf_itr, fillsize, 0.f);

	if (bbuf_itr != end_itr) {
		throw runtime_error("bug in write_back_buffer!");
	}

	bbuf_ready = false;
	return len;
}

int warsaw::service::LoadAndReadBuffer::loadBackBufferAndSwap(istream& istr) {
	int samples_read = loadBackBuffer(istr);
	swap();
	return samples_read;
}

/*
 *  warsaw::service::write_and_store_buffer
 */
warsaw::service::WriteAndStoreBuffer::WriteAndStoreBuffer(int siz) : DoubleBuffer(siz) {
}

bool warsaw::service::WriteAndStoreBuffer::writeFrontBuffer(sample_t* from_buf, int len) {
	buf_itr_t itr;
	{
		lock_guard<mutex> guard(swap_safe);
		if (fbuf_itr + len < frontBuffer()->end()) {
			itr = fbuf_itr;
			fbuf_itr += len;
		} else {
			return false;
		}
	}

	copy(from_buf, &from_buf[len], &(*itr));

	itr += len;

	return true;
}

int warsaw::service::WriteAndStoreBuffer::swapAndStoreBackBuffer(ostream& ostr) {
	int len = fbuf_itr - frontBuffer()->begin();
	swap();

	ostr.write(reinterpret_cast<char*> (&(*backBuffer()->begin())), len * sizeof (sample_t));

	return len;
}



/*
 *  warsaw::service::single_buffer
 */

warsaw::service::SingleBuffer::SingleBuffer(istream& istr) {
	istr.seekg(0, ios::end);
	int len = istr.tellg() / sizeof (sample_t);
	istr.seekg(0, ios::beg);

	buf = new buf_t(len);
	reset();
	istr.read(reinterpret_cast<char*> (&(*buf_itr)), len * sizeof (sample_t));
}

void warsaw::service::SingleBuffer::read(sample_t* to, int len) {
	if (buf_itr + len > buf->end()) {
		throw "out of bounds"; //todo maybe better to fill w zeros?
	}
	copy(buf_itr, buf_itr + len, to);
	buf_itr += len;
}

void warsaw::service::SingleBuffer::reset() {
	buf_itr = buf->begin();
}
