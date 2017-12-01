#!/bin/bash

sudo ip link set tun0 up
sudo ip addr add fc00:1234:ffff::1/64 dev tun0
