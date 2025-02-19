PROJECT_NAME ?= aba
VERSION ?= 0.0.1
CC ?= gcc
BUILD ?= debug #release
FLAGS += -Wall #-Werror #-O2
LINKS ?= abautils.c
SOURCE ?= str.c

ifeq ($(BUILD), debug)
	flags += -g -Og
endif
ifeq ($(BUILD), release)
    flags += -O3
endif

$(PROJECT_NAME): $(SOURCE)
	$(CC) -o $(PROJECT_NAME) $(SOURCE) $(FLAGS) $(LINKS)

