SRC_DIR = ./src
INC_DIR = ./inc
OUT_DIR = ./build
OBJ_DIR = $(OUT_DIR)/obj

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

.DEFAULT_GOAL := all

-include $(patsubst %.o,%.d,$(OBJ_FILES))

all: $(OBJ_FILES)
	@echo Enlazando $@
	@gcc $(OBJ_FILES) -o $(OUT_DIR)/app.elf

#-MDD generan dependencias para recompilar los .h cada vez que se cambian
#-D Define un #define algo como si fuera desde el hache
# -DUSED_STATIC_MEMORY -DMAX_GPIO_NUMBER=6

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo Compilando $<
	@mkdir -p $(OBJ_DIR)
	@gcc -o $@ -c $< -I$(INC_DIR) -MMD

clean:
	@rm -r $(OUT_DIR)

doc:
	@mkdir -p $(OUT_DIR)
	@doxygen doxyfile
