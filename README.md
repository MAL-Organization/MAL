# MAL - MCU Abstraction Library
This library as for goal to abstract the MCU from your code allowing you to change MCU more easily.

At the moment, the library only support 2 MCUs:
* STM32F072CB
* STM32F072RB

## Build process
Since we never have infinite time to work on projects , the build process is not yet fully automated.
The "perform" step is not yet working.

Doing a build still require manual steps in order to publish to GitHub
* Once your environment is setup properly (see below) you can run the following command in the /library folder:
 * mvn -U -B clean release:clean release:prepare release:perform
 * This command will tag and commit the build, and will generate under /library/target a mal-X.X.X-library.zip package
* Once the build is done, we still need to upload it to GitHub, you should see the tag that was just created here: https://github.com/MAL-Organization/MAL/releases
 * On the right of the new tag, click edit
 * Take the /library/target a mal-X.X.X-library.zip package and upload it to GitHub
 * Click Publish release

## Environment setup
The build script run Eclipse in headless mode. You need a fully working Eclipse workspace in order to do a build. Here is how to get there.
* Install last GNU ARM Toolchain by following this: http://gnuarmeclipse.github.io/toolchain/install/
* Install the GNU ARM Windows build tool by following this: http://gnuarmeclipse.github.io/windows-build-tools/install/
* Install MinGW with the latest release here: https://sourceforge.net/projects/mingw/files/Installer/
 * Install base and g++
 * Add the following path to Path environment variable: [PathToMinGW]/bin
* Download and install pyhton3
 * Add the following path to Path environment variable: [PathToPython3]
* If not present install Java JDK
* Install maven
* Download and unzip latest Eclipse CDT package, found here: http://www.eclipse.org/downloads/packages/
* Launch Eclipse and create a new workspace
* Install GNU ARM Eclipse plugin by following this: http://gnuarmeclipse.github.io/plugins/install/
* Install the Unit Test plugin for CDT from your CDT update site.
* Make sure Global tools path are set properly by following this: http://gnuarmeclipse.github.io/windows-build-tools/install/#check-the-global-tools-path
* Checkout the MAL repo
* In Eclipse in the project Explorer section, right click > Import > General > Existing Projects into Workspace > Next
* Select root directory, click Browse... > Select the library folder in the MAL repo > Ok > Finish
* In Eclipse in the project Explorer section, right click > Import > General > Existing Projects into Workspace > Next
* Select root directory, click Browse... > Select the test folder in the MAL repo > Ok > Finish
* Install git and make sure the path of the executable is the Path environment variable.
* Voila, you are ready to develop and build
