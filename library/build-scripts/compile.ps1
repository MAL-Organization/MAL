#Retrieve args when compile script is called
$eclipse_path = $args[0]
$workspace_path = $args[1]
$build_configs = $args[2]
$maven_base_dir = $args[3]

$xml_file = '.cproject'
$xml_path = "$maven_base_dir\$xml_file"
$base_optimization_level = "ilg.gnuarmeclipse.managedbuild.cross.option.optimization.level."
#$optimization_levels = @("optimize","more","most","size","debug","none")
$optimization_levels = @("none")
$debug_level_build = "ilg.gnuarmeclipse.managedbuild.cross.option.debugging.level.none"
$debug_level_restore = "ilg.gnuarmeclipse.managedbuild.cross.option.debugging.level.max"


function ChangeDebugLevelInXML {
	param([string]$xml_path, [string]$debug_level)
	#Load XML
	$xml = new-object system.xml.xmldocument
	$xml.PreserveWhitespace = $true
	$xml.Load($xml_path)
	#Find proper storageModule and option that will control optimization level
	$storagemodules_xml = $xml.cproject.storageModule.cconfiguration.storageModule
	for ($i=0; $i -le $storagemodules_xml.length-1; $i++){
		if ($storagemodules_xml[$i].moduleId -eq "cdtBuildSystem") {
			$options_xml = $storagemodules_xml[$i].configuration.folderInfo.toolChain.option
			for ($j=0; $j -le $options_xml.length-1; $j++) {
				if ($options_xml[$j].name -eq "Debug level") {
					if ($options_xml[$j].value -ne $null) {
						$options_xml[$j].value = $debug_level
						break
					}
				}	
			}
		}
	}
	$xml.Save($xml_path)
}

function ChangeOptimizationLevelInXML {
	param([string]$xml_path, [string]$base_optimization_level, [string]$level)
	#Load XML
	$xml = new-object system.xml.xmldocument
	$xml.PreserveWhitespace = $true
	$xml.Load($xml_path)
	#Find proper storageModule and option that will control optimization level
	$storagemodules_xml = $xml.cproject.storageModule.cconfiguration.storageModule
	for ($i=0; $i -le $storagemodules_xml.length-1; $i++){
		if ($storagemodules_xml[$i].moduleId -eq "cdtBuildSystem") {
			$options_xml = $storagemodules_xml[$i].configuration.folderInfo.toolChain.option
			for ($j=0; $j -le $options_xml.length-1; $j++) {
				if ($options_xml[$j].name -eq "Optimization Level") {
					$options_xml[$j].value = $base_optimization_level+$level
					break
				}	
			}
		}
	}
	$xml.Save($xml_path)
}

#Change project location in eclipse for the deploy build inside /target/checkout
function ChangeEclipseProjectLocation {
	param([string]$workspace_path, [string]$maven_base_dir)
	if ($maven_base_dir -contains "checkout") {
		echo "================= CHANGING PROJECT LOCATION ============================="
		echo "================= CHANGING PROJECT LOCATION ============================="
		echo "================= CHANGING PROJECT LOCATION ============================="
		$location_file_path = "$workspace_path\.metadata\.plugins\org.eclipse.core.resources\.projects\mal\.location"
		(Get-Content $location_file_path).replace('library', 'library/target/checkout/library') | Set-Content $location_file_path
	}
}

function CreateBuildParametersString {
	param([array]$build_configs, [string]$workspace_path)
	$parameters_array = @()
	$parameters_array += "--launcher.suppressErrors"
	$parameters_array += "-nosplash"
	$parameters_array += "-application"
	$parameters_array += "org.eclipse.cdt.managedbuilder.core.headlessbuild"
	$parameters_array += "-data"
	$parameters_array += $workspace_path
	for ($i=0; $i -le $build_configs.length-1; $i++) {
		$parameters_array += "-cleanBuild"
		$parameters_array += "mal/" + $build_configs[$i]
	}	
	$parameters_array
}

function MoveLibInTarget {
	param([array]$build_configs, [string]$optimization_level, [string]$maven_base_dir)
	for ($i=0; $i -le $build_configs.length-1; $i++) {
		$build_config = $build_configs[$i]
		mkdir  "$maven_base_dir\target\libraries\$build_config\$optimization_level"
		Copy-Item "$maven_base_dir\$build_config\libmal.a" -Destination "$maven_base_dir\target\libraries\$build_config\$optimization_level\."	
	}
}

<#=========================================
               Compile script
===========================================#>
#Set debug level to none
ChangeDebugLevelInXML $xml_path $debug_level_build
#Chnage Eclipse project location if in deploy build
ChangeEclipseProjectLocation $workspace_path $maven_base_dir
#Create parameter string
$build_parameters = CreateBuildParametersString $build_configs $workspace_path
#Create executable string
$exe = $eclipse_path + "\eclipsec.exe"
#For each optimisation level set level in XML, compile, save files at proper location
for ($i=0; $i -le $optimization_levels.length-1; $i++){
	ChangeOptimizationLevelInXML $xml_path $base_optimization_level $optimization_levels[$i]
	& $exe $build_parameters
	MoveLibInTarget $build_configs $optimization_levels[$i] $maven_base_dir
}
#Restore debug level to max
ChangeDebugLevelInXML $xml_path $debug_level_restore