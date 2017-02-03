#!/bin/bash

# Compile server
gcc server.c -o server -lpthread

# Compile client
gcc client.c -o client -lpthread
