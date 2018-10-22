################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../sloeber.ino.cpp 

LINK_OBJ += \
./sloeber.ino.cpp.o 

CPP_DEPS += \
./sloeber.ino.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
sloeber.ino.cpp.o: ../sloeber.ino.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/devel/arduino/sloeber-431//arduinoPlugin/packages/esp8266/tools/xtensa-lx106-elf-gcc/1.20.0-26-gb404fb9-2/bin/xtensa-lx106-elf-g++" -D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ "-I/devel/arduino/sloeber-431//arduinoPlugin/packages/esp8266/hardware/esp8266/2.4.2/tools/sdk/include" "-I/devel/arduino/sloeber-431//arduinoPlugin/packages/esp8266/hardware/esp8266/2.4.2/tools/sdk/lwip2/include" "-I/devel/arduino/sloeber-431//arduinoPlugin/packages/esp8266/hardware/esp8266/2.4.2/tools/sdk/libc/xtensa-lx106-elf/include" "-I/devel/arduino/sloeberWorkspaces/workspace/testWemos/Release/core" -c -Wall -Wextra  -Os -g -mlongcalls -mtext-section-literals -fno-exceptions -fno-rtti -falign-functions=4 -std=c++11 -MMD -ffunction-sections -fdata-sections -DF_CPU=80000000L -DLWIP_OPEN_SRC -DTCP_MSS=536   -DARDUINO=10802 -DARDUINO_ESP8266_WEMOS_D1R1 -DARDUINO_ARCH_ESP8266 -DARDUINO_BOARD="ESP8266_WEMOS_D1R1"   -DESP8266   -I"/devel/arduino/sloeber-431/arduinoPlugin/packages/esp8266/hardware/esp8266/2.4.2/cores/esp8266" -I"/devel/arduino/sloeber-431/arduinoPlugin/packages/esp8266/hardware/esp8266/2.4.2/variants/d1" -I"/devel/arduino/sloeber-431/arduinoPlugin/packages/esp8266/hardware/esp8266/2.4.2/libraries/esp8266/src" -I"/devel/arduino/sloeber-431/arduinoPlugin/packages/esp8266/hardware/esp8266/2.4.2/libraries/ESP8266WiFi/src" -I"/devel/arduino/sloeber-431/arduinoPlugin/packages/esp8266/hardware/esp8266/2.4.2/libraries/ESP8266HTTPClient/src" -I"/devel/arduino/sloeber-431/arduinoPlugin/libraries/WiFiManager/0.14" -I"/devel/arduino/sloeber-431/arduinoPlugin/packages/esp8266/hardware/esp8266/2.4.2/libraries/DNSServer/src" -I"/devel/arduino/sloeber-431/arduinoPlugin/packages/esp8266/hardware/esp8266/2.4.2/libraries/ESP8266WebServer/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"

	@echo 'Finished building: $<'
	@echo ' '


