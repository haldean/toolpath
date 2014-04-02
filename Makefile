EXECUTABLE=tp

all:
	$(MAKE) -C src
	cp src/$(EXECUTABLE) .

clean:
	$(MAKE) -C src clean
	rm $(EXECUTABLE)

run: all
	./$(EXECUTABLE)
