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
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
firefly_jar.lss \

FLASH_IMAGE += \
firefly_jar.hex \

EEPROM_IMAGE += \
firefly_jar.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: firefly_jar.elf secondary-outputs

# Tool invocations
firefly_jar.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,firefly_jar.map -mmcu=attiny13 -o "firefly_jar.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

firefly_jar.lss: firefly_jar.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S firefly_jar.elf  >"firefly_jar.lss"
	@echo 'Finished building: $@'
	@echo ' '

firefly_jar.hex: firefly_jar.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex firefly_jar.elf  "firefly_jar.hex"
	@echo 'Finished building: $@'
	@echo ' '

firefly_jar.eep: firefly_jar.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex firefly_jar.elf  "firefly_jar.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: firefly_jar.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=attiny13 firefly_jar.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) firefly_jar.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
