#!/usr/bin/env bash

# set -x


BIN_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
source $BIN_DIR/cluster.sh

LOCAL_HOST=`hostname`
EXECUTABLES=($1)
SCRIPTS=($2)
MAKE_FOLDER=$3
SCRIPT_FOLDER=$4
DEST_FOLDER=$5

cd $MAKE_FOLDER
#make clean
make $1
rm -rf core
cd -

for EXEC in "${EXECUTABLES[@]}"
do
	#echo "${EXEC} copied to {${HOSTS[@]/$LOCAL_HOST}}"
	for HOST in "${HOSTS[@]}"
	do
		ssh ${HOST} "mkdir -p ${DEST_FOLDER}"
		scp $MAKE_FOLDER/${EXEC} ${HOST}:${DEST_FOLDER}/${EXEC}
		echo "${EXEC} copied to ${HOST}"
	done
done

for SCRIPT in "${SCRIPTS[@]}"
do
	#echo "${EXEC} copied to {${HOSTS[@]/$LOCAL_HOST}}"
	for HOST in "${HOSTS[@]}"
	do
		scp -o ConnectTimeout=5 $SCRIPT_FOLDER/${SCRIPT} ${HOST}:${DEST_FOLDER}/${SCRIPT}
		scp -o ConnectTimeout=5 $SCRIPT_FOLDER/cluster.sh ${HOST}:${DEST_FOLDER}/cluster.sh
		echo "${SCRIPT} copied to ${HOST}"
	done
done