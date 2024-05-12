#!/bin/bash

#get generated settings files
rsync -av --update picture@picture.uml.edu:piccam/bin/output/settings/* bin/output/settings/
