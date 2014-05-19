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
traffic_lights_pwm.lss \

FLASH_IMAGE += \
traffic_lights_pwm.hex \

EEPROM_IMAGE += \
traffic_lights_pwm.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: traffic_lights_pwm.elf secondary-outputs

# Tool invocations
traffic_lights_pwm.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,traffic_lights_pwm.map -mmcu=attiny45 -o "traffic_lights_pwm.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

traffic_lights_pwm.lss: traffic_lights_pwm.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S traffic_lights_pwm.elf  >"traffic_lights_pwm.lss"
	@echo 'Finished building: $@'
	@echo ' '

traffic_lights_pwm.hex: traffic_lights_pwm.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex traffic_lights_pwm.elf  "traffic_lights_pwm.hex"
	@echo 'Finished building: $@'
	@echo ' '

traffic_lights_pwm.eep: traffic_lights_pwm.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex traffic_lights_pwm.elf  "traffic_lights_pwm.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: traffic_lights_pwm.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=attiny45 traffic_lights_pwm.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) traffic_lights_pwm.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
