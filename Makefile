PROJECT_NAME     := banji
TARGETS          := banji_dev banji
OUTPUT_DIRECTORY := _build

SDK_ROOT := sdk
PROJ_DIR := .
TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc
LINKER_SCRIPT  := ble_app_template_gcc_nrf52.ld

include $(TEMPLATE_PATH)/Makefile.common

GDB_PORT := 2331
GDB_CMD_PATH := gdb_cmds.txt
GDB := $(GNU_INSTALL_ROOT)/$(GNU_PREFIX)-gdb

# Toolchain commands
CC := $(GNU_INSTALL_ROOT)/$(GNU_PREFIX)-gcc
AS := $(GNU_INSTALL_ROOT)/$(GNU_PREFIX)-as
AR := $(GNU_INSTALL_ROOT)/$(GNU_PREFIX)-ar -r
LD := $(GNU_INSTALL_ROOT)/$(GNU_PREFIX)-ld
NM := $(GNU_INSTALL_ROOT)/$(GNU_PREFIX)-nm
OBJDUMP := $(GNU_INSTALL_ROOT)/$(GNU_PREFIX)-objdump
OBJCOPY := $(GNU_INSTALL_ROOT)/$(GNU_PREFIX)-objcopy
SIZE := $(GNU_INSTALL_ROOT)/$(GNU_PREFIX)-size
GDB := $(GNU_INSTALL_ROOT)/$(GNU_PREFIX)-gdb

ifeq ($(OS),Windows_NT)
# The Windows command shell 'start' function is used so the executable
# is started in its own window.
	TERMINAL := cmd /c start ""
	TERMINAL_END :=
	NRFJPROG := nrfjprog.exe
	GDBSERVER := JLinkGDBServerCL.exe
	RTT_CLIENT := JLinkRTTClient.exe
else
	TERMINAL := osascript -e 'tell app "Terminal" to do script "
	TERMINAL_END := "'
	NRFJPROG := nrfjprog
	GDBSERVER := JLinkGDBServer
	RTT_CLIENT := JLinkRTTClient
endif

