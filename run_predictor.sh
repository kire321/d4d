#!/bin/bash

(cd $2; cat `ls | sort`) | ./predictor $1
