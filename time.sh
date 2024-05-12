#!/bin/bash

#set date & time on flight computer
ssh root@picture.uml.edu date -s @`( date -u +"%s" )`

