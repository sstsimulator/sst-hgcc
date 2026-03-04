#!/usr/bin/env python
#
# SST Python driver for the test_pthread skeleton app.
# Uses simple single-node setup (no MPI, no merlin network).

import sst
import sst.hg

node0 = sst.Component("Node0", "hg.Node")
os0 = node0.setSubComponent("os_slot", "hg.OperatingSystem")

link0 = sst.Link("link0")
link0.connect( (node0,"network","1ns"), (node0,"network","1ns") )

os0.addParams({
    "app1.name" : "test_pthread",
    "app1.exe_library_name" : "test_pthread",
})
