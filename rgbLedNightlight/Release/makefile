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
rgbLedNightlight.lss \

FLASH_IMAGE += \
rgbLedNightlight.hex \

EEPROM_IMAGE += \
rgbLedNightlight.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: rgbLedNightlight.elf secondary-outputs

# Tool invocations
rgbLedNightlight.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,rgbLedNightlight.map -mmcu=attiny45 -o "rgbLedNightlight.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

rgbLedNightlight.lss: rgbLedNightlight.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S rgbLedNightlight.elf  >"rgbLedNightlight.lss"
	@echo 'Finished building: $@'
	@echo ' '

rgbLedNightlight.hex: rgbLedNightlight.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex rgbLedNightlight.elf  "rgbLedNightlight.hex"
	@echo 'Finished building: $@'
	@echo ' '

rgbLedNightlight.eep: rgbLedNightlight.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex rgbLedNightlight.elf  "rgbLedNightlight.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: rgbLedNightlight.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=attiny45 rgbLedNightlight.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) rgbLedNightlight.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
