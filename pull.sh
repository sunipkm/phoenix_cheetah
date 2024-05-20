#!/bin/bash

#get generated settings files
rsync -av --update picture@picture.uml.edu:piccam/data/* data/

#clear
ssh picture@picture.uml.edu 'source .bashrc; cd piccam; rm -f data/*'