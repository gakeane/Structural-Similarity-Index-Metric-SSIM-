# Structural-Similarity-Index-Metric-SSIM-
Code used to develop a plug-in for the NUKE image processing software which adds the structural similarity index metric (SSIM). This metric is used to determine the quality of an image by comparing it to a reference image. The value of this metric will be between 0 and 1 where 1 indicates identical images.

Nuke example plug-in
--------------------

This solution builds a plug-in .dll that can be used with Nuke.

It is designed for use in Visual Studio 2010, and we highly recommend that you use this version.  See below for further
details.

The steps that are required to get it working in Nuke are:
- Make sure the include path is correct for your version
  - This can be found in Project -> Properties -> Configuration Properties -> C/C++ -> Additional Include Directories
  - It should be something along the lines of "C:\Program Files\Nuke6.5v1\include"
- Make sure the library path is correct for your version
  - This can be found in Project -> Properties -> Configuration Properties -> Linker -> Additional Library Directories
  - It should be something like "C:\Program Files\Nuke7v1"
  - If you build with the headers from one version (e. g. 6.5v1) and libraries from another (e. g. 6.3v2), you'll get a
    linker error.  This is intentional, because doing so would otherwise cause a crash at run time.
    - Major versions (5, 6) and minor versions (6.0, 6.5) are expected to be binary incompatible, but source compatible, so
      although plug-ins cannot be interchanged directly between the versions, only a recompile for a new version is needed.
      For 'v'-release updates (6.5v1, 6.5v2, etc), every effort is made to ensure that the versions are binary compatible,
      and no recompilation should be required.
- Either change the name of the Output File (next to the library path) or rename the plugin after building.  It needs to
  match the name of the class that the .dll will create.  For example, if you build the standard configuration unmodified,
  you should find the resulting ExamplePlugin.dll and rename it to Mirror.dll.  You can add this as a post-build step to the
  solution if you like.
- Build the .dll and put it in your NUKE_PATH - e. g. in "C:\Users\yourname\.nuke" on Vista and later, or
  "C:\Documents and Settings\yourname\.nuke" on XP

You can test this by attempting to create a node of the given type in Nuke.  If your plug-in has been added to a menu, it
will be on the tab menu automatically; if not, press "X" and run a TCL command with the exact name of your plug-in.  An
"unknown command" error means that your plug-in has not been found, so the .dll might not be in your plug-in path.  An
error loading the DLL means something has not built correctly or is incompatible - so check that you've built for the
right version of Nuke (using the right headers and libraries).
An error that says "plugin did not define..." means that your .dll and op names do not match; for example, if you're
building the Mirror example class, make sure the binary is called Mirror.dll.

We recommend that you use this solution in Visual Studio 2010 SP1.  Other versions may work, but please bear in mind that
they will use different versions of the Microsoft VC++ standard libraries, which can cause problems.  For example, Visual
Studio 2008 changes the implementation of STL containers and iterators in a way that is, by default, binary-incompatible
with applications built with 2010 builds, such as Nuke.  This problem can be avoided by setting _SECURE_SCL to 0, as
mentioned in the link below, but other problems may be encountered.
http://msdn.microsoft.com/en-us/library/aa985896.aspx

We also recommend that you always build your plug-ins in release mode.  Debug mode builds will expect different sizes and
locations of the objects in the DDImage headers, such as the Op class.  This means that running a debug plug-in in Nuke
will probably cause crashes in many operations.  Unfortunately this makes debugging custom plug-ins a bit more difficult.
This problem may be addressed in a future release, but doing so would require extensive changes to the API.

Additionally, systems with Nuke installed will be correctly set up with the VC++ libraries for 2010, but may not have
those required for binaries built with other versions.  Plug-in developers would need to ensure that these other versions
of the libraries are installed on any client machines that need to run those plug-ins, or crashes might result.

For these reasons, we recommend you build in release mode with Visual Studio 2010 if at all possible to minimise
these difficulties.

Please see the NDK documentation for further information about developing plug-ins.

- The Nuke dev team
