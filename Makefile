TARGET = flog
SOURCES = frog.c
CFLAGS = -Wall -Wextra -Werror -pthread

# Regra padrão para compilar com gprof
all: $(TARGET)_gprof

# Regra para compilar com gprof
$(TARGET)_gprof: $(SOURCES)
	gcc -pg $(CFLAGS) -o ./build/$(TARGET)_gprof $^


# Regra para compilar para usar com perf
$(TARGET)_perf: $(SOURCES)
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -f ./build/$(TARGET)_gprof ./build/$(TARGET)_perf ./build/gmon.out