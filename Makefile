cc := gcc

log := thirdparty/log/log.c thirdparty/log/log.h
cjson := thirdparty/cjson/cJSON.c thirdparty/cjson/cJSON.h

log_test: test/log_test.c $(log)
	@$(cc) $^ -o $@ -Ithirdparty
	@./$@
	@rm -rf $@


cjso_test: test/cjson_test.c $(log) $(cjson)
	@$(cc) $^ -o $@ -Ithirdparty
	@./$@
	@rm -rf $@
