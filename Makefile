

OBJ_DIR = ./obj
BIN_DIR = ./bin
SRC_DIR = ./src
SKEL_DIR = ./skeleton
INC_DIR = 


INCLUDE += \
	-I $(INC_DIR)


APP_SRC += \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/cxl.c \
	$(SRC_DIR)/dram.c \
	$(SRC_DIR)/init.c 
	

GCCFLAGS += \
	-g \
	-O2 \


V = 1
ifeq ($(V),1)
	Q =
else
	Q = @
endif

G_S = \033[0;32m
G_L = \033[0m

RUNFILE = sim_cxl


TARGET_OBJ = \
	$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(APP_SRC))

all: runfile

runfile: make_setting $(TARGET_OBJ);
	$(Q)gcc -o $(BIN_DIR)/$@ $(TARGET_OBJ) $(INC_DIR) $(GCCCFLAGS)
	$(info [ $(shell echo "$(G_S)OK$(G_L)") ] : Build $(RUNFILE) )
	ln -sf $(BIN_DIR)/$@ ./runfile



$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(Q)gcc -c $< -o $@ $(INC_DIR) $(GCCFLAGS)

.c.o:
	gcc -c $< -o $@ -I$(INC_DIR)

-include $(OBJS:.o=.d)

make_setting:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)
	$(info [ $(shell echo "$(G_S)OK$(G_L)") ] : Make Directory )


clean:
	@rm -vf $(BIN_DIR)/*
	@rm -vf $(OBJ_DIR)/*.o
	@rm -vf $(SRC_DIR)/*.o
	@rm -vf $(UTL_DIR)/*.o
	$(info [ $(shell echo "$(G_S)OK$(G_L)") ]: Clean)







