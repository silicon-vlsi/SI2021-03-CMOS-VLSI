#!/usr/bin/env python
import os
import sys
cppsimsharedhome = os.getenv("CPPSIMSHAREDHOME")
sys.path.append(cppsimsharedhome + 'Python')
from cppsimdata import *
from ngspicedata import *
