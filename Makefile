BINARY=tp
TEST_BINARY=slicetest

all:
	$(MAKE) -C src
	cp src/$(BINARY) .

$(TEST_BINARY):
	$(MAKE) -C src $(TEST_BINARY)
	cp src/$(TEST_BINARY) .

clean:
	$(MAKE) -C src clean
	rm -f $(BINARY) $(TEST_BINARY)

run: all
	./$(EXECUTABLE)
