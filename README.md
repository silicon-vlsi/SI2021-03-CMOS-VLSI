# SI/2021-03: Digital CMOS VLSI Design
This github respository is for the Summer Internship Course conducted between May 17 till June 11, 2021. The course is conducted and taught by Mr. Puneet Mittal from VLSI Expert Pvt Ltd, Prof. Santunu Sarangi and Prof. Saroj Rout from Silicon Institute of Technology, Bhubaneswar.

## Announcements
- [21 May] : Use this [Link](https://us02web.zoom.us/meeting/register/tZAvcuyoqTgrHNwFF4Abu4TnOFCOn4WFVf1S) to register for Zoom sessions from May 24 till June 11, Mon-Fri: 10am-1pm, 2:3-5:30pm

## References/Resources
- [**Hodges**] Hodges, David A., and David. "*Analysis And Design Of Digital Integrated Circuits, In Deep Submicron Technology*" (Special Indian Edition, 3rd Ed). Tata McGraw-Hill Education, 2005.
- [**Kang**] Leblebici, Y., Chul W. K., and Sung-Mo (Steve) Kang. "*CMOS Digital Integrated Circuits Analysis & Design*". 4th ed. McGraw-Hill Education, 2014
- [**Weste**] Weste, Neil, and David Harris. "*CMOS VLSI Design: A Circuits and Systems Perspective*". Pearson Education, 2011


## Table of Content
- [NGSpice](#NGSpice)
    - [QuickStart Guide](#Quick-Start-Guide)
- [CppSimLite](#CppSimLite)
    - [Sue2](#Sue2)
- [Magic](#Magic)
- [Netgen](#Netgen)
- [Technology](#Technology)

## NGSpice
[NGSpice] is a open source spice simulator for electric and electronic circuits. 
- [NGSpice Reference Manual][NGSpiceMan]: Comple reference manual in HTML format.

Precompiled ngspice v32 is installed in `/project2020/eda/ngspice-32`. Add the following environment variables in your `~/.bashrc`

```bash
export  SPICE_LIB_DIR=/project2020/eda/ngspice-32/glnxa64/share/ngspice
export  SPICE_EXEC_DIR=/project2020/eda/ngspice-32/glnxa64/bin
export  PATH=$PATH:$SPICE_EXEC_DIR
```
There is a initialization script in `$SPICE_LIB_DIR/scripts/spinit`. You can overwrite any of the initilization by adding commands to a local `~/.spiceinit` .

### Quick Start Guide
You can open a text editor create a *netlist* of the intended circuit for example of a voltage divider as shown below (say filename `divider.sp`):
```spice
First line in ngspice is always the title line
* This is a comment line
Vbat    vin     0       DC 5
R1      vin     vout    1k
R2      vout    0       1k

.control
tran 0.1u 1u
.endc

.end
```
Then start `ngspice` and source the netlist at the ngspice command prompt:
```bash
ngspice 1 -> source divider.sp
```
It should output the node voltages at the initial transient voltages. you can plot any of the nodes eg.:
```bash
ngspice 2 -> plot v(vout)
```
If you want to edit the file without leaving ngspice, simply type edit eg.
```bash
ngspice 3 -> edit
```
**IMPORANT NOTE** While editing inside ngspice, if you make an error, you may lose the netlist file. This maybe a bug in ngspice.

The preferred method of running ngspice is in batch mode:
```bash
ngspice -b -r filename.raw -o filename.log input.sp
```
And to quit, simply type `quit`.

### Using the Python Library
[FIXME: Add relevant information]

## CppSimLite
**CppSimLite** is stripped down version of **CppSim**, (http://cppsim.com) developed by Mike Perrott for mixed-signal system and circuit modeling. Although CppSim is a suite of tools for doing mixed-signal simulation, CppSimLite is a stripped down version for using the schematic editor **Sue2** and it's accompanying toolboxes for *Python* and *HSPC*.

If you have cloned the git repo as explained before, you should have the following directory structure for the *CppSimLite*:
```bash
~/SI2021-03-CMOS-VLSI/tools/CppSimLite
├── CHANGES.md				;Changes made to CppSim
├── cppsim_bashrc_file_example		;example .bashrc 
├── CppSimShared
│   ├── bin
│   ├── Doc				;All documents kept here
│   ├── HspiceToolbox
│   ├── MatlabCode
│   ├── Python				;Python lib
│   ├── Sue2				;Sue2 scripts
│   └── SueLib				;All Sue2 Private Libs
├── Import_Export
├── Netlist				;Sue2 netlists resides here
├── SimRuns				;Sue2 NGSpice runs resides
├── SpiceModels				;**NOTE**NGspice models in ngspice
├── Sue2
├── SueLib				;Public Libraries
│   └── myLib
└── Todo-Bugs.md			;Keeping tracks of Bugs and Todos
```

- Setting the Environment Variables in `~/.bashrc`

```bash
export CPPSIMHOME=$HOME/SI2021-03-CMOS-VLSI/tools/CppSimLite
export CPPSIMSHAREDHOME=$CPPSIMHOME/CppSimShared
export EDITOR=/usr/bin/vim
export PATH=$PATH:$CPPSIMSHAREDHOME/bin
```
### Sue2
- Once the environment variables are set, Sue2 can be started by typing
```bash
sue2
```
- The schematic editor will launch with an empty canvas and 3 library panels on the right.
- The first panel on the top is for `schematic` only and the bottom two for symbols or icons to use in the schematic.
- You can choose what library to appear in each panel by clicking the the menu bar in the panel. The menu will show a list of the available Libraries stored in `$CPPSIMSHAREDHOME/SueLib`(Private Libs) and `$CPPSIMHOME/SueLib`(Piblic Libs) and the list and the order is loaded from `$CPPSIMHOME/Sue2/sue.lib`
- To select a schematic, use the cursor to select the schematic (eg. *invX1*) and then click **Shift-LeftMouseButton**. **NOTE** There is bug in *sue2* in Linux-LXLE distro where LeftMouseButton doesn't work. If you are working in any other Linux (eg. ubuntu) just LeftMouseButton works.
- You can create a netlist by clicking *Tools -> Create a netlist (with top sub)* and give a directory to save (default: *$CPPSIMHOME/Netlist*) **NOTE** While saving for the option *File Type* choose *All ()* Another bug which creates two .sp extensions otherwise.
- Now you can can write a Spice testbench and include and instatiate the above created netlist. There is alrady a example testbench in *$CPPSIMHOME/SimRuns/myLib/invX1/TB_invX1.sp*

## Magic
[Magic] is the most popular open-source Layout tool written in the 1980's at Berkeley by John Ousterhout (now famous for writing scripting languuage Tcl) and now maintained by Tim Edwards (opencircuitdesign.com/magic).\\

**Setting Up the Environment Variables**
**NOTE**: The below is path is an example. Just make sure it matches your particular path.
```bash
export MAGIC_HOME=/project2020/eda/magic-83
export CAD_ROOT=$MAGIC_HOME/lib
export PATH=$PATH:$MAGIC_HOME/bin
```

## Netgen
[Netgen] is a tool for comparing netlists, a process known as LVS, which stands for "Layout vs. Schematic". This is an important step in the integrated circuit design flow, ensuring that the geometry that has been laid out matches the expected circuit.
Netgen is currently maintained by Tim Edwards (opencircuitdesign.com/netgen)
**Setting Up the Environment Variables**
**NOTE**: The below is path is an example. Just make sure it matches your particular path.
```bash
export NETGEN_HOME=/project2020/eda/netgen-15
export PATH=$PATH:$NETGEN_HOME/bin
```

## MOSIS Scalable CMOS Technology ([SCMOS])
[SCMOS] is a *lambda-based* scalable design rules that can be interfaced to many CMOS fabrication process available at MOSIS. **NOTE** The scalable design rules does not interface with Fabs now because of lot unique process nuances.

- The Spice model files are located at `<PATH-TO-REPO>/project2020/eda/ngspice-32/models/scn4m_subm`
- Typical MOS parameters:
  - **NMOS**: tox=7.6nm, nch=1.7e17/cm^3, Vt0=0.49V, un(mobility)=445 cm^2/Vs
  - **PMOS**: tox=7.6nm, nch=1.7e17/cm^3, Vt0=-0.66V, up(mobility)=151 cm^2/Vs
  - Vdd=5V, Lmin=0.4um, Wmin=0.6um

## Skywater 130nm PDK
- https://www.github.com/google/skywater-pdk
- https://skywater-pdk.readthedocs.io/en/latest/


* * *

[OpenRAM]:              https://openram.soe.ucsc.edu/
[OpenRAMgit]:           https://github.com/VLSIDA/OpenRAM 
[OpenRAMpaper]:         https://ieeexplore.ieee.org/document/7827670/
[SCMOS]:                https://www.mosis.com/files/scmos/scmos.pdf
[NGSpice]:              http://ngspice.sourceforge.net
[NGSpiceMan]:           http://ngspice.sourceforge.net/docs/ngspice-html-manual/manual.xhtml
[Magic]:                http://opencircuitdesign.com/magic/
[Netgen]:               http://opencircuitdesign.com/netgen/
