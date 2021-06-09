# SI/2021-03: Digital CMOS VLSI Design
This github respository is for the **Summer Internship Course** conducted between May 17 till June 11, 2021. The course is conducted and taught by Mr. Puneet Mittal from VLSI Expert Pvt Ltd, Prof. Santunu Sarangi and Prof. Saroj Rout from Silicon Institute of Technology, Bhubaneswar.
### Course Content
- **Introduction Digital Design**: Digital VLSI Design FLow, MUX-based & Decoder-based circuits, Finite-State Machines (FSM), Flip-Flops (Setup/Hold) Time, Delays (Propagation/transition), Noise-margin, clock-gating and timing concepts.
- **Introduction to CMOS VLSI Design**: VLSI Design flow: custom & semi-custom, Introduction to CMOS Processing.
- **Review of Basic Circuit Theory**: Essential network theory: KCL/KVL, superposition, circuit elements, node equation method, Thevenin/Norton theorem, dynamic circuit analysis (RC).
- **Passive Integrated Circuit Devices**: Resistors, Capacitors. Introduction to Semiconductors. Diodes.
- **MOS Device Physics**: Threshold voltage, Current-Voltage relation, Intrinsic and Parasitic capacitance. MOS modeling. Parameter extraction.
- **Inverter Characteristics**: Static: Voltage Trandsfer Chracteristics (VTC), Noise Margin, Power & Area tradeoff. Dynamic: delays (propagation/transition), dynamic power.
- **Layout**: Introduction to design rules. DRC/LVS, stick diagram & euler graph.
- **Advanced topics**: standard cells, Liberty format (.LIB).
- **Project**: 6T SRAM Cell in 0.5um SCMOS technology.
- **Cloud-based EDA Platform**: Sue2 schematic editor, nspice simulator, Magic Layout editor, netgen LVS tool, CppSim Python Lib for ngspice.

