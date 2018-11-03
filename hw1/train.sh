#!/bin/bash
epoch="$1"
#echo "$epoch"
./train "$epoch" model_init.txt ../dsp_hw1/seq_model_01.txt model_01.txt
./train "$epoch" model_init.txt ../dsp_hw1/seq_model_02.txt model_02.txt
./train "$epoch" model_init.txt ../dsp_hw1/seq_model_03.txt model_03.txt
./train "$epoch" model_init.txt ../dsp_hw1/seq_model_04.txt model_04.txt
./train "$epoch" model_init.txt ../dsp_hw1/seq_model_05.txt model_05.txt
