# 线程池项目 Makefile

# 编译器设置
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread

# 目标可执行文件
TARGET = threadpool_test

# 源文件
SRCS = threadpool.cpp test.cpp

# 目标文件
OBJS = $(SRCS:.cpp=.o)

# 默认目标
all: $(TARGET)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译源文件为目标文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

# 重新编译
rebuild: clean all

# 运行测试程序
run: $(TARGET)
	./$(TARGET)

# 调试版本
debug: CXXFLAGS += -g -DDEBUG
debug: rebuild

# 显示帮助信息
help:
	@echo "可用目标:"
	@echo "  all      - 编译项目 (默认)"
	@echo "  clean    - 清理生成的文件"
	@echo "  rebuild  - 重新编译"
	@echo "  run      - 编译并运行测试程序"
	@echo "  debug    - 编译调试版本"
	@echo "  help     - 显示此帮助信息"

# 声明伪目标
.PHONY: all clean rebuild run debug help
