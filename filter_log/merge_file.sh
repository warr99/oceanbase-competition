#!/bin/bash
  
# Check if the correct number of arguments is provided
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 TRACE_ID LOG_PATH MERGE_LOG_PATH"
    exit 1
fi

TRACE_ID=$1
LOG_PATH=$2
MERGE_LOG_PATH=$3

# Use grep to find lines containing the TRACE_ID in specified log files
grep "${TRACE_ID}" "${LOG_PATH}/observer"* "${LOG_PATH}/election"* "${LOG_PATH}/rootservice"* \
    | sed 's/:/ /' \
    | awk '{tmp=$1; $1=$2; $2=$3; $3=$4; $4=tmp; print $0}' \
    | sort > "${MERGE_LOG_PATH}"

echo "Merged logs with TRACE_ID ${TRACE_ID} and saved to ${MERGE_LOG_PATH}"