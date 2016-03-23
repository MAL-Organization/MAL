#Retrieve args when compile script is called
$eclipse_path = $args[0]
$workspace_path = $args[1]
$maven_base_dir = $args[2]

# Include common functions
. ($maven_base_dir + "/build-scripts/common.ps1")

<#=========================================
               Test script
===========================================#>
Write-Output "Building library for MINGW..."
#Create parameter string for mal
$build_configs = @("MINGW")
$build_parameters = CreateBuildParametersString $build_configs $workspace_path "mal"
#Create executable string
$exe = $eclipse_path + "\eclipsec.exe"
#Launch build and capture output to exit on errors
$output = & $exe $build_parameters *>&1
if ($output -like "*Workspace already in use!*") {
	Write-Output "ERROR: Workspace already in use! - Please close Eclipse"
	Exit 1
	break
}
elseif ($output -like "*Skipping*") {
	Write-Output "ERROR: Wrong workspace path"
	Exit 1
	break
}

Write-Output "Building unit tests..."
#Create parameter string for test
$build_configs = @("Debug")
$build_parameters = CreateBuildParametersString $build_configs $workspace_path "mal-test"
#Create executable string
$exe = $eclipse_path + "\eclipsec.exe"
#Launch build and capture output to exit on errors
$output = & $exe $build_parameters *>&1
if ($output -like "*Workspace already in use!*") {
	Write-Output "ERROR: Workspace already in use! - Please close Eclipse"
	Exit 1
	break
}
elseif ($output -like "*Skipping*") {
	Write-Output "ERROR: Wrong workspace path"
	Exit 1
	break
}
# Execute tests
$exe = $maven_base_dir + "/../test/Debug/mal-test.exe"
$output = & $exe
echo $output

Exit $lastexitcode
