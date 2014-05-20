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
rgb_color_matcher.lss \

FLASH_IMAGE += \
rgb_color_matcher.hex \

EEPROM_IMAGE += \
rgb_color_matcher.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: rgb_color_matcher.elf secondary-outputs

# Tool invocations
rgb_color_matcher.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,rgb_color_matcher.map -mmcu=atmega8 -o "rgb_color_matcher.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

rgb_color_matcher.lss: rgb_color_matcher.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S rgb_color_matcher.elf  >"rgb_color_matcher.lss"
	@echo 'Finished building: $@'
	@echo ' '

rgb_color_matcher.hex: rgb_color_matcher.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex rgb_color_matcher.elf  "rgb_color_matcher.hex"
	@echo 'Finished building: $@'
	@echo ' '

rgb_color_matcher.eep: rgb_color_matcher.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex rgb_color_matcher.elf  "rgb_color_matcher.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: rgb_color_matcher.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega8 rgb_color_matcher.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) rgb_color_matcher.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
