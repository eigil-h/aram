#!/bin/bash
pushd src/main/cc/model
rm *-odb.?xx
odb -d sqlite --generate-query --generate-schema --std c++11 project.h
odb -d sqlite --generate-query --generate-schema --std c++11 audioclip.h
popd

