#!/bin/bash

rm -f hstack.root
hadd hstack.root h1_run*
cd ..
tbrowser root_files/hstack.root