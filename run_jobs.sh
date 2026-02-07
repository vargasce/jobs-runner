#!/bin/sh

JOB_RUNNER="/home/javo/source/job-runner-c/job-runner"
CONFIG="/home/javo/source/job-runner-c/jobs.json"
LOGDIR="/home/javo/source/job-runner-c/logs"

mkdir -p "$LOGDIR"

exec "$JOB_RUNNER" >>"$LOGDIR/runner.log" 2>&1
