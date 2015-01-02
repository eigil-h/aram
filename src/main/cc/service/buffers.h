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

#ifndef ARAM_BUFFERS_H
#define ARAM_BUFFERS_H
#include <array>
#include <vector>
#include <mutex>

using namespace std;

namespace aram {
	namespace service {
		typedef float sample_t;
		typedef vector<sample_t> buf_t;
		typedef buf_t::iterator buf_itr_t;

		class DoubleBuffer {
		protected:
			array<buf_t*, 2> dbuf;
			buf_itr_t fbuf_itr;
			bool bbuf_ready;
			int b_idx;
			mutex swap_safe;

			/*xXx*xXx*
			PRIVATE API
			 *xXx*xXx*/

			buf_t* frontBuffer();
			buf_t* backBuffer();
			void swap();

			/*xXx*xXx*
			PUBLIC API
			 *xXx*xXx*/

		public:
			/**
			 * Constructor, allocate buffers.
			 * @param len Length of each buffer.
			 */
			DoubleBuffer(int siz);

			/**
			 * Free buffers.
			 */
			virtual ~DoubleBuffer();
		};

		class LoadAndReadBuffer : public DoubleBuffer {
		public:

			LoadAndReadBuffer(int siz);

			/**
			 * Copy len floats to given buffer. Swaps the front/back buffers if necessary.
			 * Note, len > buffers is not considered. This class' intention is to be used as
			 * a large buffer for some smaller real time/hardware buffers.
			 * Return true if there was a swap.
			 */
			bool readFrontBuffer(sample_t* to_buf, int len);

			/**
			 * Fill up back buffer. Excess buffer space are filled with 0.
			 * Return number of actual samples read from stream.
			 */
			int loadBackBuffer(istream& istr);

			/**
			 * Use first time so that there's data to read in the front buffer.
			 * Return number of actual samples read from stream.
			 */
			int loadBackBufferAndSwap(istream& istr);
		};

		class WriteAndStoreBuffer : public DoubleBuffer {
		public:

			WriteAndStoreBuffer(int siz);

			/**
			 * Write len number of samples to front buffer.
			 * Return true if it could write all.
			 * Note - if false it means that there are samples that never will be stored.
			 */
			bool writeFrontBuffer(sample_t* from_buf, int len);

			/**
			 * Swap the buffers and write to ostr as much as was written to it.
			 * Return number of samples stored.
			 */
			int swapAndStoreBackBuffer(ostream& ostr);
		};

		class SingleBuffer {
			buf_t* buf;
			buf_itr_t buf_itr;

		public:
			SingleBuffer(istream& istr);
			void read(sample_t* to, int len);
			void reset();
		};
	}
}
#endif
