# Simple syhthesis script to use TSMC/45nm libraries
#
# 
#
file mkdir reports
file mkdir netlist

remove_design -all

define_design_lib RAST -path "RAST"

#####################
# Config Variables
#####################
# The clock input signal name.
set CLK  "clk"
# The reset input signal name.
set RST  "rst"

set DRIVER_CELL "INVD0BWP"
set DR_CELL_OUT "ZN"


#####################
# Path Variables
#####################
set SYN  /usr/class/ee/synopsys/data1/syn/2010.03/libraries/syn/
set TSMC_45 /usr/class/ee271/project/lib/TSMC_45nm/


#####################
# Set Design Library
#####################

#TSMC 45nm Library
set link_library { * tcbn45gsbwpml.db dw_foundation.sldb}
set target_library "tcbn45gsbwpml.db"
#set link_library { * tcbn45gsbwphvtml.db dw_foundation.sldb}
#set target_library "tcbn45gsbwphvtml.db"

set synthetic_library [list  dw_foundation.sldb]
set dw_lib     $SYN
set sym_lib    $TSMC_45
set target_lib $TSMC_45

#set tech_file 
#set mw_reference_library 
#set mw_lib_name 
#set max_tlu_file
#set min_tlu_file
#set prs_map_file

set search_path [list ./ ../rtl/  $dw_lib $target_lib $sym_lib]

#set mv_power_net VDD
#set mw_ground_net VSS
#set mw_logic1_net VDD
#set mw_logic0_net VSS
#set mw_power_port VDD
#set mw_ground_port VSS

#create_mw_lib -technology $tech_file \
#              -mw_reference_library $mw_reference_library \
#                                    $mw_lib_name
#open_mw_lib $mw_lib_name 

#report_mw_lib
#set_check_library_options -logic_vs_physical
#check_library

#set_tlu_plus_files -max_tluplus  $max_tlu_file \
#                   -min_tluplus  $min_tlu_file \
#                   -tech2itf_map $prs_map_file

#check_tlu_plus_files

###################
# Read Design
###################

analyze -library RAST -format sverilog [glob ${RUNDIR}/genesis_synth/*.v]
elaborate ${DESIGN_TARGET} -architecture verilog -library RAST


#################################
# Define Design Environment 
#################################
# go here

#################################
# Design Rule Constraints
#################################
# go here


##################################
# Design Optimization Constraints
##################################

# create clock
create_clock $CLK -period $CLK_PERIOD

# set output delay and load
set_fanout_load 4 [get_ports "*" -filter {@port_direction == out} ]
set_output_delay [ expr $CLK_PERIOD*3/4 ] -clock $CLK  [get_ports "*" -filter {@port_direction == out} ]
#set_output_delay [ expr $CLK_PERIOD*1/2 ] -clock $CLK halt_RnnnnL

set_wire_load_selection WireAreaLowkCon
set_wire_load_mode top

set_max_fanout 4.0 [get_ports "*" -filter {@port_direction != out} ]


# set input delay on all input ports except 'clk' and 'rst'
set all_inputs_wo_rst_clk [remove_from_collection [remove_from_collection [all_inputs] [get_port $CLK]] [get_port $RST]]
set_input_delay -clock $CLK [ expr $CLK_PERIOD*3/4 ] $all_inputs_wo_rst_clk
set_driving_cell -lib_cell $DRIVER_CELL -pin $DR_CELL_OUT [ get_ports "*" -filter {@port_direction == in} ]

# set no input delay on control ports
set_input_delay -clock $CLK 0 screen_RnnnnS
set_input_delay -clock $CLK 0 subSample_RnnnnU

# set target die area
set_max_area $TARGET_AREA

# set DC don't touch reset network
remove_driving_cell $RST
set_drive 0 $RST
set_dont_touch_network $RST


##########################################
# Synthesize Design (Optimize for Timing)
##########################################
#set power analysis
#set_power_prediction
set_optimize_registers true -design ${DESIGN_TARGET}
compile_ultra -retime

##########################
# Analyze Design 
##########################
redirect "reports/design_report" { report_design }
check_design
redirect "reports/design_check" {check_design }
#report_constraint -all_violators
#redirect "reports/constraint_report" {report_constraint -all_violators}
report_area 
redirect "reports/area_report" { report_area }
#redirect "reports/area_hier_report" { report_area -hier }
report_power
redirect "reports/power_report" { report_power -analysis_effort hi }
#redirect "reports/power_hier_report" { report_power -hier }
#report_timing -path full -delay max -max_paths 10
#redirect "report/timing_report_max" { report_timing -path full -delay max -max_paths 200 }
#report_timing -path full -delay min -max_paths 10
report_timing
redirect "reports/timing_report_maxsm" { report_timing -significant_digits 4 }
#redirect "reports/timing_report_min" { report_timing -path full -delay min -max_paths 50 }
#report_timing_requirements
#redirect "reports/timing_requirements_report" { report_timing_requirements }
report_qor
redirect "reports/qor_report" { report_qor }
check_error
redirect "reports/error_checking_report" { check_error }




###################################
# Save the Design DataBase
###################################
write_sdf -version 2.1 "netlist/sdf_rasterizer"
write -hierarchy -format verilog -output "netlist/rast.gv"
write -format verilog -hier -o "netlist/rast.psv"
write -format ddc -hierarchy -output "rast.mapped.ddc"

#Concise Results in a singular file
echo $CLK_PERIOD >> results.txt
sh cat reports/error_checking_report >> results.txt
sh grep -i slack reports/timing_report_maxsm >> results.txt
sh cat reports/power_report | grep Total >> results.txt
sh cat reports/power_report | grep Cell | grep Leakage >> results.txt
sh cat reports/area_report | grep Total | grep cell >> results.txt

exit 





