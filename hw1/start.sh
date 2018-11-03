#!/bin/bash
epoch="$1"
sh train.sh "$epoch" > train_log.txt 2>&1
sh predict.sh > predict_log.txt 2>&1
python3 score.py ans.txt ../dsp_hw1/testing_answer.txt
