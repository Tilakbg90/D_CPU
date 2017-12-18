#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-cpu.mk)" "nbproject/Makefile-local-cpu.mk"
include nbproject/Makefile-local-cpu.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=cpu
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/cpu.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/cpu.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../src/AES_DS.c ../src/AES_US.c ../src/AXLE_MON.c ../src/COMM1_FDP.c ../src/COMM_DS.c ../src/COMM_SM.c ../src/COMM_US.c ../src/CRC16.c ../src/CRC32.c ../src/DAC_MAIN.c ../src/ERROR.c ../src/RELAYDRV.c ../src/RELAYMGR.c ../src/RESET.c ../src/RESTORE.c ../src/RLYA_MGR.c ../src/RLYB_MGR.c ../src/RLYD3_MGR.c ../src/SYS_MON.c ../src/AES.c ../src/AXLE_MON_LCWS_1.c ../src/AXLE_MON_LCWS_2.c ../src/AXLE_MON_LCWS_3.c ../src/AXLE_MON_LCWS_4.c ../src/RLYD4_MGR.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1360937237/AES_DS.o ${OBJECTDIR}/_ext/1360937237/AES_US.o ${OBJECTDIR}/_ext/1360937237/AXLE_MON.o ${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o ${OBJECTDIR}/_ext/1360937237/COMM_DS.o ${OBJECTDIR}/_ext/1360937237/COMM_SM.o ${OBJECTDIR}/_ext/1360937237/COMM_US.o ${OBJECTDIR}/_ext/1360937237/CRC16.o ${OBJECTDIR}/_ext/1360937237/CRC32.o ${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o ${OBJECTDIR}/_ext/1360937237/ERROR.o ${OBJECTDIR}/_ext/1360937237/RELAYDRV.o ${OBJECTDIR}/_ext/1360937237/RELAYMGR.o ${OBJECTDIR}/_ext/1360937237/RESET.o ${OBJECTDIR}/_ext/1360937237/RESTORE.o ${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o ${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o ${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o ${OBJECTDIR}/_ext/1360937237/SYS_MON.o ${OBJECTDIR}/_ext/1360937237/AES.o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o ${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1360937237/AES_DS.o.d ${OBJECTDIR}/_ext/1360937237/AES_US.o.d ${OBJECTDIR}/_ext/1360937237/AXLE_MON.o.d ${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o.d ${OBJECTDIR}/_ext/1360937237/COMM_DS.o.d ${OBJECTDIR}/_ext/1360937237/COMM_SM.o.d ${OBJECTDIR}/_ext/1360937237/COMM_US.o.d ${OBJECTDIR}/_ext/1360937237/CRC16.o.d ${OBJECTDIR}/_ext/1360937237/CRC32.o.d ${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o.d ${OBJECTDIR}/_ext/1360937237/ERROR.o.d ${OBJECTDIR}/_ext/1360937237/RELAYDRV.o.d ${OBJECTDIR}/_ext/1360937237/RELAYMGR.o.d ${OBJECTDIR}/_ext/1360937237/RESET.o.d ${OBJECTDIR}/_ext/1360937237/RESTORE.o.d ${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o.d ${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o.d ${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o.d ${OBJECTDIR}/_ext/1360937237/SYS_MON.o.d ${OBJECTDIR}/_ext/1360937237/AES.o.d ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o.d ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o.d ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o.d ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o.d ${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1360937237/AES_DS.o ${OBJECTDIR}/_ext/1360937237/AES_US.o ${OBJECTDIR}/_ext/1360937237/AXLE_MON.o ${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o ${OBJECTDIR}/_ext/1360937237/COMM_DS.o ${OBJECTDIR}/_ext/1360937237/COMM_SM.o ${OBJECTDIR}/_ext/1360937237/COMM_US.o ${OBJECTDIR}/_ext/1360937237/CRC16.o ${OBJECTDIR}/_ext/1360937237/CRC32.o ${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o ${OBJECTDIR}/_ext/1360937237/ERROR.o ${OBJECTDIR}/_ext/1360937237/RELAYDRV.o ${OBJECTDIR}/_ext/1360937237/RELAYMGR.o ${OBJECTDIR}/_ext/1360937237/RESET.o ${OBJECTDIR}/_ext/1360937237/RESTORE.o ${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o ${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o ${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o ${OBJECTDIR}/_ext/1360937237/SYS_MON.o ${OBJECTDIR}/_ext/1360937237/AES.o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o ${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o

# Source Files
SOURCEFILES=../src/AES_DS.c ../src/AES_US.c ../src/AXLE_MON.c ../src/COMM1_FDP.c ../src/COMM_DS.c ../src/COMM_SM.c ../src/COMM_US.c ../src/CRC16.c ../src/CRC32.c ../src/DAC_MAIN.c ../src/ERROR.c ../src/RELAYDRV.c ../src/RELAYMGR.c ../src/RESET.c ../src/RESTORE.c ../src/RLYA_MGR.c ../src/RLYB_MGR.c ../src/RLYD3_MGR.c ../src/SYS_MON.c ../src/AES.c ../src/AXLE_MON_LCWS_1.c ../src/AXLE_MON_LCWS_2.c ../src/AXLE_MON_LCWS_3.c ../src/AXLE_MON_LCWS_4.c ../src/RLYD4_MGR.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

# The following macros may be used in the pre and post step lines
Device=PIC24FJ256GB210
ProjectDir="F:\Personal\SSDAC_N\Main_CPU\Software\Root\Trunk\cpu.X"
ConfName=cpu
ImagePath="dist\cpu\${IMAGE_TYPE}\cpu.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"
ImageDir="dist\cpu\${IMAGE_TYPE}"
ImageName="cpu.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IsDebug="true"
else
IsDebug="false"
endif

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-cpu.mk dist/${CND_CONF}/${IMAGE_TYPE}/cpu.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
	@echo "--------------------------------------"
	@echo "User defined post-build step: ["E:\Microchip\MPLABX\v3.65_R\mplab_ide\bin\hexmate.exe" --edf="E:\Microchip\MPLABX\v3.65_R\mplab_ide\dat\en_msgs.txt" ${ImagePath} -ADDRESSING=2 +-CK=0x0000-0x2ABEF@0x2ABF0w4g5p0x04C11DB7t000000 -O${ImagePath}]"
	@"E:\Microchip\MPLABX\v3.65_R\mplab_ide\bin\hexmate.exe" --edf="E:\Microchip\MPLABX\v3.65_R\mplab_ide\dat\en_msgs.txt" ${ImagePath} -ADDRESSING=2 +-CK=0x0000-0x2ABEF@0x2ABF0w4g5p0x04C11DB7t000000 -O${ImagePath}
	@echo "--------------------------------------"

MP_PROCESSOR_OPTION=24FJ256GB210
MP_LINKER_FILE_OPTION=,--script="..\src\linker_script.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1360937237/AES_DS.o: ../src/AES_DS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AES_DS.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AES_DS.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AES_DS.c  -o ${OBJECTDIR}/_ext/1360937237/AES_DS.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AES_DS.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AES_DS.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/AES_US.o: ../src/AES_US.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AES_US.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AES_US.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AES_US.c  -o ${OBJECTDIR}/_ext/1360937237/AES_US.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AES_US.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AES_US.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/AXLE_MON.o: ../src/AXLE_MON.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AXLE_MON.c  -o ${OBJECTDIR}/_ext/1360937237/AXLE_MON.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AXLE_MON.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AXLE_MON.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o: ../src/COMM1_FDP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/COMM1_FDP.c  -o ${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/COMM_DS.o: ../src/COMM_DS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM_DS.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM_DS.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/COMM_DS.c  -o ${OBJECTDIR}/_ext/1360937237/COMM_DS.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/COMM_DS.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/COMM_DS.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/COMM_SM.o: ../src/COMM_SM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM_SM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM_SM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/COMM_SM.c  -o ${OBJECTDIR}/_ext/1360937237/COMM_SM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/COMM_SM.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/COMM_SM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/COMM_US.o: ../src/COMM_US.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM_US.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM_US.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/COMM_US.c  -o ${OBJECTDIR}/_ext/1360937237/COMM_US.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/COMM_US.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/COMM_US.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/CRC16.o: ../src/CRC16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/CRC16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/CRC16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/CRC16.c  -o ${OBJECTDIR}/_ext/1360937237/CRC16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/CRC16.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/CRC16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/CRC32.o: ../src/CRC32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/CRC32.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/CRC32.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/CRC32.c  -o ${OBJECTDIR}/_ext/1360937237/CRC32.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/CRC32.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/CRC32.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o: ../src/DAC_MAIN.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DAC_MAIN.c  -o ${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/ERROR.o: ../src/ERROR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ERROR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ERROR.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ERROR.c  -o ${OBJECTDIR}/_ext/1360937237/ERROR.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/ERROR.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/ERROR.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RELAYDRV.o: ../src/RELAYDRV.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RELAYDRV.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RELAYDRV.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RELAYDRV.c  -o ${OBJECTDIR}/_ext/1360937237/RELAYDRV.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RELAYDRV.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RELAYDRV.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RELAYMGR.o: ../src/RELAYMGR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RELAYMGR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RELAYMGR.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RELAYMGR.c  -o ${OBJECTDIR}/_ext/1360937237/RELAYMGR.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RELAYMGR.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RELAYMGR.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RESET.o: ../src/RESET.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RESET.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RESET.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RESET.c  -o ${OBJECTDIR}/_ext/1360937237/RESET.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RESET.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RESET.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RESTORE.o: ../src/RESTORE.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RESTORE.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RESTORE.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RESTORE.c  -o ${OBJECTDIR}/_ext/1360937237/RESTORE.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RESTORE.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RESTORE.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o: ../src/RLYA_MGR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RLYA_MGR.c  -o ${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o: ../src/RLYB_MGR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RLYB_MGR.c  -o ${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o: ../src/RLYD3_MGR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RLYD3_MGR.c  -o ${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/SYS_MON.o: ../src/SYS_MON.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/SYS_MON.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/SYS_MON.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/SYS_MON.c  -o ${OBJECTDIR}/_ext/1360937237/SYS_MON.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/SYS_MON.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/SYS_MON.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/AES.o: ../src/AES.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AES.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AES.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AES.c  -o ${OBJECTDIR}/_ext/1360937237/AES.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AES.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AES.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o: ../src/AXLE_MON_LCWS_1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AXLE_MON_LCWS_1.c  -o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o: ../src/AXLE_MON_LCWS_2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AXLE_MON_LCWS_2.c  -o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o: ../src/AXLE_MON_LCWS_3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AXLE_MON_LCWS_3.c  -o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o: ../src/AXLE_MON_LCWS_4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AXLE_MON_LCWS_4.c  -o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o: ../src/RLYD4_MGR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RLYD4_MGR.c  -o ${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1360937237/AES_DS.o: ../src/AES_DS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AES_DS.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AES_DS.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AES_DS.c  -o ${OBJECTDIR}/_ext/1360937237/AES_DS.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AES_DS.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AES_DS.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/AES_US.o: ../src/AES_US.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AES_US.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AES_US.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AES_US.c  -o ${OBJECTDIR}/_ext/1360937237/AES_US.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AES_US.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AES_US.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/AXLE_MON.o: ../src/AXLE_MON.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AXLE_MON.c  -o ${OBJECTDIR}/_ext/1360937237/AXLE_MON.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AXLE_MON.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AXLE_MON.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o: ../src/COMM1_FDP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/COMM1_FDP.c  -o ${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/COMM1_FDP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/COMM_DS.o: ../src/COMM_DS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM_DS.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM_DS.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/COMM_DS.c  -o ${OBJECTDIR}/_ext/1360937237/COMM_DS.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/COMM_DS.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/COMM_DS.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/COMM_SM.o: ../src/COMM_SM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM_SM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM_SM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/COMM_SM.c  -o ${OBJECTDIR}/_ext/1360937237/COMM_SM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/COMM_SM.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/COMM_SM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/COMM_US.o: ../src/COMM_US.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM_US.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/COMM_US.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/COMM_US.c  -o ${OBJECTDIR}/_ext/1360937237/COMM_US.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/COMM_US.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/COMM_US.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/CRC16.o: ../src/CRC16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/CRC16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/CRC16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/CRC16.c  -o ${OBJECTDIR}/_ext/1360937237/CRC16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/CRC16.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/CRC16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/CRC32.o: ../src/CRC32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/CRC32.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/CRC32.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/CRC32.c  -o ${OBJECTDIR}/_ext/1360937237/CRC32.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/CRC32.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/CRC32.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o: ../src/DAC_MAIN.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DAC_MAIN.c  -o ${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/DAC_MAIN.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/ERROR.o: ../src/ERROR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ERROR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ERROR.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ERROR.c  -o ${OBJECTDIR}/_ext/1360937237/ERROR.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/ERROR.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/ERROR.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RELAYDRV.o: ../src/RELAYDRV.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RELAYDRV.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RELAYDRV.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RELAYDRV.c  -o ${OBJECTDIR}/_ext/1360937237/RELAYDRV.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RELAYDRV.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RELAYDRV.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RELAYMGR.o: ../src/RELAYMGR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RELAYMGR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RELAYMGR.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RELAYMGR.c  -o ${OBJECTDIR}/_ext/1360937237/RELAYMGR.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RELAYMGR.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RELAYMGR.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RESET.o: ../src/RESET.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RESET.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RESET.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RESET.c  -o ${OBJECTDIR}/_ext/1360937237/RESET.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RESET.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RESET.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RESTORE.o: ../src/RESTORE.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RESTORE.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RESTORE.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RESTORE.c  -o ${OBJECTDIR}/_ext/1360937237/RESTORE.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RESTORE.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RESTORE.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o: ../src/RLYA_MGR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RLYA_MGR.c  -o ${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RLYA_MGR.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o: ../src/RLYB_MGR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RLYB_MGR.c  -o ${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RLYB_MGR.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o: ../src/RLYD3_MGR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RLYD3_MGR.c  -o ${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RLYD3_MGR.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/SYS_MON.o: ../src/SYS_MON.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/SYS_MON.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/SYS_MON.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/SYS_MON.c  -o ${OBJECTDIR}/_ext/1360937237/SYS_MON.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/SYS_MON.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/SYS_MON.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/AES.o: ../src/AES.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AES.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AES.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AES.c  -o ${OBJECTDIR}/_ext/1360937237/AES.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AES.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AES.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o: ../src/AXLE_MON_LCWS_1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AXLE_MON_LCWS_1.c  -o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o: ../src/AXLE_MON_LCWS_2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AXLE_MON_LCWS_2.c  -o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o: ../src/AXLE_MON_LCWS_3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AXLE_MON_LCWS_3.c  -o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o: ../src/AXLE_MON_LCWS_4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AXLE_MON_LCWS_4.c  -o ${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/AXLE_MON_LCWS_4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o: ../src/RLYD4_MGR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RLYD4_MGR.c  -o ${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/RLYD4_MGR.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/cpu.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../src/linker_script.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/cpu.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  --fill=0xFFDEAD  -mreserve=data@0x800:0x81B -mreserve=data@0x81C:0x81D -mreserve=data@0x81E:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x827 -mreserve=data@0x82A:0x84F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--no-handles,--no-isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml,--no-ivt$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/cpu.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../src/linker_script.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/cpu.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_cpu=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  --fill=0xFFDEAD -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--no-handles,--no-isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml,--no-ivt$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/cpu.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/cpu
	${RM} -r dist/cpu

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
