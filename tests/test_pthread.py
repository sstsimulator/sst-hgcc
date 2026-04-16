#!/usr/bin/env python
# Single-node SST driver for test_pthread.

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
