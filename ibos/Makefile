CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = 

BUILD = debug

ifeq ($(BUILD),debug)
	CFLAGS += -g -Og
	BUILD_DIR = build/debug
else ifeq ($(BUILD),release)
	CFLAGS += -O3 -DNDEBUG
	BUILD_DIR = build/release
endif

SRCS_DIR = src
TEST_SRCS_DIR = test/src
BENCH_SRCS_DIR = bench/src
MOCK_SRCS_DIR = mock/src

SRCS = $(wildcard $(SRCS_DIR)/*.c)
TEST_SRCS = $(wildcard $(TEST_SRCS_DIR)/*.c)
BENCH_SRCS = $(wildcard $(BENCH_SRCS_DIR)/*.c)
MOCK_SRCS = $(wildcard $(MOCK_SRCS_DIR)/*.c)

OBJS_DIR = $(BUILD_DIR)/$(SRCS_DIR)
TEST_OBJS_DIR = $(BUILD_DIR)/$(TEST_SRCS_DIR)
BENCH_OBJS_DIR = $(BUILD_DIR)/$(BENCH_SRCS_DIR)
MOCK_OBJS_DIR = $(BUILD_DIR)/$(MOCK_SRCS_DIR)

OBJS = $(patsubst $(SRCS_DIR)/%.c,$(OBJS_DIR)/%.o,$(SRCS))
TEST_OBJS = $(patsubst $(TEST_SRCS_DIR)/%.c,$(TEST_OBJS_DIR)/%.o,$(TEST_SRCS))
BENCH_OBJS = $(patsubst $(BENCH_SRCS_DIR)/%.c,$(BENCH_OBJS_DIR)/%.o,$(BENCH_SRCS))
MOCK_OBJS = $(patsubst $(MOCK_SRCS_DIR)/%.c,$(MOCK_OBJS_DIR)/%.o,$(MOCK_SRCS))

TEST_TARGETS = $(TEST_OBJS:.o=)
BENCH_TARGETS = $(BENCH_OBJS:.o=)

MOCKED_OBJS = $(addprefix $(OBJS_DIR)/, $(notdir $(MOCK_OBJS)))
UNMOCKED_OBJS = $(filter-out $(MOCKED_OBJS), $(OBJS))

MOCKED_TEST_TARGETS = $(TEST_OBJS:.o=_mock)
MOCKED_BENCH_TARGETS = $(BENCH_OBJS:.o=_mock)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJS_DIR): | $(BUILD_DIR)
	mkdir -p $(OBJS_DIR)

$(TEST_OBJS_DIR): | $(BUILD_DIR)
	mkdir -p $(TEST_OBJS_DIR)

$(BENCH_OBJS_DIR): | $(BUILD_DIR)
	mkdir -p $(BENCH_OBJS_DIR)

$(MOCK_OBJS_DIR): | $(BUILD_DIR)
	mkdir -p $(MOCK_OBJS_DIR)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c | $(OBJS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJS_DIR)/%.o: $(TEST_SRCS_DIR)/%.c | $(TEST_OBJS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCH_OBJS_DIR)/%.o: $(BENCH_SRCS_DIR)/%.c | $(BENCH_OBJS_DIR) 
	$(CC) $(CFLAGS) -c $< -o $@

$(MOCK_OBJS_DIR)/%.o: $(MOCK_SRCS_DIR)/%.c | $(MOCK_OBJS_DIR) 
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJS_DIR)/%: $(TEST_OBJS_DIR)/%.o $(OBJS) 
	$(CC) $(LDFLAGS) -o $@ $^

$(BENCH_OBJS_DIR)/%: $(BENCH_OBJS_DIR)/%.o $(OBJS) 
	$(CC) $(LDFLAGS) -o $@ $^

$(TEST_OBJS_DIR)/%_mock: $(TEST_OBJS_DIR)/%.o $(UNMOCKED_OBJS) $(MOCK_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BENCH_OBJS_DIR)/%_mock: $(BENCH_OBJS_DIR)/%.o $(UNMOCKED_OBJS) $(MOCK_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -rf $(BUILD_DIR) 

build: $(OBJS) $(TEST_OBJS) $(BENCH_OBJS) $(MOCK_OBJS) $(TEST_TARGETS) $(BENCH_TARGETS) $(MOCKED_TEST_TARGETS) $(MOCKED_BENCH_TARGETS)

test: build
	@for test in $(TEST_TARGETS); do \
		./$$test; \
	done

test-mock: build
	@for test in $(MOCKED_TEST_TARGETS); do \
		./$$test; \
	done

bench: build
	@for bench in $(BENCH_TARGETS); do \
		./$$bench; \
	done

bench-mock: build
	@for bench in $(MOCKED_BENCH_TARGETS); do \
		./$$bench; \
	done
	
debug-info:
	@echo "CC: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "BUILD_DIR: $(BUILD_DIR)"
	@echo "BUILD: $(BUILD)"
	@echo "SRCS_DIR: $(SRCS_DIR)"
	@echo "TEST_SRCS_DIR: $(TEST_SRCS_DIR)"
	@echo "BENCH_SRCS_DIR: $(BENCH_SRCS_DIR)"
	@echo "MOCK_SRCS_DIR: $(MOCK_SRCS_DIR)"
	@echo "SRCS: $(SRCS)"
	@echo "TEST_SRCS: $(TEST_SRCS)"
	@echo "BENCH_SRCS: $(BENCH_SRCS)"
	@echo "MOCK_SRCS: $(MOCK_SRCS)"
	@echo "OBJS_DIR: $(OBJS_DIR)"
	@echo "TEST_OBJS_DIR: $(TEST_OBJS_DIR)"
	@echo "BENCH_OBJS_DIR: $(BENCH_OBJS_DIR)"
	@echo "MOCK_OBJS_DIR: $(MOCK_OBJS_DIR)"
	@echo "OBJS: $(OBJS)"
	@echo "TEST_OBJS: $(TEST_OBJS)"
	@echo "BENCH_OBJS: $(BENCH_OBJS)"
	@echo "MOCK_OBJS: $(MOCK_OBJS)"
	@echo "TEST_TARGETS: $(TEST_TARGETS)"
	@echo "BENCH_TARGETS: $(BENCH_TARGETS)"
	@echo "MOCKED_OBJS: $(MOCKED_OBJS)"
	@echo "UNMOCKED_OBJS: $(UNMOCKED_OBJS)"
	@echo "MOCKED_BENCH_TARGETS: $(MOCKED_BENCH_TARGETS)"
	@echo "MOCKED_TEST_TARGETS: $(MOCKED_TEST_TARGETS)"
