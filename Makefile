################################################################################
################ Makefile Definitions
################################################################################
# This little trick finds where the makefile exists
DESIGN_HOME := $(realpath $(dir $(word $(words $(MAKEFILE_LIST)), $(MAKEFILE_LIST))))
$(warning WARNING: RAST home set to $(DESIGN_HOME)) 

# RUNDIR is where we are actually running
RUNDIR := $(realpath ./)
$(warning WARNING: RUNDIR set to $(RUNDIR)) 

# this line enables a local Makefile to override values of the main makefile
-include Makefile.local

########### Generic Env Defs ############
#########################################


ifndef DESIGN_NAME
  DESIGN_NAME:=rast
  $(warning WARNING: Running with default design.  DESIGN_NAME=$(DESIGN_NAME))
endif

ifeq ($(DESIGN_NAME), rast)
  SRC_DIR := $(DESIGN_HOME)
  INST_NAME ?= rast
  MOD_NAME ?=  rast
  TOP_NAME ?= top_rast
  GENESIS_SYNTH_TOP_PATH = $(TOP_NAME).$(INST_NAME)
endif

TOP_MODULE ?= $(TOP_NAME)

############# For Genesis2 ##############
#########################################

# list src folders and include folders
GENESIS_SRC := 	-srcpath ./			\
		-srcpath $(DESIGN_HOME)/rtl	\
		-srcpath $(DESIGN_HOME)/verif			

GENESIS_INC := 	-incpath ./			\
		-incpath $(DESIGN_HOME)/rtl	\
		-incpath $(DESIGN_HOME)/verif



# vpath directive tells where to search for *.vp and *.vph files
vpath 	%.vp  $(GENESIS_SRC)
vpath 	%.svp  $(GENESIS_SRC)
vpath 	%.vph $(GENESIS_INC)
vpath 	%.svph $(GENESIS_INC)
vpath	%.c	$(C_SRC)


