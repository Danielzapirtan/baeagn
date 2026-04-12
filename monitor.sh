#! /bin/bash

for PLATFORM in iccf chess; do
    gh run list --workflow $PLATFORM --status in_progress | cat
done
