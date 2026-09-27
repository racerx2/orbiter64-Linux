Orbiter 2024 Readme
===================


Orbiter is now published as an Open Source project under the MIT
License (see LICENSE file for details).


1. Hardware requirements
------------------------
The minimum hardware requirements needed by Orbiter are:
RAM: 500 MB
CPU: Dual Core
GPU: 50 GFlops, Vulkan 1.4 driver
Disk: 5 GB of free space

The recommended hardware requirements are:
RAM: 2 GB
GPU: 100 GFlops
Disk: 10 GB of free space (80 GB if you want hi-res textures)


2. Orbiter installation
-----------------------
Create a new folder for the Orbiter installation, e.g. ~/Orbiter.

If a previous version of Orbiter is already installed on your
computer, you should not install the new version into the same
folder, because this could lead to file conflicts. You may want
to keep your old installation until you have made sure that the
latest version works without problems. Multiple Orbiter
installations can exist on the same computer.

Unpack the Orbiter .tar.gz installation package into the new
folder, using either your file manager's archive tool or
tar -xzf OpenOrbiter-<version>-Linux.tar.gz -C ~/Orbiter
Important: Take care to preserve the directory structure of the
package. The Orbiter folder is OpenOrbiter-<version>-Linux/Orbiter
inside it.

After unpacking the package, make sure your Orbiter folder
contains the executable (Orbiter), its launcher (OpenOrbiter)
and, among other files, the Config, Meshes, Scenarios and Textures
subfolders.

To uninstall Orbiter, simply remove the Orbiter folders with
all contents and subdirectories. This will completely remove
Orbiter from your hard drive.


3. Launching Orbiter
--------------------
The Orbiter simulator is launched with the OpenOrbiter launcher
in the Orbiter folder (it starts the Orbiter executable from its
own folder). Graphics come from the included VulkanClient plugin
(a Vulkan 1.4 driver is required): select it as graphics engine
on the Video tab of the Launchpad. Without a graphics engine,
Orbiter runs in console mode.
Once running, Orbiter will show you the "Launchpad" dialog, where
you can select video options and simulation parameters.

You are now ready to start, select a scenario from the Launchpad
dialog and click the "Launch Orbiter" button!


4. Help
-------
Help files are located in the Doc subfolder.
Orbiter User Manual.pdf is the main Orbiter manual, and it is
highly recommended you read it.

The in-game help system can be opened via the "Help" button on
the Orbiter Launchpad dialog, or with Alt-F1 while running
Orbiter.

Remaining questions can be posted on the Orbiter user forum at
https://orbiter-forum.com.
