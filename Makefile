CFLAGS = -Wall -Wextra 
CFLAGS = -Wall
  
LIBS = -lcunit
  
TARGET = tests
  
SRCS = b_list.c b_list_tests.c
OBJS = b_list b_list_tests 
  
all: $(TARGET)
 
$(TARGET): $(OBJS)
	gcc $(CFLAGS) -o $@ $^ $(LIBS)
 
b_list: b_list.c
	gcc $(CFLAGS) -c $< -o $@
 
b_list_tests: b_list_tests.c
	gcc $(CFLAGS) -c $< -o $@
 
clean:
	rm -f $(OBJS) $(TARGET)
