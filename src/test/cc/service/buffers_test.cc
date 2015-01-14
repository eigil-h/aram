#include "catch.hpp"
#include "../main/buffers.h"
#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>


static bool run = true;
vector<wreckit::sample_t> result;

void back_thread_func(wreckit::LoadAndReadBuffer& my_dbuf, istream& ifstr) {
	while (run) {
		this_thread::sleep_for(chrono::milliseconds(200));
		my_dbuf.load_back_buffer(ifstr);
	}
}

void front_thread_func(wreckit::LoadAndReadBuffer& my_dbuf) {
	wreckit::sample_t my_buf[3];
	while (run) {
		my_dbuf.read_front_buffer(my_buf, 3);
		result.push_back(my_buf[0]);
		result.push_back(my_buf[1]);
		result.push_back(my_buf[2]);
		this_thread::sleep_for(chrono::milliseconds(50));
	}
}

static void make_file(wreckit::sample_t max) {
	ofstream of("/tmp/tmpdata", ios::out | ios::binary | ios::trunc);
	if (of.is_open()) {
		for (wreckit::sample_t i = 0.f; i < max; i += 1.f) {
			of.write(reinterpret_cast<char*> (&i), sizeof (wreckit::sample_t));
		}
		of.close();
	} else {
		throw exception();
	}
}

TEST_CASE("double_buffer/single_thread", "Should behave as predicted in a single thread run.") {
	make_file(24.f);
	ifstream ifstr("/tmp/tmpdata", ios::in | ios::binary);

	wreckit::LoadAndReadBuffer my_dbuf(10);
	wreckit::sample_t my_buf[3];

	REQUIRE(my_dbuf.load_back_buffer_and_swap(ifstr) == 10);
	REQUIRE(my_dbuf.load_back_buffer(ifstr) == 10);
	REQUIRE(my_dbuf.load_back_buffer(ifstr) == 0);
	REQUIRE(!my_dbuf.read_front_buffer(my_buf, 3));
	REQUIRE(!my_dbuf.read_front_buffer(my_buf, 3));
	REQUIRE(!my_dbuf.read_front_buffer(my_buf, 3));
	REQUIRE(my_dbuf.load_back_buffer(ifstr) == 0);
	REQUIRE(my_dbuf.read_front_buffer(my_buf, 3));
	REQUIRE(my_dbuf.load_back_buffer(ifstr) == 4);
	REQUIRE(!my_dbuf.read_front_buffer(my_buf, 3));
	REQUIRE(!my_dbuf.read_front_buffer(my_buf, 3));
	REQUIRE(my_dbuf.read_front_buffer(my_buf, 3));
	REQUIRE(!my_dbuf.read_front_buffer(my_buf, 3));

	REQUIRE(my_buf[0] == 21.f);
	REQUIRE(my_buf[1] == 22.f);
	REQUIRE(my_buf[2] == 23.f);

	REQUIRE(!my_dbuf.read_front_buffer(my_buf, 3));

	REQUIRE(my_buf[0] == 0.f);
	REQUIRE(my_buf[1] == 0.f);
	REQUIRE(my_buf[2] == 0.f);

	ifstr.close();
}

TEST_CASE("double_buffer/double_thread", "Two threads accessing one double_buffer") {
	make_file(140.f);
	ifstream ifstr("/tmp/tmpdata", ios::in | ios::binary);

	wreckit::LoadAndReadBuffer my_dbuf(20);
	my_dbuf.load_back_buffer_and_swap(ifstr);

	thread back_thread(back_thread_func, ref(my_dbuf), ref(ifstr));
	thread front_thread(front_thread_func, ref(my_dbuf));

	this_thread::sleep_for(chrono::seconds(3));
	run = false;
	front_thread.join();
	back_thread.join();

	ifstr.close();

	for (int i = 0; i < 140; i++) {
		REQUIRE(result[i] == static_cast<wreckit::sample_t> (i));
	}
	for (int i = 140; i < result.size(); i++) {
		REQUIRE(result[i] == 0.f);
	}
}

TEST_CASE("single_buffer", "Load from istream, then read it") {
	make_file(140.f);
	ifstream ifstr("/tmp/tmpdata", ios::in | ios::binary);

	wreckit::SingleBuffer my_sbuf(ifstr);

	wreckit::sample_t my_buf[14];

	for (int i = 0; i < 10; i++) {
		my_sbuf.read(my_buf, 14);
		for (int j = 0; j < 14; j++) {
			REQUIRE(my_buf[j] == 14.f * i + j);
		}
	}
}

TEST_CASE("write_and_store_buffer", "should store written data to file") {
	wreckit::WriteAndStoreBuffer my_dbuf(14);
	wreckit::sample_t my_buf[10];

	{
		//Get some data into buffer
		make_file(70.f);
		ifstream ifstr("/tmp/tmpdata", ios::in | ios::binary);
		wreckit::SingleBuffer my_sbuf(ifstr);
		my_sbuf.read(my_buf, 10);

		//Write to front buffer
		REQUIRE(my_dbuf.write_front_buffer(my_buf, 10));
	}

	{
		//Swap buffers and write to file
		ofstream ofstr("/tmp/tmpdataout", ios::out | ios::binary);
		REQUIRE(my_dbuf.swap_and_store_back_buffer(ofstr) == 10);

		//Should be allowed to write 10 more bytes because of swap
		REQUIRE(my_dbuf.write_front_buffer(my_buf, 10));

		//Should not be allowed to write 10 more bytes because of no swap
		REQUIRE(!my_dbuf.write_front_buffer(my_buf, 10));
	}

	{
		//Finally check we got expected stored data
		ifstream ifstr("/tmp/tmpdataout", ios::in | ios::binary);
		wreckit::SingleBuffer my_sbuf(ifstr);
		my_sbuf.read(my_buf, 10);
		for (int j = 0; j < 10; j++) {
			REQUIRE(my_buf[j] == j);
		}
	}
}