GENESIS_ENV :=		$(wildcard $(DESIGN_HOME)/verif/*.vp) $(wildcard $(DESIGN_HOME)/verif/*.svp)
#GENESIS_ENV :=		$(DESIGN_HOME)/verif/top_rast.vp
GENESIS_ENV :=		$(notdir $(GENESIS_ENV)) 

GENESIS_DESIGN := 	$(wildcard $(DESIGN_HOME)/rtl/*.vp) $(wildcard $(DESIGN_HOME)/rtl/*.svp)
GENESIS_DESIGN := 	$(notdir $(GENESIS_DESIGN))

GENESIS_INPUTS :=	$(GENESIS_PRIMITIVES) $(GENESIS_ENV) $(GENESIS_DESIGN) 




# debug level
GENESIS_DBG_LEVEL := 0

# List of generated verilog files
GENESIS_VLOG_LIST := genesis_vlog.vf
GENESIS_SYNTH_LIST := $(GENESIS_VLOG_LIST:%.vf=%.synth.vf)
GENESIS_VERIF_LIST := $(GENESIS_VLOG_LIST:%.vf=%.verif.vf)

# xml hierarchy file
ifndef GENESIS_HIERARCHY
GENESIS_HIERARCHY := $(MOD_NAME).xml
else
  $(warning WARNING: GENESIS_HIERARCHY set to $(GENESIS_HIERARCHY))
endif
GENESIS_TMP_HIERARCHY := $(MOD_NAME)_target.xml
# For more Genesis parsing options, type 'Genesis2.pl -help'
#        [-parse]                    ---   should we parse input file to generate perl modules?
#        [-sources|srcpath dir]      ---   Where to find source files
#        [-includes|incpath dir]     ---   Where to find included files
#        [-input file1 .. filen]     ---   List of top level files
#                                    ---   The default is STDIN, but some functions
#                                    ---   (such as "for" or "while")
#                                    ---   may not work properly.
#        [-perl_modules modulename]  ---   Additional perl modules to load
GENESIS_PARSE_FLAGS := 	-parse $(GENESIS_SRC) $(GENESIS_INC)	-input $(GENESIS_INPUTS)		

# For more Genesis parsing options, type 'Genesis2.pl -help'
#        [-generate]                 ---   should we generate a verilog hierarchy?
#        [-top topmodule]            ---   Name of top module to start generation from
#        [-depend filename]          ---   Should Genesis2 generate a dependency file list? (list of input files)
#        [-product filename]         ---   Should Genesis2 generate a product file list? (list of output files)
#        [-hierarchy filename]       ---   Should Genesis2 generate a hierarchy representation tree?
#        [-xml filename]             ---   Input XML representation of definitions
GENESIS_GEN_FLAGS :=	-gen -top $(TOP_MODULE)					\
                        -synthtop $(GENESIS_SYNTH_TOP_PATH)                     \
			-depend depend.list					\
			-product $(GENESIS_VLOG_LIST)				\
			-hierarchy $(GENESIS_HIERARCHY)                		


# Input xml/cfg files, input parameters
GENESIS_CFG_XML	:= empty.xml
GENESIS_CFG_SCRIPT	:=
GENESIS_PARAMS	:=
ifneq ($(strip $(GENESIS_CFG_SCRIPT)),)
  GENESIS_GEN_FLAGS	:= $(GENESIS_GEN_FLAGS) -cfg $(GENESIS_CFG_SCRIPT)
  $(warning WARNING: GENESIS_CFG_SCRIPT set to $(GENESIS_CFG_SCRIPT))
endif
ifneq ($(strip $(GENESIS_CFG_XML)),)
  GENESIS_GEN_FLAGS	:= $(GENESIS_GEN_FLAGS) -xml $(GENESIS_CFG_XML)
  $(warning WARNING: GENESIS_CFG_XML set to $(GENESIS_CFG_XML))
endif
ifneq ($(strip $(GENESIS_PARAMS)),)
  GENESIS_GEN_FLAGS	:= $(GENESIS_GEN_FLAGS) -parameter $(GENESIS_PARAMS)
  $(warning WARNING: GENESIS_PARAMS set to $(GENESIS_PARAMS))
endif



############### For Verilog ################
############################################

##### FLAGS FOR SYNOPSYS COMPILATION ####
COMPILER := vcs

EXECUTABLE := $(RUNDIR)/simv

VERILOG_ENV :=		 

VERILOG_DESIGN :=	

VERILOG_FILES :=  	$(VERILOG_ENV) $(VERILOG_DESIGN)


VERILOG_LIBS := 	-y $(RUNDIR) +incdir+$(RUNDIR)			\
					-y $(SYNOPSYS)/dw/sim_ver/		\
					+incdir+$(SYNOPSYS)/dw/sim_ver/	 \
					-y $(SYNOPSYS)/packages/gtech/src_ver/	\
					+incdir+$(SYNOPSYS)/packages/gtech/src_ver/ \



# "-sverilog" enables system verilog
# "+lint=PCWM" enables linting error messages
# "+libext+.v" specifies that library files (imported by the "-y" directive) ends with ".v"
# "-notice" used to get details when ports are coerced to inout
# "-full64" for 64 bit compilation and simulation
# "+v2k" for verilog 2001 constructs such as generate
# "-timescale=1ns/1ns" sets the time unit and time precision for the entire design
# "+noportcoerce" compile-time option to shut off the port coercion for the entire design
# "-top topModuleName" specifies the top module
# "-f verilogFiles.list" specifies a file that contains list of verilog files to compile

# for C DPI function
C_SRC := 		$(DESIGN_HOME)/gold/rastBBox_fix_sv.c
C_INC :=        $(DESIGN_HOME)/gold/rastBBox_fix_sv.h

C_OBJ :=		$(C_SRC:%.c=%.o)
C_INC_FLAG := 		-I$(DESIGN_HOME)/gold -I$(VCS_HOME)/include
C_COMP_FLAG :=		-c -lm -m64
# for C DPI function


VERILOG_COMPILE_FLAGS := 	-sverilog 					\
				+cli 						\
				+lint=PCWM					\
				+libext+.v					\
				-notice						\
				-full64						\
				+v2k						\
				-debug_pp					\
				-timescale=1ns/1ns				\
				+noportcoerce         				\
				-ld $(VCS_CC) -debug_pp				\
				-top $(TOP_MODULE)				\
				-f $(GENESIS_VLOG_LIST) 			\
				$(VERILOG_FILES) $(VERILOG_LIBS) $(C_OBJ)

# "+vpdbufsize+100" limit the internal buffer to 100MB (forces flushing to disk)
# "+vpdports" Record information about ports (signal/in/out)
# "+vpdfileswitchsize+1000" limits the wave file to 1G (then switch to next file)
VERILOG_SIMULATION_FLAGS := 	$(VERILOG_SIMULATION_FLAGS) 			\
				-l $(EXECUTABLE).log				\
				+vpdbufsize+100					\
				+vpdfileswitchsize+100
##### END OF FLAGS FOR SYNOPSYS COMPILATION ####




############ For Design Compiler ###########
############################################
DESIGN_TARGET = rast

# The target clock period and area in library units (nS) (um^2). 45 
# you should be able to achieve 0.3ns clock rate by adjusting pipeline depths
CLK_PERIOD=0.5; 
TARGET_AREA=40000;


# flags for dc/icc
SYNTH_HOME	= $(DESIGN_HOME)/synth
SYNTH_RUNDIR	= $(RUNDIR)/synth
SYNTH_LOGS	= $(SYNTH_RUNDIR)
DC_LOG	= $(SYNTH_LOGS)/dc.log


SET_SYNTH_PARAMS = 	set DESIGN_HOME $(DESIGN_HOME); 	\
			set RUNDIR $(RUNDIR); 			\
			set DESIGN_TARGET $(DESIGN_TARGET); 	\
			set CLK_PERIOD  $(CLK_PERIOD); 				\
			set TARGET_AREA $(TARGET_AREA);	


DC_COMMAND_STRING = "$(SET_SYNTH_PARAMS) source -echo -verbose $(SYNTH_HOME)/rast_dc.tcl"


############ For CPP Gold Model ############
############################################

CPP_FLAGS = -Wall -O3 -g -lm -I$(DESIGN_HOME)/gold

GOLD_PROG = rast_gold

CPP_INC = $(DESIGN_HOME)/gold/helper.h \
		$(DESIGN_HOME)/gold/zbuff.h \
		$(DESIGN_HOME)/gold/rastBBox_fix.h

CPP_SRC = $(DESIGN_HOME)/gold/helper.cpp \
		$(DESIGN_HOME)/gold/zbuff.cpp \
		$(DESIGN_HOME)/gold/rastBBox_fix.cpp \
		$(DESIGN_HOME)/gold/rastTest.cpp   

CPP_OBJ = $(CPP_SRC:.cpp=.o)


################################################################################
################ Makefile Rules
################################################################################
#default rule: 
all: $(EXECUTABLE)

# Gold Model rules:
#####################
.PHONY: comp_gold clean_gold

comp_gold: $(GOLD_PROG)

$(CPP_OBJ): $(CPP_SRC) $(CPP_INC)
	cd $(DESIGN_HOME)/gold; g++ $(CPP_FLAGS) -c $(CPP_SRC)

$(GOLD_PROG) : $(CPP_OBJ)
	g++ $(CPP_FLAGS) $(CPP_OBJ) -o $(GOLD_PROG)

clean_gold :
	@echo ""
	@echo Cleanning previous gold model compile
	@echo ========================================
	rm -f $(GOLD_PROG) $(CPP_OBJ)

# Genesis2 rules:
#####################
# This is the rule to activate Genesis2 generator to generate verilog 
# files (_unqN.v) from the genesis (.vp) program.
# Use "make GEN=<genesis2_gen_flags>" to add elaboration time flags
.PHONY: gen genesis_clean
gen: $(GENESIS_VLOG_LIST) $(GENESIS_SYNTH_LIST) $(GENESIS_VERIF_LIST)

$(GENESIS_VLOG_LIST) $(GENESIS_SYNTH_LIST) $(GENESIS_VERIF_LIST) : $(GENESIS_INPUTS) $(GENESIS_CFG_XML)
	@echo ""
	@echo Making $@ because of $?
	@echo ==================================================
	Genesis2.pl $(GENESIS_GEN_FLAGS) $(GEN) $(GENESIS_PARSE_FLAGS) -input $(GENESIS_INPUTS) -debug $(GENESIS_DBG_LEVEL)



genesis_clean:
	@echo ""
	@echo Cleanning previous runs of Genesis
	@echo ===================================
	if test -f "genesis_clean.cmd"; then	\
		./genesis_clean.cmd;	\
	fi
	\rm -rf $(GENESIS_VLOG_LIST) $(GENESIS_SYNTH_LIST) $(GENESIS_VERIF_LIST)



# VCS rules:
#####################
# compile rules:
# use "make COMP=+define+<compile_time_flag[=value]>" to add compile time flags
.PHONY: comp
comp: $(EXECUTABLE)

$(C_OBJ): $(C_SRC) $(C_INC) 
	gcc $(C_INC_FLAG) $(C_COMP_FLAG) $(C_SRC) -o $(C_OBJ)

$(EXECUTABLE):	$(VERILOG_FILES) $(GENESIS_VLOG_LIST) $(C_OBJ)
	@echo ""
	@echo Making $@ because of $?
	@echo ==================================================
	$(COMPILER)  $(VERILOG_COMPILE_FLAGS) $(COMP) 2>&1 | tee comp_bb.log 




# Simulation rules:
#####################
# use "make run RUN=+<runtime_flag[=value]>" to add runtime flags
.PHONY: run  run_wave 
run_wave: $(EXECUTABLE)
	@echo ""
	@echo Now Running $(EXECUTABLE) with wave
	@echo ==================================================
	mkdir -p sb_log mon_log
	$(EXECUTABLE) +wave $(VERILOG_SIMULATION_FLAGS) $(RUN) 2>&1 | tee run_bb.log 

run: $(EXECUTABLE)
	@echo ""
	@echo Now Running $(EXECUTABLE)
	@echo ==================================================
	mkdir -p sb_log mon_log
	$(EXECUTABLE) $(VERILOG_SIMULATION_FLAGS) $(RUN) 2>&1 | tee run_bb.log

# DC & ICC Run rules:
############################
# DC Run
#######################
.PHONY: run_dc clean_dc clean_reports

run_dc: $(DC_LOG)

$(DC_LOG): $(GENESIS_SYNTH_LIST) $(SYNTH_HOME)/rast_dc.tcl
	@echo ""
	@echo Now Running DC SHELL: Making $@ because of $?
	@echo =============================================
	@sleep 1;
	@if test ! -d "$(SYNTH_LOGS)"; then 					\
		mkdir -p $(SYNTH_LOGS);						\
	fi
	cd $(SYNTH_RUNDIR); dc_shell-t -64bit -x $(DC_COMMAND_STRING) 2>&1 | tee -i $(DC_LOG)

clean_dc:
	@echo ""
	@echo Removing previous DC run log
	@echo =============================================
	\rm -f $(DC_LOG)

clean_reports:
	@echo ""
	@echo Removing previous DC reports
	@echo =============================================
	rm -fr $(SYNTH_RUNDIR)/reports
	rm -fr $(SYNTH_RUNDIR)/results.txt	
	cd $(SYNTH_RUNDIR); rm -fr alib-52 command.log default.svf netlist/
	cd $(SYNTH_RUNDIR); rm -fr cache/ db/ netlist/ RAST/ rast.mapped.ddc



# Cleanup rules:
#####################
.PHONY: clean cleanall 
clean: genesis_clean clean_dc clean_gold
	@echo ""
	@echo Cleanning old files, bineries and garbage
	@echo ==================================================
	\rm -f $(EXECUTABLE) 
	\rm -rf csrc
	\rm -rf *.daidir
	\rm -rf ucli.key
	\rm -rf gold/*.o
	\rm -f *.tmp
	\rm -f *.h
	\rm -rf $(EXECUTABLE).*


cleanall: clean clean_reports
	@echo ""
	@echo Cleanning old logs, images and waveforms
	@echo ==================================================
	\rm -rf DVE*
	\rm -rf vcdplus.vpd
	\rm -rf sb_log mon_log
	\rm -f *.log
	\rm -f *.ppm
