#!/bin/bash

# Compile server
gcc server.c vector.c -o server -lpthread

# Compile client
gcc client.c vector.c -o client -lpthread
