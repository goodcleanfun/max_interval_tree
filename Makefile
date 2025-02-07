
test:
	clib install --dev
	@$(CC) test.c -I src -I deps -o $@
	@./$@

.PHONY: test