# Source files common to all targets
SRC_FILES += \
	$(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52840.S \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
	$(SDK_ROOT)/components/libraries/button/app_button.c \
	$(SDK_ROOT)/components/libraries/util/app_error.c \
	$(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
	$(SDK_ROOT)/components/libraries/util/app_error_weak.c \
	$(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
	$(SDK_ROOT)/components/libraries/timer/app_timer2.c \
	$(SDK_ROOT)/components/libraries/util/app_util_platform.c \
	$(SDK_ROOT)/components/libraries/crc16/crc16.c \
	$(SDK_ROOT)/components/libraries/timer/drv_rtc.c \
	$(SDK_ROOT)/components/libraries/fds/fds.c \
	$(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c \
	$(SDK_ROOT)/components/libraries/util/nrf_assert.c \
	$(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
	$(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c \
	$(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
	$(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
	$(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
	$(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
	$(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
	$(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c \
	$(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
	$(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
	$(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
	$(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
	$(SDK_ROOT)/components/libraries/sortlist/nrf_sortlist.c \
	$(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
	$(SDK_ROOT)/components/libraries/sensorsim/sensorsim.c \
	$(SDK_ROOT)/modules/nrfx/mdk/system_nrf52840.c \
	$(SDK_ROOT)/components/boards/boards.c \
	$(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
	$(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_pdm.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spim.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spis.c \
	$(SDK_ROOT)/components/libraries/bsp/bsp.c \
	$(SDK_ROOT)/components/libraries/bsp/bsp_btn_ble.c \
	$(PROJ_DIR)/$(wildcard *.c) \
	$(PROJ_DIR)/$(wildcard HM01B0/*.c) \
	$(PROJ_DIR)/$(wildcard bmi270/*.c) \
	$(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
	$(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
	$(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
	$(SDK_ROOT)/components/ble/peer_manager/auth_status_tracker.c \
	$(SDK_ROOT)/components/ble/common/ble_advdata.c \
	$(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
	$(SDK_ROOT)/components/ble/common/ble_conn_params.c \
	$(SDK_ROOT)/components/ble/common/ble_conn_state.c \
	$(SDK_ROOT)/components/ble/common/ble_srv_common.c \
	$(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c \
	$(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c \
	$(SDK_ROOT)/components/ble/peer_manager/id_manager.c \
	$(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
	$(SDK_ROOT)/components/ble/nrf_ble_qwr/nrf_ble_qwr.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_database.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_id.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_manager.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_manager_handler.c \
	$(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c \
	$(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c \
	$(SDK_ROOT)/components/ble/peer_manager/security_manager.c \
	$(SDK_ROOT)/external/utf_converter/utf.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \
	$(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_qspi.c \
	$(SDK_ROOT)/components/libraries/cli/nrf_cli.c \
	$(SDK_ROOT)/components/libraries/cli/libuarte/nrf_cli_libuarte.c \
	$(SDK_ROOT)/components/libraries/libuarte/nrf_libuarte_drv.c \
	$(SDK_ROOT)/components/libraries/libuarte/nrf_libuarte_async.c \
	$(SDK_ROOT)/components/libraries/queue/nrf_queue.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_ppi.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_rtc.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_timer.c \
	$(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twi.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twim.c \
	$(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_twi.c \
	$(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_ppi.c \
	$(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_spis.c \


# Include folders common to all targets
INC_FOLDERS += \
	$(SDK_ROOT)/components/ble/ble_services/ble_ancs_c \
	$(SDK_ROOT)/components/ble/ble_services/ble_ias_c \
	$(SDK_ROOT)/components/libraries/pwm \
	$(SDK_ROOT)/components/libraries/usbd/class/cdc/acm \
	$(SDK_ROOT)/components/libraries/usbd/class/hid/generic \
	$(SDK_ROOT)/components/libraries/usbd/class/msc \
	$(SDK_ROOT)/components/libraries/usbd/class/hid \
	$(SDK_ROOT)/modules/nrfx/hal \
	$(SDK_ROOT)/components/nfc/ndef/conn_hand_parser/le_oob_rec_parser \
	$(SDK_ROOT)/components/libraries/log \
	$(SDK_ROOT)/components/ble/ble_services/ble_gls \
	$(SDK_ROOT)/components/libraries/fstorage \
	$(SDK_ROOT)/components/libraries/mutex \
	$(SDK_ROOT)/components/libraries/gpiote \
	$(SDK_ROOT)/components/libraries/bootloader/ble_dfu \
	$(SDK_ROOT)/components/nfc/ndef/connection_handover/common \
	$(SDK_ROOT)/components/boards \
	$(SDK_ROOT)/components/ble/ble_advertising \
	$(SDK_ROOT)/external/utf_converter \
	$(SDK_ROOT)/components/ble/ble_services/ble_bas_c \
	$(SDK_ROOT)/modules/nrfx/drivers/include \
	$(SDK_ROOT)/components/libraries/experimental_task_manager \
	$(SDK_ROOT)/components/ble/ble_services/ble_hrs_c \
	$(SDK_ROOT)/components/softdevice/s140/headers/nrf52 \
	$(SDK_ROOT)/components/nfc/ndef/connection_handover/le_oob_rec \
	$(SDK_ROOT)/components/libraries/queue \
	$(SDK_ROOT)/components/libraries/pwr_mgmt \
	$(SDK_ROOT)/components/ble/ble_dtm \
	$(SDK_ROOT)/components/toolchain/cmsis/include \
	$(SDK_ROOT)/components/ble/ble_services/ble_rscs_c \
	$(SDK_ROOT)/components/ble/common \
	$(SDK_ROOT)/components/ble/ble_services/ble_lls \
	$(SDK_ROOT)/components/nfc/platform \
	$(SDK_ROOT)/components/libraries/bsp \
	$(SDK_ROOT)/components/nfc/ndef/connection_handover/ac_rec \
	$(SDK_ROOT)/components/ble/ble_services/ble_bas \
	$(SDK_ROOT)/components/libraries/mpu \
	$(SDK_ROOT)/components/libraries/experimental_section_vars \
	$(SDK_ROOT)/components/ble/ble_services/ble_ans_c \
	$(SDK_ROOT)/components/libraries/slip \
	$(SDK_ROOT)/components/libraries/delay \
	$(SDK_ROOT)/components/libraries/memobj \
	$(SDK_ROOT)/components/ble/ble_services/ble_nus_c \
	$(SDK_ROOT)/components/softdevice/common \
	$(SDK_ROOT)/components/ble/ble_services/ble_ias \
	$(SDK_ROOT)/components/libraries/usbd/class/hid/mouse \
	$(SDK_ROOT)/components/libraries/low_power_pwm \
	$(SDK_ROOT)/components/nfc/ndef/conn_hand_parser/ble_oob_advdata_parser \
	$(SDK_ROOT)/components/ble/ble_services/ble_dfu \
	$(SDK_ROOT)/external/fprintf \
	$(SDK_ROOT)/components/libraries/svc \
	$(SDK_ROOT)/components/libraries/atomic \
	$(SDK_ROOT)/components \
	$(SDK_ROOT)/components/libraries/scheduler \
	$(SDK_ROOT)/components/libraries/cli \
	$(SDK_ROOT)/components/libraries/crc16 \
	$(SDK_ROOT)/components/nfc/t4t_parser/apdu \
	$(SDK_ROOT)/components/libraries/util \
	$(PROJ_DIR)/config \
	$(PROJ_DIR)/HM01B0 \
	$(PROJ_DIR)/bmi270 \
	$(PROJ_DIR)/ \
	$(PROJ_DIR)/sdk_patch \
	$(SDK_ROOT)/components/libraries/balloc \
	$(SDK_ROOT)/components/libraries/ecc \
	$(SDK_ROOT)/components/libraries/hardfault \
	$(SDK_ROOT)/components/ble/ble_services/ble_cscs \
	$(SDK_ROOT)/components/libraries/hci \
	$(SDK_ROOT)/components/libraries/timer \
	$(SDK_ROOT)/components/softdevice/s140/headers \
	$(SDK_ROOT)/integration/nrfx \
	$(SDK_ROOT)/components/nfc/t4t_parser/tlv \
	$(SDK_ROOT)/components/libraries/sortlist \
	$(SDK_ROOT)/components/libraries/spi_mngr \
	$(SDK_ROOT)/components/libraries/led_softblink \
	$(SDK_ROOT)/components/nfc/ndef/conn_hand_parser \
	$(SDK_ROOT)/components/libraries/sdcard \
	$(SDK_ROOT)/components/nfc/ndef/parser/record \
	$(SDK_ROOT)/modules/nrfx/mdk \
	$(SDK_ROOT)/components/ble/ble_services/ble_cts_c \
	$(SDK_ROOT)/components/ble/ble_services/ble_nus \
	$(SDK_ROOT)/components/libraries/twi_mngr \
	$(SDK_ROOT)/components/ble/ble_services/ble_hids \
	$(SDK_ROOT)/components/libraries/strerror \
	$(SDK_ROOT)/components/libraries/crc32 \
	$(SDK_ROOT)/components/nfc/ndef/connection_handover/ble_oob_advdata \
	$(SDK_ROOT)/components/nfc/t2t_parser \
	$(SDK_ROOT)/components/nfc/ndef/connection_handover/ble_pair_msg \
	$(SDK_ROOT)/components/libraries/usbd/class/audio \
	$(SDK_ROOT)/components/libraries/sensorsim \
	$(SDK_ROOT)/components/nfc/t4t_lib \
	$(SDK_ROOT)/components/ble/peer_manager \
	$(SDK_ROOT)/components/libraries/mem_manager \
	$(SDK_ROOT)/components/libraries/ringbuf \
	$(SDK_ROOT)/components/ble/ble_services/ble_tps \
	$(SDK_ROOT)/components/nfc/ndef/parser/message \
	$(SDK_ROOT)/components/ble/ble_services/ble_dis \
	$(SDK_ROOT)/components/nfc/ndef/uri \
	$(SDK_ROOT)/components/ble/nrf_ble_gatt \
	$(SDK_ROOT)/components/ble/nrf_ble_qwr \
	$(SDK_ROOT)/components/libraries/gfx \
	$(SDK_ROOT)/components/libraries/button \
	$(SDK_ROOT)/modules/nrfx \
	$(SDK_ROOT)/components/libraries/twi_sensor \
	$(SDK_ROOT)/integration/nrfx/legacy \
	$(SDK_ROOT)/components/libraries/usbd/class/hid/kbd \
	$(SDK_ROOT)/components/nfc/ndef/connection_handover/ep_oob_rec \
	$(SDK_ROOT)/external/segger_rtt \
	$(SDK_ROOT)/components/libraries/atomic_fifo \
	$(SDK_ROOT)/components/ble/ble_services/ble_lbs_c \
	$(SDK_ROOT)/components/nfc/ndef/connection_handover/ble_pair_lib \
	$(SDK_ROOT)/components/libraries/crypto \
	$(SDK_ROOT)/components/ble/ble_racp \
	$(SDK_ROOT)/components/libraries/fds \
	$(SDK_ROOT)/components/nfc/ndef/launchapp \
	$(SDK_ROOT)/components/libraries/atomic_flags \
	$(SDK_ROOT)/components/ble/ble_services/ble_hrs \
	$(SDK_ROOT)/components/ble/ble_services/ble_rscs \
	$(SDK_ROOT)/components/nfc/ndef/connection_handover/hs_rec \
	$(SDK_ROOT)/components/libraries/usbd \
	$(SDK_ROOT)/components/nfc/ndef/conn_hand_parser/ac_rec_parser \
	$(SDK_ROOT)/components/libraries/stack_guard \
	$(SDK_ROOT)/components/libraries/cli/libuarte \
	$(SDK_ROOT)/components/libraries/libuarte \
	$(SDK_ROOT)/components/libraries/ringbuf \
	$(SDK_ROOT)/components/libraries/log/src \
	$(SDK_ROOT)/modules/nrfx/drivers/src/prs \

# Libraries common to all targets
LIB_FILES += \
	$(SDK_ROOT)/components/toolchain/cmsis/dsp/GCC/libarm_cortexM4lf_math.a \

# Optimization flags
OPT = -O3 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# Dev Kit BSP file
banji_dev: CFLAGS += -DBOARD_PCA10056
banji_dev: ASMFLAGS += -DBOARD_PCA10056

# FF Kit BSP file
banji: CFLAGS += -DBOARD_CUSTOM
banji: ASMFLAGS += -DBOARD_CUSTOM

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DARM_MATH_CM4
CFLAGS += -DAPP_TIMER_V2
CFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DNRF52840_XXAA
CFLAGS += -DNRF_SD_BLE_API_VERSION=7
CFLAGS += -DS140
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-unused-variable
# CFLAGS += -DBSP_DEFINES_ONLY
# CFLAGS += -DNRF52
# CFLAGS += -DNRF52_PAN_74

# C++ flags common to all targets
CXXFLAGS += $(OPT)
# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DARM_MATH_CM4
ASMFLAGS += -DAPP_TIMER_V2
ASMFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF52840_XXAA
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=7
ASMFLAGS += -DS140
ASMFLAGS += -DSOFTDEVICE_PRESENT
# ASMFLAGS += -DNRF52
# ASMFLAGS += -DBSP_DEFINES_ONLY
# ASMFLAGS += -DNRF52_PAN_74

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys
LDFLAGS += -u _printf_float

banji_dev: CFLAGS += -D__HEAP_SIZE=8192
banji_dev: CFLAGS += -D__STACK_SIZE=8192
banji_dev: ASMFLAGS += -D__HEAP_SIZE=8192
banji_dev: ASMFLAGS += -D__STACK_SIZE=8192

banji: CFLAGS += -D__HEAP_SIZE=8192
banji: CFLAGS += -D__STACK_SIZE=8192
banji: ASMFLAGS += -D__HEAP_SIZE=8192
banji: ASMFLAGS += -D__STACK_SIZE=8192

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm

.PHONY: default help

# Default target - first one defined
default: banji

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo   banji      - build form factor
	@echo		banji_dev  - build dev kit
	@echo		flash_softdevice - flash soft device
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing form factor binary
	@echo   flash_dev  - flash dev kit binary

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash flash_softdevice flash_dev erase

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/banji.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/banji.hex --sectorerase
	nrfjprog -f nrf52 --reset

flash_dev: banji_dev
	@echo Flashing: $(OUTPUT_DIRECTORY)/banji_dev.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/banji_dev.hex --sectorerase
	nrfjprog -f nrf52 --reset

# Flash softdevice
flash_softdevice:
	@echo Flashing: s140_nrf52_7.2.0_softdevice.hex
	nrfjprog -f nrf52 --program $(SDK_ROOT)/components/softdevice/s140/hex/s140_nrf52_7.2.0_softdevice.hex --sectorerase
	nrfjprog -f nrf52 --reset

erase:
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := $(PROJ_DIR)/config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)

log:
	osascript jlink-server.applescript
	JLinkRTTClient

log_server:
	JLinkExe -device NRF52 -speed 4000 -if SWD -AutoConnect 1
	# JLinkExe -device NRF52 -speed 4000 -if SWD -AutoConnect 1 > /dev/null &

log_client:
	JLinkRTTClient

.PHONY: gdb_client gdb_server gdb

gdb_client:
	@echo "target remote localhost:$(GDB_PORT)" > $(GDB_CMD_PATH)
	@echo "mon speed 10000" >> $(GDB_CMD_PATH)
	@echo "mon flash download=1" >> $(GDB_CMD_PATH)
	@echo "load $(OUTPUT_DIRECTORY)/$(TARGETS).out" >> $(GDB_CMD_PATH)
	@echo "break main" >> $(GDB_CMD_PATH)
	@echo "mon reset 0" >> $(GDB_CMD_PATH)
	@echo "c" >> $(GDB_CMD_PATH)

gdb_server:
	$(TERMINAL) $(GDBSERVER) -device nrf52832_XXAA -if swd -port $(GDB_PORT) $(TERMINAL_END)
	$(TERMINAL) $(GDB) $(CURDIR)/$(OUTPUT_DIRECTORY)/$(TARGETS).out -x $(CURDIR)/$(GDB_CMD_PATH) $(TERMINAL_END)

gdb: gdb_client gdb_server
