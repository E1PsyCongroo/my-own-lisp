.DEFAULT_GOAL = app

WORK_DIR	:= 	$(shell pwd)
MPC_DIR		:= 	$(WORK_DIR)/mpc
BUILD_DIR	:= 	$(WORK_DIR)/build
OBJ_DIR		:= 	$(BUILD_DIR)/obj

# Project sources
BINARY		:= 	$(BUILD_DIR)/clisp
SRC_DIR		:= 	$(WORK_DIR)/src
SRCS		:=  $(shell find $(SRC_DIR) -type f -name "*.c")
OBJS 		:= 	$(SRCS:%.c=$(OBJ_DIR)/%.o)
INC_PATH	:=	$(MPC_DIR)
LIBS		:= 	$(MPC_DIR)/build/libmpc.so

# Compilation flags
INCLUDES 	:=	$(addprefix -I, $(INC_PATH))
CC			:= 	clang
LD			:= 	clang
CFLAGS 		:= 	-MMD -Wall -Wpedantic -Werror -std=c11 -Og -ggdb -fsanitize=address,undefined $(INCLUDES)
LDFLAGS 	:= 	$(CFLAGS) -ledit -lm

# Compilation patterns
$(OBJ_DIR)/%.o: %.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<

# Depencies
-include $(OBJS:.o=.d)

app: $(BINARY)

$(BINARY): $(OBJS)
	@echo + LD $@
	@$(LD) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)

run: app
	@echo RUN $(BINARY)
	@$(BINARY)

clean:
	-rm -rf $(BUILD_DIR)

.PHONY: run clean
