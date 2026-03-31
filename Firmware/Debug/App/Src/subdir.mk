################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/Src/app.c \
../App/Src/app_callbacks.c 

C_DEPS += \
./App/Src/app.d \
./App/Src/app_callbacks.d 

OBJS += \
./App/Src/app.o \
./App/Src/app_callbacks.o 


# Each subdirectory must supply rules for building sources it contributes
App/Src/%.o App/Src/%.su App/Src/%.cyclo: ../App/Src/%.c App/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -IC:/Users/Lenovo/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.7/Drivers/STM32F1xx_HAL_Driver/Inc -IC:/Users/Lenovo/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.7/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -IC:/Users/Lenovo/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.7/Drivers/CMSIS/Device/ST/STM32F1xx/Include -IC:/Users/Lenovo/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.7/Drivers/CMSIS/Include -I../App/Inc -I../Modules/Inc -I../Platform/nc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-App-2f-Src

clean-App-2f-Src:
	-$(RM) ./App/Src/app.cyclo ./App/Src/app.d ./App/Src/app.o ./App/Src/app.su ./App/Src/app_callbacks.cyclo ./App/Src/app_callbacks.d ./App/Src/app_callbacks.o ./App/Src/app_callbacks.su

.PHONY: clean-App-2f-Src

