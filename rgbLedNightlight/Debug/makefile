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

SIZEDUMMY += \
sizedummy \


# All Target
all: rgbLedNightlight.elf secondary-outputs

# Tool invocations
rgbLedNightlight.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,rgbLedNightlight.map -mmcu=atmega16 -o"rgbLedNightlight.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

rgbLedNightlight.lss: rgbLedNightlight.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S rgbLedNightlight.elf  >"rgbLedNightlight.lss"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: rgbLedNightlight.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega16 rgbLedNightlight.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) rgbLedNightlight.elf
	-@echo ' '

secondary-outputs: $(LSS) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