## Announcements
- [26 May] : Probelem assignment 2 posted on vlsiexpert e-learning portal. For upload type questions, please upload your answer as a attachment (either pdf or png or JPEG etc).
- [21 May] : Use this [Link](https://us02web.zoom.us/meeting/register/tZAvcuyoqTgrHNwFF4Abu4TnOFCOn4WFVf1S) to register for Zoom sessions from May 24 till June 11, Mon-Fri: 10am-1pm, 2:3-5:30pm

## Session Activities
- [May 17-21] Introduction to Digital Design by Puneet Mittal, VLSI Expert Pvt. Ltd.
- [May 24] Introduction to CMOS VLSI Design Flow [[Video](https://www.youtube.com/watch?v=NVzHuigvpt4)]
  - [*Suggested Reading*]: [Hodges] Chapter-**1**, [Kang] Chapter-**1**
- [May 24] Introduction to CMOS PRocessing [[Video](https://www.youtube.com/watch?v=dauFDKM-Eu0)]
  - [*Suggested Reading*]: [Hodges] Section **3.1,3.2**
  - [*Suggested Problems*]: [Hodges] Prob **3.11, 3.12**
- [May 25] *Session-1*: Passive Integrated Circuit Devices. *Session-2*: MOS Device-I: Threshold Voltage [[Video:Session-1](https://www.youtube.com/watch?v=3SCYAH57Ixw) | [Video:Session-2](https://www.youtube.com/watch?v=OUZV9N0b3Lc)]
  - [*Suggested Reading*]: [Hodges] Section **3.2.4, 3.2.5**
  - [*Suggested Problems*]: [Hodges] Example **3.1**, Prob **3.13, 3.14**
- [May 26] *Session-1*: MOS Device-II: IV Characteristics and MOS Capacitance. *Session-2*: Basic Circuits. [[Video:Session-1](
https://www.youtube.com/watch?v=UUCB_dgFiwA) | [Video:Session-2](https://www.youtube.com/watch?v=OzlsThjjUDA)]
  - [*Suggested Reading*]: [Hodges] Section **2.2.1, 2.2.2, 2.2.3, 2.2.7** [Kang]: Chapter **3** [Uyemura] Section **1.1, 1.2** (Excellent treatment on Vt) [Baker]: Section **6.1, 6.2, 6.3**
  - [*Suggested Problems*]: [Hodges] Example **2.1,2.2,2.3,2.4,2.5,2.11**, Prob. **2.1,2.4,2.5,2.7,2.8**
- [May 27] *Session-1* Modeling of MOS Device for Circuit Simulation. *Session-2*: Introduction to Lab on Cloud. *Session-3*: Parameter Extraction. MOS Problem discussion. [Video: [Session-1](https://www.youtube.com/watch?v=mQEryii3McE) | [Session-2](https://www.youtube.com/watch?v=riMKnP0AaRI) | [Session-3](https://www.youtube.com/watch?v=zFpJc6QFUc4)]
  - [*Suggested Reading*]: [Hodges] Section **3.4, 3.5, 3.6, Appendix-A**, [NGSpice Manual](/tools/CppSimLite/CppSimShared/Doc/ngspice-32-manual.pdf)
  - [*Lab Assignment*] MOS Parameter extraction and MOS Level-1 Modeling [Link-to-PDK](docs/2021-0528-Assignment-MOS-SPICE.pdf)
- [May 28] *Session-1*: Intorduction to Linux. *Session-2*: Lab-1 -- Parameter extraction. [Video: [Session-1](https://www.youtube.com/watch?v=ZGqEjvjWcqI) | [Session-2](https://www.youtube.com/watch?v=hVf8IAfiDyI)]
- [May 31] *Session-1*: Solving problems related MOS Device. *Session-2*: Lab-2 Parameter extraction (contd.) [Video: [Session-1](https://www.youtube.com/watch?v=BWagkpkn60w) | [Session-2](https://www.youtube.com/watch?v=bogjUJGIeWM)]
- [June 1] *Session-1*: Inverter Characteristics -- Static. *Session-2*: Lab-3: Inverter Characterisitcs - Static [Video: [Session-1](https://www.youtube.com/watch?v=DJALnK61MYs) | [Session-2](https://www.youtube.com/watch?v=xgcMiJlihWY)]
  - [*Suggested Reading*]: [Hodges] Chapter 4
  - [*Suggested Problems*]: [Hodges] Example **4.6**, Problem **4.1,4.2,4.5,4.8,4.10,4.11**
- [June 2] *Session-1*: Inverter Characteristics -- Dynamic. *Session-2*: Lab-4 -- Inverter Characterisitcs - Dynamic [Video: [Session-1](https://www.youtube.com/watch?v=jXyXVIjiYcU) | [Session-2](https://www.youtube.com/watch?v=nAUKbAKebjQ)], [[Notes](https://www.dropbox.com/s/tt8w5zbh7vesn1x/2021-0602-15VLSI7T-Module1-Lecture16-17-Inverter-switching.pdf)]
  - [*Suggested Reading*]: [Hodges] Section **6.1,6.2,6.3,6.4,6.5**  
  - [*Suggested Problems*]: [Hodges] Example **6.1,6.3,6.4,6.5,6.8**
- [June 3] *Session-1*: Parasitics and Interconnect effect on switching. *Session-2*: Lab-5 -- Parasitics [Video: [Session-1](https://www.youtube.com/watch?v=euWVJqATv2A) | [Session-2](https://www.youtube.com/watch?v=jmFchoahUFM)], [[Notes](https://www.dropbox.com/s/zu1fobf63x9vjhx/2021-0603-15VLSI7T-Module4-Lecture3-Interconnect-Parasitics.pdf)]
- [June 4] *Session-1*: Energy and power calculation for inverters. *Session-2*: Lab-6 -- Inverter Layout using Magic.  [Videos: [Session-1](https://www.youtube.com/watch?v=oHBa8uv6xeU) | [Session-2](https://www.youtube.com/watch?v=y3ZpKECBKDg)]
- [June 7] *Session-1*: Standard Cell Design. *Session-2*: Lab-7 -- Standard Cell Layout [Videos: [Session-1](https://www.youtube.com/watch?v=er_Kh8FgRmM) | [Session-1](https://www.youtube.com/watch?v=bOG1iROYqz8)]
- [June 8] *Session-1*: MOS Device Equations: Refinement and additional effects. *Session-2*: Lab-8 -- Temperature effect on Vt and mobility. Python API for ngspice. [Videos: [Session-1](https://www.youtube.com/watch?v=KUFQLb0Japk) | [Session-1](https://www.youtube.com/watch?v=dnUg7rxwc5Q)]
  - [*Suggested Reading*]: [Hodges] Section **2.2.4, 2.2.6, 3.6.2, 3.6.3, 3.7.1, 3.7.2, 3.7.5**
  - [*Suggested Problems*]: [Hodges] Example **2.6, 2.7, 2.8**

## References/Resources
- [**Hodges**] Hodges, David A., et.al. "*Analysis And Design Of Digital Integrated Circuits, In Deep Submicron Technology*" (Special Indian Edition, 3rd Ed). Tata McGraw-Hill Education, 2005.
  - [**Appendix-A**] Chia J. & Saleh R., "*A Brief Introduction to SPICE*" [[Link-to-PDF](tools/CppSimLite/CppSimShared/Doc/Hodges-BriefIntroToSPICE-AppA.pdf)]
  - Useful Design Parameters from [Hodges] [[Link-to-PDF](docs/usefulParameters.pdf)]
- [**Kang**] Leblebici, Y., Chul W. K., and Sung-Mo (Steve) Kang. "*CMOS Digital Integrated Circuits Analysis & Design*". 4th ed. McGraw-Hill Education, 2014
- [**Uyemura**] Uyemura, John P. "*CMOS Logic Circuit Design*". Springer, 2007
- [**Baker**] Baker, R. Jacob. "*CMOS: Circuit Design, Layout, and Simulation*". John Wiley & Sons, 2008
- [**Weste**] Weste, Neil, and David Harris. "*CMOS VLSI Design: A Circuits and Systems Perspective*". Pearson Education, 2011
- [NGSpice Reference Manual][NGSpiceMan]: Comple reference manual in HTML format
  - [MOSFET Models](http://ngspice.sourceforge.net/docs/ngspice-html-manual/manual.xhtml#magicparlabel-1356039)
- [**BSIM4v4.8.0**](http://ngspice.sourceforge.net/external-documents/models/BSIM480_Manual.pdf): MOSFET Model -- User's Manual
- [**YouTube PLaylist-OpenSourceEDA**](https://www.youtube.com/playlist?list=PL7R2OODNugWFY2qeZ7qlVFNIkN8ABhuSO): Santunu Sarangi, Satabdi Panda and Pracheeta Mohapatra, "[15VLSI7T](https://github.com/silicon-vlsi/15VLSI7T):VLSI Design Course for 7th Sem AEI, 2020" -- Excellent set of detailed videos of Labs conducted for the course using ngspice, sue2, magic and netgen.
- [**Linux on Virtual Box**](https://www.dropbox.com/s/2lovix0ntsw8yfw/2020-0917-Open%20Source%20EDA%20Setup.pdf): Step by step isntruction on how to install a Virtual Machine on your Windows desktop/laptop using Virtual Box and then install Linux in the Virtual Machine.
- Quick Start Guide: [[Linux](https://www.makeuseof.com/tag/a-quick-guide-to-get-started-with-the-linux-command-line/)] [[Vim](https://www.engadget.com/2012/07/10/vim-how-to/)]


## Table of Content
- [EDA on Cloud](#LabonCloud)
- [NGSpice](#NGSpice)
    - [QuickStart Guide](#Quick-Start-Guide)
- [CppSimLite](#CppSimLite)
    - [Sue2](#Sue2)
- [Magic](#Magic)
- [Netgen](#Netgen)
- [Technology](#Technology)

## Lab on Cloud
[Lab Intro](https://www.youtube.com/watch?v=riMKnP0AaRI)

**System Requirement**:
- Desktop/Laptop with an internet connection.
- An upto-date web browser, preferably **Chrome** or Chrome-based.

**Onetime step to setup the account:**
- Navigate to https://virtualcoach.jnaapti.io/
- Click on "*Forgot your password*" and follow instruction to reset password

**Connecting and Starting the Virtual Linux Desktop**
- Navigate to https://virtualcoach.jnaapti.io/
- Login into the dashboard and from the top-left corner dropdown menu click **Lab Instances**.
- Start the instance by clicking the play button   
- Once the instance is ready, click on the **desktop** icon  on the right.
- A new tab should open up with “noVNC” title and a “connect” button underneath. Click it to launch the Linux desktop. (Ignore the error “No session for pid 83” and click ok)
- Scroll to the bottom and click the start button on the lower left corner and start a Terminal by clicking “System Tools -> **LXTerminal**”  NOTE: You can choose UXTerm and XTerm as well.
- That should bring up a terminal

**Setting up the Lab Environment**
- All our Labs and course resources are maintained at https://github.com/silicon-vlsi/SI2021-03-CMOS-VLSI
- First you have to clone this *repository* to your home directory:
  - ```git clone https://github.com/silicon-vlsi/SI2021-03-CMOS-VLSI```
- After cloning the repository, you directory should look something like this:
```
SI2021-03-CMOS-VLSI/
├── LICENSE
├── README.md
├── docs
│   ├── 2021-0528-Assignment-MOS-SPICE.pdf
│   └── usefulParameters.pdf
└── tools
    └── CppSimLite
```
- **IMPORTANT**: DO NOT WORK in this directory `SI2021-03-CMOS-VLSI`. Treat the content of this directory as READ-ONLY.
- In order to get all the environment variables of the EDA tools, append your ```.bashrc``` with the following:
```bash
if [ -f $HOME/SI2021-03-CMOS-VLSI/bashrc_eda ]; then
    . $HOME/SI2021-03-CMOS-VLSI/bashrc_eda
else
    echo "bashrc_eda DOES NOT EXIST"
fi
```
- Next time you start a terminal, you can access all the tools.

## NGSpice
[NGSpice] is a open source spice simulator for electric and electronic circuits. 
- [NGSpice Reference Manual][NGSpiceMan]: Complete reference manual in HTML format.

Precompiled ngspice v32 is installed in `/project2020/eda/ngspice-32`. Add the following environment variables in your `~/.bashrc`

```bash
export  SPICE_LIB_DIR=/project2020/eda/ngspice-32/glnxa64/share/ngspice
export  SPICE_EXEC_DIR=/project2020/eda/ngspice-32/glnxa64/bin
export  PATH=$PATH:$SPICE_EXEC_DIR
```
There is a initialization script in `$SPICE_LIB_DIR/scripts/spinit`. You can overwrite any of the initilization by adding commands to a local `~/.spiceinit` .

The Spice model files are located at `/project2020/eda/ngspice-32/models/scn4m_subm/nom`

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

- The Spice model files are located at `/project2020/eda/ngspice-32/models/scn4m_subm/nom`
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
