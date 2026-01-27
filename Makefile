cc := gcc

log := thirdparty/log/log.c thirdparty/log/log.h

log_test: test/log_test.c $(log)
	@$(cc) $^ -o $@ -Ithirdparty
	@./$@
	@rm -rf $@
