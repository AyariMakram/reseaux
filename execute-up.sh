#!/bin/bash

script="vagrant up"


cd VM1
$script &
cd ..

cd VM2
$script &
cd ..

cd VM3
$script &
cd ..

cd VM1-6
$script &
cd ..

cd VM2-6
$script &
cd ..

cd VM3-6
$script &
cd ..



