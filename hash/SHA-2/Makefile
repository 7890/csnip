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

default: $(BUILD)/sha224-test-sha256$(OPTIMIZE) \
$(BUILD)/sha224-test-sha256-x86$(OPTIMIZE) \
$(BUILD)/sha224-test-sha256-x8664$(OPTIMIZE) \
$(BUILD)/sha256-test-sha256$(OPTIMIZE) \
$(BUILD)/sha256-test-sha256-x86$(OPTIMIZE) \
$(BUILD)/sha256-test-sha256-x8664$(OPTIMIZE) \
$(BUILD)/sha384-test-sha512$(OPTIMIZE) \
$(BUILD)/sha384-test-sha512-x86$(OPTIMIZE) \
$(BUILD)/sha384-test-sha512-x8664$(OPTIMIZE) \
$(BUILD)/sha512-test-sha512$(OPTIMIZE) \
$(BUILD)/sha512-test-sha512-x86$(OPTIMIZE) \
$(BUILD)/sha512-test-sha512-x8664$(OPTIMIZE)

#=========
$(BUILD)/sha224-test-sha256$(OPTIMIZE): $(SOURCE)/sha224-test.c $(SOURCE)/sha256.c
	$(CC) -o $@ $^ $(OPTIMIZE)

$(BUILD)/sha224-test-sha256-x86$(OPTIMIZE): $(SOURCE)/sha224-test.c $(SOURCE)/sha256-x86.S
ifeq ($(ARCH),32)
	$(CC) -o $@ $^ $(OPTIMIZE)
else
	@echo "won't build $@ (x86 only)"
endif

$(BUILD)/sha224-test-sha256-x8664$(OPTIMIZE): $(SOURCE)/sha224-test.c $(SOURCE)/sha256-x8664.S
ifeq ($(ARCH),64)
	$(CC) -o $@ $^ $(OPTIMIZE)
else
	@echo "won't build $@ (x86-64 only)"
endif

#=========
$(BUILD)/sha256-test-sha256$(OPTIMIZE): $(SOURCE)/sha256-test.c $(SOURCE)/sha256.c
	$(CC) -o $@ $^ $(OPTIMIZE)

$(BUILD)/sha256-test-sha256-x86$(OPTIMIZE): $(SOURCE)/sha256-test.c $(SOURCE)/sha256-x86.S
ifeq ($(ARCH),32)
	$(CC) -o $@ $^ $(OPTIMIZE)
else
	@echo "won't build $@ (x86 only)"
endif

$(BUILD)/sha256-test-sha256-x8664$(OPTIMIZE): $(SOURCE)/sha256-test.c $(SOURCE)/sha256-x8664.S
ifeq ($(ARCH),64)
	$(CC) -o $@ $^ $(OPTIMIZE)
else
	@echo "won't build $@ (x86-64 only)"
endif

#=========
$(BUILD)/sha384-test-sha512$(OPTIMIZE): $(SOURCE)/sha384-test.c $(SOURCE)/sha512.c
	$(CC) -o $@ $^ $(OPTIMIZE)

$(BUILD)/sha384-test-sha512-x86$(OPTIMIZE): $(SOURCE)/sha384-test.c $(SOURCE)/sha512-x86.S
ifeq ($(ARCH),32)
	$(CC) -o $@ $^ $(OPTIMIZE)
else
	@echo "won't build $@ (x86 only)"
endif

$(BUILD)/sha384-test-sha512-x8664$(OPTIMIZE): $(SOURCE)/sha384-test.c $(SOURCE)/sha512-x8664.S
ifeq ($(ARCH),64)
	$(CC) -o $@ $^ $(OPTIMIZE)
else
	@echo "won't build $@ (x86-64 only)"
endif

#=========
$(BUILD)/sha512-test-sha512$(OPTIMIZE): $(SOURCE)/sha512-test.c $(SOURCE)/sha512.c
	$(CC) -o $@ $^ $(OPTIMIZE)

$(BUILD)/sha512-test-sha512-x86$(OPTIMIZE): $(SOURCE)/sha512-test.c $(SOURCE)/sha512-x86.S
ifeq ($(ARCH),32)
	$(CC) -o $@ $^ $(OPTIMIZE)
else
	@echo "won't build $@ (x86 only)"
endif

$(BUILD)/sha512-test-sha512-x8664$(OPTIMIZE): $(SOURCE)/sha512-test.c $(SOURCE)/sha512-x8664.S
ifeq ($(ARCH),64)
	$(CC) -o $@ $^ $(OPTIMIZE)
else
	@echo "won't build $@ (x86-64 only)"
endif

clean:
	-rm -f $(BUILD)/*
	-rm -rf $(BUILD)
