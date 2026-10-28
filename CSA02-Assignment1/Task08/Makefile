CC     = gcc
CFLAGS = -Wall -Wextra -O0
OBJS   = main.o input.o tariff.o report.o

billing_split: $(OBJS)
	$(CC) $(OBJS) -o $@
%.o: %.c billing.h tariff.h
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o billing_split
