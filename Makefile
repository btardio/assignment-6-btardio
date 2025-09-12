all:
	$(MAKE) -C server all
clean:
	$(MAKE) -C server test
install:
	$(MAKE) -C server install
