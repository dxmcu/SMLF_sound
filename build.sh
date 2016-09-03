#!/bin/bash

function sound_server()
{
 bazel build //:sound_server -c opt --cpu=k8 --host_cpu=k8 --copt=-g0 --copt=-O3 --copt=-s --strip=always
 sudo cp bazel-bin/sound_server/sound_server output
 echo "build sounde_server done!!!"
}

function all()
{
  sound_server
}
$@
