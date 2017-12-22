#CC = gcc
CC = c99
#CC = g++

#where to find sources
SOURCE = src

#all generated / compiled files will be put below $(BUILD) dir
BUILD = _build

ARCH := $(shell getconf LONG_BIT)

#uncomment for non-optizied build
OPTIMIZE ?= -O2

$(shell mkdir -p $(BUILD)/)

.PHONY: default clean

default: $(BUILD)/sha1-test-naive$(OPTIMIZE) \
$(BUILD)/sha1-test-fast$(OPTIMIZE) \
$(BUILD)/sha1-test-naive-x86$(OPTIMIZE) \
$(BUILD)/sha1-test-fast-x86$(OPTIMIZE) \
$(BUILD)/sha1-test-fast-x8664$(OPTIMIZE)

$(BUILD)/sha1-test-naive$(OPTIMIZE): $(SOURCE)/sha1-test.c $(SOURCE)/sha1-naive.c
	$(CC) -o $@ $^ $(OPTIMIZE)

$(BUILD)/sha1-test-fast$(OPTIMIZE): $(SOURCE)/sha1-test.c $(SOURCE)/sha1-fast.c
	$(CC) -o $@ $^ $(OPTIMIZE)

$(BUILD)/sha1-test-naive-x86$(OPTIMIZE): $(SOURCE)/sha1-test.c $(SOURCE)/sha1-naive-x86.S
ifeq ($(ARCH),32)
	$(CC) -o $@ $^ $(OPTIMIZE)
else
	@echo "won't build $@ (x86 only)"
endif

$(BUILD)/sha1-test-fast-x86$(OPTIMIZE): $(SOURCE)/sha1-test.c $(SOURCE)/sha1-fast-x86.S
ifeq ($(ARCH),32)
	$(CC) -o $@ $^ $(OPTIMIZE)
else
	@echo "won't build $@ (x86 only)"
endif

$(BUILD)/sha1-test-fast-x8664$(OPTIMIZE): $(SOURCE)/sha1-test.c $(SOURCE)/sha1-fast-x8664.S
ifeq ($(ARCH),64)
	$(CC) -o $@ $^ $(OPTIMIZE)
else
	@echo "won't build $@ (x86-64 only)"
endif

clean:
	-rm -f $(BUILD)/*
	-rm -rf $(BUILD)
