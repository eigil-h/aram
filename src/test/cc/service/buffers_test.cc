#include "../catch.hpp"
#include "../../../main/cc/service/buffers.h"
#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>


static bool run = true;
vector<aram::service::sample_t> result;

void back_thread_func(aram::service::LoadAndReadBuffer& my_dbuf, forward_list<istream*>& ifstr) {
	while (run) {
		this_thread::sleep_for(chrono::milliseconds(200));
		my_dbuf.loadBackBuffer(ifstr);
	}
}

void front_thread_func(aram::service::LoadAndReadBuffer& my_dbuf) {
	aram::service::sample_t my_buf[3];
	while (run) {
		my_dbuf.readFrontBuffer(my_buf, 3);
		result.push_back(my_buf[0]);
		result.push_back(my_buf[1]);
		result.push_back(my_buf[2]);
		this_thread::sleep_for(chrono::milliseconds(50));
	}
}

static void make_file(aram::service::sample_t max) {
	ofstream of("/tmp/tmpdata", ios::out | ios::binary | ios::trunc);
	if (of.is_open()) {
		for (aram::service::sample_t i = 0.f; i < max; i += 1.f) {
			of.write(reinterpret_cast<char*> (&i), sizeof (aram::service::sample_t));
		}
		of.close();
	} else {
		throw exception();
	}
}

TEST_CASE("double_buffer/single_thread", "Should behave as predicted in a single thread run.") {
	make_file(24.f);
	ifstream ifstr("/tmp/tmpdata", ios::in | ios::binary);
	forward_list<istream*> istr_list(1, &ifstr);

	aram::service::LoadAndReadBuffer my_dbuf(10);
	aram::service::sample_t my_buf[3];

	REQUIRE(my_dbuf.loadBackBufferAndSwap(istr_list) == 10);
	REQUIRE(my_dbuf.loadBackBuffer(istr_list) == 10);
	REQUIRE(my_dbuf.loadBackBuffer(istr_list) == 0);
	REQUIRE(!my_dbuf.readFrontBuffer(my_buf, 3));
	REQUIRE(!my_dbuf.readFrontBuffer(my_buf, 3));
	REQUIRE(!my_dbuf.readFrontBuffer(my_buf, 3));
	REQUIRE(my_dbuf.loadBackBuffer(istr_list) == 0);
	REQUIRE(my_dbuf.readFrontBuffer(my_buf, 3));
	REQUIRE(my_dbuf.loadBackBuffer(istr_list) == 4);
	REQUIRE(!my_dbuf.readFrontBuffer(my_buf, 3));
	REQUIRE(!my_dbuf.readFrontBuffer(my_buf, 3));
	REQUIRE(my_dbuf.readFrontBuffer(my_buf, 3));
	REQUIRE(!my_dbuf.readFrontBuffer(my_buf, 3));

	REQUIRE(my_buf[0] == 21.f);
	REQUIRE(my_buf[1] == 22.f);
	REQUIRE(my_buf[2] == 23.f);

	REQUIRE(!my_dbuf.readFrontBuffer(my_buf, 3));

	REQUIRE(my_buf[0] == 0.f);
	REQUIRE(my_buf[1] == 0.f);
	REQUIRE(my_buf[2] == 0.f);

	ifstr.close();
}

TEST_CASE("double_buffer/double_thread", "Two threads accessing one double_buffer") {
	make_file(140.f);
	ifstream ifstr("/tmp/tmpdata", ios::in | ios::binary);
	forward_list<istream*> istrList(1, &ifstr);

	aram::service::LoadAndReadBuffer my_dbuf(20);
	my_dbuf.loadBackBufferAndSwap(istrList);

	thread back_thread(back_thread_func, ref(my_dbuf), ref(istrList));
	thread front_thread(front_thread_func, ref(my_dbuf));

	this_thread::sleep_for(chrono::seconds(3));
	run = false;
	front_thread.join();
	back_thread.join();

	ifstr.close();

	for (int i = 0; i < 140; i++) {
		REQUIRE(result[i] == static_cast<aram::service::sample_t> (i));
	}
	for (int i = 140; i < result.size(); i++) {
		REQUIRE(result[i] == 0.f);
	}
}

TEST_CASE("single_buffer", "Load from istream, then read it") {
	make_file(140.f);
	ifstream ifstr("/tmp/tmpdata", ios::in | ios::binary);

	aram::service::SingleBuffer my_sbuf(ifstr);

	aram::service::sample_t my_buf[14];

	for (int i = 0; i < 10; i++) {
		my_sbuf.read(my_buf, 14);
		for (int j = 0; j < 14; j++) {
			REQUIRE(my_buf[j] == 14.f * i + j);
		}
	}
}

TEST_CASE("write_and_store_buffer", "should store written data to file") {
	aram::service::WriteAndStoreBuffer my_dbuf(14);
	aram::service::sample_t my_buf[10];

	{
		//Get some data into buffer
		make_file(70.f);
		ifstream ifstr("/tmp/tmpdata", ios::in | ios::binary);
		aram::service::SingleBuffer my_sbuf(ifstr);
		my_sbuf.read(my_buf, 10);

		//Write to front buffer
		REQUIRE(my_dbuf.writeFrontBuffer(my_buf, 10));
	}

	{
		//Swap buffers and write to file
		ofstream ofstr("/tmp/tmpdataout", ios::out | ios::binary);
		REQUIRE(my_dbuf.swapAndStoreBackBuffer(ofstr) == 10);

		//Should be allowed to write 10 more bytes because of swap
		REQUIRE(my_dbuf.writeFrontBuffer(my_buf, 10));

		//Should not be allowed to write 10 more bytes because of no swap
		REQUIRE(!my_dbuf.writeFrontBuffer(my_buf, 10));
	}

	{
		//Finally check we got expected stored data
		ifstream ifstr("/tmp/tmpdataout", ios::in | ios::binary);
		aram::service::SingleBuffer my_sbuf(ifstr);
		my_sbuf.read(my_buf, 10);
		for (int j = 0; j < 10; j++) {
			REQUIRE(my_buf[j] == j);
		}
	}
}
