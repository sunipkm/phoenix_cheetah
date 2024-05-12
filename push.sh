#!/bin/bash

#sync files
rsync -av --delete --exclude-from=excludes.txt ../piccam picture@picture.uml.edu:

#make
ssh picture@picture.uml.edu 'source .bashrc; cd piccam; make'


