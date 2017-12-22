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

default: $(BUILD)/md5-test$(OPTIMIZE) \
$(BUILD)/md5-test-naive-x86$(OPTIMIZE) \
$(BUILD)/md5-test-fast-x86$(OPTIMIZE) \
$(BUILD)/md5-test-fast-x8664$(OPTIMIZE)

$(BUILD)/md5-test$(OPTIMIZE): $(SOURCE)/md5-test.c $(SOURCE)/md5.c
	$(CC) -o $@ $^ $(OPTIMIZE)

$(BUILD)/md5-test-naive-x86$(OPTIMIZE): $(SOURCE)/md5-test.c $(SOURCE)/md5-naive-x86.S
ifeq ($(ARCH),32)
	$(CC) -o $@ $^ $(OPTIMIZE)
else
	@echo "won't build $@ (x86 only)"
endif

$(BUILD)/md5-test-fast-x86$(OPTIMIZE): $(SOURCE)/md5-test.c $(SOURCE)/md5-fast-x86.S
ifeq ($(ARCH),32)
	$(CC) -o $@ $^ $(OPTIMIZE)
else
	@echo "won't build $@ (x86 only)"
endif

$(BUILD)/md5-test-fast-x8664$(OPTIMIZE): $(SOURCE)/md5-test.c $(SOURCE)/md5-fast-x8664.S
ifeq ($(ARCH),64)
	$(CC) -o $@ $^ $(OPTIMIZE)
else
	@echo "won't build $@ (x86-64 only)"
endif

clean:
	-rm -f $(BUILD)/*
	-rm -rf $(BUILD)
