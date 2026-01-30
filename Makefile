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


mqtt := app/app_mqtt.c app/app_mqtt.h
mqtt_test: test/mqtt_test.c $(log) $(cjson) $(mqtt)
	@$(cc) $^ -o $@ -Ithirdparty -Iapp -lpaho-mqtt3c
	@./$@
	@rm -rf $@


thread_test: test/thread_test.c $(log) 
	@$(cc) $^ -o $@ -Ithirdparty -Iapp
	@./$@
	@rm -rf $@


thread_test_1: test/thread_test_1.c $(log) 
	@$(cc) $^ -o $@ -Ithirdparty -Iapp
	@./$@
	@rm -rf $@



thread_test_2: test/thread_test_2.c $(log) 
	@$(cc) $^ -o $@ -Ithirdparty -Iapp
	@./$@
	@rm -rf $@


app_pool := app/app_pool.c app/app_pool.h
app_pool_test: test/app_pool_test.c $(log) $(app_pool)
	@$(cc) $^ -o $@ -Ithirdparty -Iapp
	@./$@
	@rm -rf $@

app_buffer := app/app_buffer.c app/app_buffer.h
app_buffer_test: test/app_buffer_test.c $(log) $(app_buffer)
	@$(cc) $^ -o $@ -Ithirdparty -Iapp
	@./$@
	@rm -rf $@

