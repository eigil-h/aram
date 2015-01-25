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

#include "buffers.h"
#include <iostream>
#include <exception>
#include <algorithm>

/*
 *  aram::service::double_buffer
 */

aram::service::DoubleBuffer::DoubleBuffer(int buffer_size) : b_idx(0), bbuf_ready(true) {
	dbuf[0] = new buf_t(buffer_size);
	dbuf[1] = new buf_t(buffer_size);
	swap(); //Initialize the iterators!
}

aram::service::DoubleBuffer::~DoubleBuffer() {
	delete dbuf[1];
	delete dbuf[0];
}

aram::service::buf_t* aram::service::DoubleBuffer::frontBuffer() {
	return dbuf[b_idx];
}

aram::service::buf_t* aram::service::DoubleBuffer::backBuffer() {
	return dbuf[(b_idx + 1)&1];
}

void aram::service::DoubleBuffer::swap() {
	lock_guard<mutex> guard(swap_safe);
	++b_idx &= 1;

	fbuf_itr = frontBuffer()->begin();
	bbuf_ready = true;
}

/*
 *  aram::service::load_and_read_buffer
 */
aram::service::LoadAndReadBuffer::LoadAndReadBuffer(int siz) : DoubleBuffer(siz) {
}

bool aram::service::LoadAndReadBuffer::readFrontBuffer(aram::service::sample_t* to_buf, int len) {
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

int aram::service::LoadAndReadBuffer::loadBackBuffer(forward_list<istream*>& istr_list) {
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

	int total_len = 0;
	int fillsize = end_itr - bbuf_itr;

	while(!istr_list.empty()) {
		istream& istr = *istr_list.front();

		int stream_pos = istr.tellg();
		istr.seekg(0, ios::end);
		int stream_end = istr.tellg();
		istr.seekg(stream_pos, ios::beg);

		int len = end_itr - bbuf_itr;
		if (len > (stream_end - stream_pos) / sizeof (sample_t)) {
			len = (stream_end - stream_pos) / sizeof (sample_t);
		}
		total_len += len;

		istr.read(reinterpret_cast<char*> (&(*bbuf_itr)), len * sizeof (sample_t));
		bbuf_itr += len;

		fillsize = end_itr - bbuf_itr;
		if(fillsize == 0) {
			//the buffer is filled
			break;
		}
		//We're done with this stream
		istr_list.pop_front();
	}

	bbuf_itr = fill_n(bbuf_itr, fillsize, 0.f);

	if (bbuf_itr != end_itr) {
		throw runtime_error("bug in loadBackBuffer!");
	}

	bbuf_ready = false;

	return total_len;
}

int aram::service::LoadAndReadBuffer::loadBackBufferAndSwap(forward_list<istream*>& istr) {
	int samples_read = loadBackBuffer(istr);
	swap();
	return samples_read;
}

/*
 *  aram::service::write_and_store_buffer
 */
aram::service::WriteAndStoreBuffer::WriteAndStoreBuffer(int siz) : DoubleBuffer(siz) {
}

bool aram::service::WriteAndStoreBuffer::writeFrontBuffer(sample_t* from_buf, int len) {
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

int aram::service::WriteAndStoreBuffer::swapAndStoreBackBuffer(ostream& ostr) {
	int len = fbuf_itr - frontBuffer()->begin();
	swap();

	ostr.write(reinterpret_cast<char*> (&(*backBuffer()->begin())), len * sizeof (sample_t));

	return len;
}

/*
 *  aram::service::single_buffer
 */

aram::service::SingleBuffer::SingleBuffer(istream& istr) {
	istr.seekg(0, ios::end);
	int len = istr.tellg() / sizeof (sample_t);
	istr.seekg(0, ios::beg);

	buf = new buf_t(len);
	reset();
	istr.read(reinterpret_cast<char*> (&(*buf_itr)), len * sizeof (sample_t));
}

void aram::service::SingleBuffer::read(sample_t* to, int len) {
	if (buf_itr + len > buf->end()) {
		throw "out of bounds"; //todo maybe better to fill w zeros?
	}
	copy(buf_itr, buf_itr + len, to);
	buf_itr += len;
}

void aram::service::SingleBuffer::reset() {
	buf_itr = buf->begin();
}
