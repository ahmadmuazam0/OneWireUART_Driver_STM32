################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Platform/Src/fe_platform_stm32_hal.c 

C_DEPS += \
./Platform/Src/fe_platform_stm32_hal.d 

OBJS += \
./Platform/Src/fe_platform_stm32_hal.o 


# Each subdirectory must supply rules for building sources it contributes
Platform/Src/%.o Platform/Src/%.su Platform/Src/%.cyclo: ../Platform/Src/%.c Platform/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -IC:/Users/Lenovo/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.7/Drivers/STM32F1xx_HAL_Driver/Inc -IC:/Users/Lenovo/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.7/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -IC:/Users/Lenovo/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.7/Drivers/CMSIS/Device/ST/STM32F1xx/Include -IC:/Users/Lenovo/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.7/Drivers/CMSIS/Include -I../App/Inc -I../Modules/Inc -I../Platform/nc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Platform-2f-Src

clean-Platform-2f-Src:
	-$(RM) ./Platform/Src/fe_platform_stm32_hal.cyclo ./Platform/Src/fe_platform_stm32_hal.d ./Platform/Src/fe_platform_stm32_hal.o ./Platform/Src/fe_platform_stm32_hal.su

.PHONY: clean-Platform-2f-Src

