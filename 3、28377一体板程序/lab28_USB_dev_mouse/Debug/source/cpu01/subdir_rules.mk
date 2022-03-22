################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
source/cpu01/%.obj: ../source/cpu01/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/ti/CCS9.2/ccs/tools/compiler/ti-cgt-c2000_18.12.3.LTS/bin/cl2000" -v28 -ml -mt --vcu_support=vcu2 --tmu_support=tmu0 --cla_support=cla1 --float_support=fpu32 --include_path="D:/ti/CCS9.2/28377YT/lab28_USB_dev_mouse" --include_path="D:/ti/CCS9.2/ccs/tools/compiler/ti-cgt-c2000_18.12.3.LTS/include" --include_path="D:/ti/CCS9.2/28377YT/lab28_USB_dev_mouse/include" -g --define=CPU1 --define=DEBUG --define=ccs_c2k --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="source/cpu01/$(basename $(<F)).d_raw" --obj_directory="source/cpu01" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


