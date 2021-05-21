After extracting the original CppSim from http://www.cppsim.com, the following changes were made to trim it down to CppSimLite

# Changes

**Deleted**:
- CppSimLite/CppSimShared/CommonCode
- CppSimLite/CppSimShared/GTKWave
- CppSimLite/CppSimShared/IVerilog
- CppSimLite/CppSimShared/NGspice
- CppSimLite/CppSimShared/CadenceLib
- CppSimLite/CppSimShared/Verilator
- CppSimLite/CadenceLib
- CppSimLite/Wine

**Moved**:
- cppsim_bshrc_file_example -> CppSimLite/.

**Added**:
- CppSimLite/CppSimShared/Doc/BSIM464_Manual.pdf
- CppSimLite/CppSimShared/Doc/ngspice-32-manual.pdf
- CppSimLite/CppSimShared/Doc/ngspice-manual.pdf (soft link)
- CppSimLite/CppSimShared/Doc/scmos.pdf


**FILE CHANGES**

- **$CPPSIMSHAREDHOME/Sue2/src/sue.tcl**
    - 43: global SPICE_BIN_PATH
    - 45: set SPICE_BIN_PATH $::env(SPICE_EXEC_DIR)

- **$CPPSIMSHAREDHOME/Sue2/src/runNGspiceForm.tcl**
    - 5,93,177,206,269,375,426,479,566: global SPICE_BIN_PATH 
    - 106,319: set ngspice_install_dir "$SPICE_BIN_PATH/.."

- **$CPPSIMSHAREDHOME/Sue2/src/windows.tcl**
    - 50: global SPICE_BIN_PATH 
    - 167,192: Removed unwanted docs and updated NGSpice doc doc to Doc in the "Doc" submenu.
    - 515,520: Removed "CppSim/VppSim" from the "Toools" submenu

- **$CPPSIMHOME/Sue2/sue.lib**
    - Removed the unwanted Libraries
    - Created a new Lib "myLib" using the Library Manager and
    - Changed the order to myLib, Spice, devices, etc.
    - Added CppSimModule and GMSK_examples because of warnings during netlist and Library Manager operations.
