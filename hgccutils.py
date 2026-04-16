
def swapSuffix(suffix, path):
  splitter = path.split(".")[:-1]
  splitter.append(suffix)
  return ".".join(splitter)

def rebaseFolder(path, srcDir, dstDir):
  folder, fname = os.path.split(path)
  newBaseFolder = folder.replace(srcDir,dstDir)
  return os.path.join(newBaseFolder, fname)


def addPrefix(prefix, path):
  import os
  splitPath = os.path.split(path)
  if len(splitPath) == 2:
    return os.path.join(splitPath[0], prefix + splitPath[1])
  else:
    return prefix + path

def addPrefixAndRebase(prefix, path, newBase):
  import os
  newPath = addPrefix(prefix, path)
  folder, name = os.path.split(newPath)
  return os.path.join(newBase, name)

def delete(files):
  import traceback
  import os
  os.system("rm -f %s" % (" ".join(files)))

def getProcTree():
  """Parent chain via ps -p (avoids full ps scan / hangs under parallel make)."""
  import os
  import configlib

  arr = []
  pid = os.getpid()
  seen = set()
  max_depth = 256
  for _ in range(max_depth):
    if pid <= 0 or pid in seen:
      break
    seen.add(pid)
    comm = configlib.getoutput("ps -p %d -o comm=" % pid).strip()
    arr.append(comm if comm else "?")
    ppid_out = configlib.getoutput("ps -p %d -o ppid=" % pid).strip()
    try:
      ppid = int(ppid_out.split()[0])
    except (ValueError, IndexError):
      break
    if ppid == pid:
      break
    pid = ppid
  return arr

def getProcName():
  import os
  import configlib
  import sys
  pid = int(os.getppid())
  runCmds = configlib.getoutput("ps -p %d" % pid).splitlines()[-1].split()
  runCmds = runCmds[3:]
  firstCmd = runCmds[0].lstrip("-")
  if firstCmd in ("/bin/sh", "sh", "bash", "/bin/bash", "tcsh", "/bin/tcsh", "zsh", "/bin/zsh"):
    if len(runCmds) > 1: #it might just be bash
      firstCmd = runCmds[1]
  cmd = os.path.split(firstCmd)[-1]
  return cmd

def cleanFlag(flag):
  from hgccvars import includeDir, execPrefix, prefix 
  return flag.replace("${includedir}", includeDir).replace("${exec_prefix}", execPrefix).replace("${prefix}",prefix).strip()
