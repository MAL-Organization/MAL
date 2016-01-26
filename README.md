# MAL - MCU Abstraction Library
This library as for goal to abstract the MCU from your code allowing you to change MCU more easily.

At the moment, the library only support 2 MCUs:
* STM32F072CB
* STM32F072RB

## Build process
Since we never have infinite time to work on projects , the build process is not yet fully automated.
The "perform" step is not yet working.

Doing a build still require manual steps.
* Once your environment is setup properly (see below) you can run the following command in the /library folder:
 * mvn -U -B clean release:clean release:prepare
 * This command will tag and commit the build, and will generate under /library/target a mal-X.X.X-library.zip package
* Take the zip package and post it on GitHub in the release section

## Environment setup
The build script run Eclipse in headless mode. You need a fully working Eclipse workspace in order to do a build. Here is how to get there.
* Install last GNU ARM Toolchain by following this: http://gnuarmeclipse.github.io/toolchain/install/
* Install the GNU ARM Windows build tool by following this: http://gnuarmeclipse.github.io/windows-build-tools/install/
* If not present install Java JDK
* If not present install Powershell 4 or newer (to find what version you have use the following command: $PSVersionTable.PSVersion )
* Download and unzip latest Eclipse CDT package, found here: http://www.eclipse.org/downloads/packages/
* Launch Eclipse and create a new workspace
* Install GNU ARM Eclipse plugin by following this: http://gnuarmeclipse.github.io/plugins/install/
* Make sure Global tools path are set properly by following this: http://gnuarmeclipse.github.io/windows-build-tools/install/#check-the-global-tools-path
* Checkout the MAL repo
* In Eclipse in the project Explorer section, right click > Import > General > Existing Projects into Workspace > Next
* Select root directory, click Browse... > Select the library folder in the MAL repo > Ok > Finish
* In the library folder, create a new file named "build.properties"
 * Enter the following lines and make sure to set the proper path to Eclipse and your workspace:
  * path.to.eclipse=C:\\eclipse
  * path.to.workspace=C:\\workspace
* Don't forget to escape backslash using \\
* Voila, you are ready to develop and build
