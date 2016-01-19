#Retrieve args when compile script is called
$eclipse_path = $args[0]
$workspace_path = $args[1]
$build_configs = $args[2]

$xml_path = '.cproject'
$base_level = "ilg.gnuarmeclipse.managedbuild.cross.option.optimization.level."
#$optimization_levels = "none","optimize","more","most","size","debug"
$optimization_levels = @("none")
$path = "$pwd\$xml_path"

function ChangeOptimizationLevelInXML
{
	param([string]$xml_path, [string]$base_level, [string]$level)
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
					$options_xml[$j].value = $base_level+$level
					break
				}
			}
		}
	}
	$xml.Save($xml_path)
}

function CreateBuildParametersString {
	param([array]$build_configs, [string]$workspace_path)
	$ParamArray = @()
	$ParamArray += "--launcher.suppressErrors"
	$ParamArray += "-nosplash"
	$ParamArray += "-application"
	$ParamArray += "org.eclipse.cdt.managedbuilder.core.headlessbuild"
	$ParamArray += "-data"
	$ParamArray += $workspace_path
	for ($i=0; $i -le $build_configs.length-1; $i++) {
		$ParamArray += "-cleanBuild"
		$ParamArray += $build_configs[$i]
	}	
	$ParamArray
}

<#=========================================
               Compile script
===========================================#>

#Create parameter string
$build_parameters = CreateBuildParametersString $build_configs $workspace_path
#Create executable string
$exe = $eclipse_path + "\eclipsec.exe"

#For each optimisation level set level in XML, compile, save files at proper location
for ($i=0; $i -le $optimization_levels.length-1; $i++){
	ChangeOptimizationLevelInXML $path $base_level $optimization_levels[$i]
	& $exe $build_parameters
}