LIB_DIR = $(PWD)/external
DEPS = nonstd
LIB_DIRS =     $(foreach d, $(DEPS), $(LIB_DIR)/$d) 
LIB_INCLUDES = $(foreach d, $(DEPS), $(LIB_DIR)/$d/include) 

LIBSCLEAN=$(addsuffix clean,$(LIB_DIRS))
LIBSfCLEAN=$(addsuffix fclean,$(LIB_DIRS))
LIBSALL=$(addsuffix all,$(LIB_DIRS))

LIB_NAME = libnonstd_glfw_opengl

INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

EXE = $(BIN_DIR)/$(LIB_NAME).a
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)


# CPPFLAGS = -Iinclude -MMD -MP -Ofast
CFLAGS   = -Wall -Wextra -Werror -g -fpic 
LDFLAGS  = $(foreach d, $(LIB_DIRS), -L $d/bin) -shared 
LDLIBS   = $(foreach d, $(DEPS), -l$d)
INCLUDES = $(foreach d, $(LIB_INCLUDES), -I$d)

.PHONY: all clean  fclean re
all: $(LIBSALL) $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@ $(INCLUDES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean: $(LIBSCLEAN)
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

fclean: $(LIBSfCLEAN) clean
	rm -f $(EXE)

re: fclean | $(EXE)

%clean: %
	$(MAKE) -C $< clean

%fclean: %
	$(MAKE) -C $< fclean

%all: %
	$(MAKE) -C $< all

-include $(OBJ:.o=.d)