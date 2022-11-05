################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
MMdriver01.lss \

FLASH_IMAGE += \
MMdriver01.hex \

EEPROM_IMAGE += \
MMdriver01.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: MMdriver01.elf secondary-outputs

# Tool invocations
MMdriver01.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-gcc -Os -Wl,--gc-sections  -L"/home/jon/workspace/Arduino_Uno/Release" -mmcu=atmega328p -o"MMdriver01.elf" $(OBJS) $(USER_OBJS) $(LIBS) -lm
	@echo 'Finished building target: $@'
	@echo ' '

MMdriver01.lss: MMdriver01.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S MMdriver01.elf  >"MMdriver01.lss"
	@echo 'Finished building: $@'
	@echo ' '

MMdriver01.hex: MMdriver01.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex MMdriver01.elf  "MMdriver01.hex"
	@echo 'Finished building: $@'
	@echo ' '

MMdriver01.eep: MMdriver01.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex MMdriver01.elf  "MMdriver01.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: MMdriver01.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=berkeley -t MMdriver01.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(CXX_DEPS)$(S_UPPER_DEPS)$(C++_DEPS)$(ASM_DEPS)$(CC_DEPS)$(INO_DEPS)$(CPP_DEPS)$(PDE_DEPS)$(C_UPPER_DEPS)$(SIZEDUMMY) MMdriver01.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
