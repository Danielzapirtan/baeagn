#! /bin/bash

PLATFORM=$1

gh run list --workflow $PLATFORM --status in_progress 
