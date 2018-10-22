################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include libraries/WiFiManager/subdir.mk
-include libraries/ESP8266WiFi/src/subdir.mk
-include libraries/ESP8266WebServer/src/detail/subdir.mk
-include libraries/ESP8266WebServer/src/subdir.mk
-include libraries/ESP8266HTTPClient/src/subdir.mk
-include libraries/DNSServer/src/subdir.mk
-include core/core/umm_malloc/subdir.mk
-include core/core/spiffs/subdir.mk
-include core/core/libb64/subdir.mk
-include core/core/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
AR += \
/devel/arduino/sloeberWorkspaces/workspace/testWemos/Release/arduino.ar \

ELF += \
testWemos.elf \

HEX += \
testWemos.hex \


# All Target
all: testWemos

# Tool invocations
testWemos: $(HEX)
	@echo 'Building target: $@'
	@echo 'Printing size:'
	"/devel/arduino/sloeber-431//arduinoPlugin/packages/esp8266/tools/xtensa-lx106-elf-gcc/1.20.0-26-gb404fb9-2/bin/xtensa-lx106-elf-size" -A "/devel/arduino/sloeberWorkspaces/workspace/testWemos/Release/testWemos.elf"
	@echo 'Finished building target: $@'
	@echo ' '

/devel/arduino/sloeberWorkspaces/workspace/testWemos/Release/arduino.ar: $(AR_OBJ)
	@echo 'Starting archiver'
	"/devel/arduino/sloeber-431//arduinoPlugin/packages/esp8266/tools/xtensa-lx106-elf-gcc/1.20.0-26-gb404fb9-2/bin/xtensa-lx106-elf-ar" cru  "/devel/arduino/sloeberWorkspaces/workspace/testWemos/Release/arduino.ar"    /devel/arduino/sloeberWorkspaces/workspace/testWemos/Release/arduino.ar $(AR_OBJ)
	@echo 'Finished building: $@'
	@echo ' '

testWemos.elf: $(LINK_OBJ) $(AR)
	@echo 'Starting combiner'
	${A.RECIPE.HOOKS.LINKING.PRELINK.1.PATTERN}
	${A.RECIPE.C.COMBINE.PATTERN.1}  $(LINK_OBJ) ${A.RECIPE.C.COMBINE.PATTERN.2} ${A.ARCHIVE_FILE_PATH} ${A.RECIPE.C.COMBINE.PATTERN.3} ${JANTJE.EXTRA.ALL} ${JANTJE.EXTRA.LINK}${A.JANTJE.POST.LINK}
	@echo 'Finished building: $@'
	@echo ' '

testWemos.hex: $(ELF)
	@echo 'Do all objcopy commands'
	"/devel/arduino/sloeber-431//arduinoPlugin/packages/esp8266/tools/esptool/0.4.13/esptool" -eo "/devel/arduino/sloeber-431//arduinoPlugin/packages/esp8266/hardware/esp8266/2.4.2/bootloaders/eboot/eboot.elf" -bo "/devel/arduino/sloeberWorkspaces/workspace/testWemos/Release/testWemos.bin" -bm dio -bf 40 -bz 4M -bs .text -bp 4096 -ec -eo "/devel/arduino/sloeberWorkspaces/workspace/testWemos/Release/testWemos.elf" -bs .irom0.text -bs .text -bs .data -bs .rodata -bc -ec
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(PDE_DEPS)$(C_DEPS)$(AR)$(CC_DEPS)$(AR_OBJ)$(C++_DEPS)$(LINK_OBJ)$(CXX_DEPS)$(ASM_DEPS)$(HEX)$(INO_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(ELF)$(CPP_DEPS)
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
