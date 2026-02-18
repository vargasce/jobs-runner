#!/bin/sh

JOB_RUNNER="/home/toro/source/job-runner-c/job-runner"
CONFIG="/home/toro/source/job-runner-c/jobs.json"
LOGDIR="/home/toro/source/job-runner-c/logs"

mkdir -p "$LOGDIR"

exec "$JOB_RUNNER" >>"$LOGDIR/runner.log" 2>&1
