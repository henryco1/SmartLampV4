################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Tests/ws2812b_LED_mock.c \
../Core/Tests/ws2812b_tests.c 

OBJS += \
./Core/Tests/ws2812b_LED_mock.o \
./Core/Tests/ws2812b_tests.o 

C_DEPS += \
./Core/Tests/ws2812b_LED_mock.d \
./Core/Tests/ws2812b_tests.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Tests/ws2812b_LED_mock.o: ../Core/Tests/ws2812b_LED_mock.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DTEST_ENABLED -DDEBUG -DSTM32L432xx -c -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Tests/ws2812b_LED_mock.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Tests/ws2812b_tests.o: ../Core/Tests/ws2812b_tests.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DTEST_ENABLED -DDEBUG -DSTM32L432xx -c -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Tests/ws2812b_tests.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

