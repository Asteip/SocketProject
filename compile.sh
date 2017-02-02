#!/bin/bash

# Compile server
gcc -lpthread server.c -o server

# Compile client
gcc client.c -o client
