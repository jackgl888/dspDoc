################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/ti/CCS9.2/ccs/tools/compiler/ti-cgt-c2000_18.12.3.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./source/F2837xS_CodeStartBranch.obj" \
"./source/F2837xS_CpuTimers.obj" \
"./source/F2837xS_DefaultISR.obj" \
"./source/F2837xS_GlobalVariableDefs.obj" \
"./source/F2837xS_Gpio.obj" \
"./source/F2837xS_PieCtrl.obj" \
"./source/F2837xS_PieVect.obj" \
"./source/F2837xS_Spi.obj" \
"./source/F2837xS_SysCtrl.obj" \
"./source/F2837xS_usDelay.obj" \
"./source/SD_SPI_Erase.obj" \
"./source/SD_SPI_Initialization.obj" \
"./source/SD_SPI_Read.obj" \
"./source/SD_SPI_Registers.obj" \
"./source/SD_SPI_Transmission.obj" \
"./source/SD_SPI_Write.obj" \
"./source/disk_sd.obj" \
"./source/diskio.obj" \
"./source/ff.obj" \
"./source/main.obj" \
"../cmd/2837xS_RAM_lnk_cpu1_USB.cmd" \
"../cmd/F2837xS_Headers_nonBIOS.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include cmd/subdir_vars.mk
-include source/subdir_vars.mk
-include cmd/subdir_rules.mk
-include source/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
lab17_SD_SPI.out \

EXE_OUTPUTS__QUOTED += \
"lab17_SD_SPI.out" \

BIN_OUTPUTS += \
lab17_SD_SPI.hex \

BIN_OUTPUTS__QUOTED += \
"lab17_SD_SPI.hex" \


# All Target
all: lab17_SD_SPI.out

# Tool invocations
lab17_SD_SPI.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"D:/ti/CCS9.2/ccs/tools/compiler/ti-cgt-c2000_18.12.3.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --cla_support=cla1 -g --define=CPU1 --define="LARGE_MODEL" --define="_DEBUG" --diag_wrap=off --diag_warning=225 --display_error_number -z -m"lab17_SD_SPI.map" --stack_size=0x200 --warn_sections -i"D:/ti/CCS9.2/ccs/tools/compiler/ti-cgt-c2000_18.12.3.LTS/lib" -i"D:/ti/CCS9.2/ccs/tools/compiler/ti-cgt-c2000_18.12.3.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="lab17_SD_SPI_linkInfo.xml" --rom_model -o "lab17_SD_SPI.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

lab17_SD_SPI.hex: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: C2000 Hex Utility'
	"D:/ti/CCS9.2/ccs/tools/compiler/ti-cgt-c2000_18.12.3.LTS/bin/hex2000"  -o "lab17_SD_SPI.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "source\F2837xS_CodeStartBranch.obj" "source\F2837xS_CpuTimers.obj" "source\F2837xS_DefaultISR.obj" "source\F2837xS_GlobalVariableDefs.obj" "source\F2837xS_Gpio.obj" "source\F2837xS_PieCtrl.obj" "source\F2837xS_PieVect.obj" "source\F2837xS_Spi.obj" "source\F2837xS_SysCtrl.obj" "source\F2837xS_usDelay.obj" "source\SD_SPI_Erase.obj" "source\SD_SPI_Initialization.obj" "source\SD_SPI_Read.obj" "source\SD_SPI_Registers.obj" "source\SD_SPI_Transmission.obj" "source\SD_SPI_Write.obj" "source\disk_sd.obj" "source\diskio.obj" "source\ff.obj" "source\main.obj" 
	-$(RM) "source\F2837xS_CpuTimers.d" "source\F2837xS_DefaultISR.d" "source\F2837xS_GlobalVariableDefs.d" "source\F2837xS_Gpio.d" "source\F2837xS_PieCtrl.d" "source\F2837xS_PieVect.d" "source\F2837xS_Spi.d" "source\F2837xS_SysCtrl.d" "source\SD_SPI_Erase.d" "source\SD_SPI_Initialization.d" "source\SD_SPI_Read.d" "source\SD_SPI_Registers.d" "source\SD_SPI_Transmission.d" "source\SD_SPI_Write.d" "source\disk_sd.d" "source\diskio.d" "source\ff.d" "source\main.d" 
	-$(RM) "source\F2837xS_CodeStartBranch.d" "source\F2837xS_usDelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

