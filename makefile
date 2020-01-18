SRC_DIR := ./src
OBJ_DIR := ./obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -pthread -lpthread
CCFLAGS := -Wall 

main: dir $(OBJ_FILES)
	g++ $(LDFLAGS) $(CCFLAGS) -o ./server $(OBJ_FILES)

dir:
	mkdir -p obj

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CCFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ_FILES)
	rm -f ./server