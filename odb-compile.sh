#!/bin/bash
pushd src/main/cc/model
rm *-odb.?xx
odb -d sqlite --generate-query --generate-schema project.h
odb -d sqlite --generate-query --generate-schema audioclip.h
popd

