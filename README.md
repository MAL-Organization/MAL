# MAL - MCU Abstraction Library
This library has for goal to abstract the MCU from your code allowing you to abstract the drivers.

At the moment, the library supports the following MCUs:
* STM32F072CB

## API Documentation
http://mal-organization.github.io/MAL/

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
* Install Java JDK
* Install maven
  * Add maven to Path environment variable
* Install python 3
  * Add the following path to Path environment variable: [PathToPython3]
  * Create a python3 executable
* Install cmake
  * Create CMAKE_HOME environment variable. This should point to the bin folder of cmake.
* Install the GNU ARM toolchain
  * Create the ARM_HOME environment variable. This should point to the bin folder of the toolchain.
  * Create the GCC_ARM_HOME environment variable. This point to [PathToToolchain]/lib/gcc/[VERSION]
* Install the GNU ARM Windows build tools by following this: https://gnu-mcu-eclipse.github.io/windows-build-tools/install/
  * Create the ARM_BUILD_TOOLS_HOME environment variable. This should point to the bin folder of the tools.
* Install cygwin
  * Install package make
  * Install package gcc-core
  * Install package gcc-g++
  * Install package curl
  * Create CYGWIN_PATH environment variable. This should point to the bin folder of cygwin.
* Install doxygen
  * Add doxygen to Path environment variable
* Install git
  * Add git to Path environment variable
