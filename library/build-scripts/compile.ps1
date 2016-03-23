#Retrieve args when compile script is called
$eclipse_path = $args[0]
$workspace_path = $args[1]
$build_configs = $args[2]
$maven_base_dir = $args[3]

# Include common functions
. ($maven_base_dir + "/build-scripts/common.ps1")

$xml_file = '.cproject'
$xml_path = "$maven_base_dir\$xml_file"
$base_optimization_level = "ilg.gnuarmeclipse.managedbuild.cross.option.optimization.level."
$optimization_levels = @("optimize","more","most","size","debug","none")
$debug_level_build = "ilg.gnuarmeclipse.managedbuild.cross.option.debugging.level.none"
$debug_level_restore = "ilg.gnuarmeclipse.managedbuild.cross.option.debugging.level.max"


function ChangeDebugLevelInXML {
	param([string]$xml_path, [string]$debug_level, [array]$configurations)
	#Load XML
	$xml = new-object system.xml.xmldocument
	$xml.PreserveWhitespace = $true
	$xml.Load($xml_path)
	#Find proper storageModule and option that will control optimization level
	$storagemodules_xml = $xml.cproject.storageModule.cconfiguration.storageModule
	for ($i=0; $i -le $storagemodules_xml.length-1; $i++){
		if ($storagemodules_xml[$i].moduleId -eq "cdtBuildSystem") {
			#Make sure this is part of the configurations
			$config_found = $false
			for($j = 0; $j -lt $configurations.length; $j++) {
				if ($storagemodules_xml[$i].configuration.name -eq $configurations[$j]) {
					$config_found = $true
					break
				}
			}
			if ($config_found -eq $false) {
				# Skip this change
				continue
			}
			#Change level
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
	param([string]$xml_path, [string]$base_optimization_level, [string]$level, [array]$configurations)
	#Load XML
	$xml = new-object system.xml.xmldocument
	$xml.PreserveWhitespace = $true
	$xml.Load($xml_path)
	#Find proper storageModule and option that will control optimization level
	$storagemodules_xml = $xml.cproject.storageModule.cconfiguration.storageModule
	for ($i=0; $i -le $storagemodules_xml.length-1; $i++){
		if ($storagemodules_xml[$i].moduleId -eq "cdtBuildSystem") {
			#Make sure this is part of the configurations
			$config_found = $false
			for($j = 0; $j -lt $configurations.length; $j++) {
				if ($storagemodules_xml[$i].configuration.name -eq $configurations[$j]) {
					$config_found = $true
					break
				}
			}
			if ($config_found -eq $false) {
				# Skip this change
				continue
			}
			#Change level
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

function RetreiveExcludeFromXML {
	param([string]$xml_path, [string]$build_config, [string]$path)
	#Load XML
	$xml = new-object system.xml.xmldocument
	$xml.PreserveWhitespace = $true
	$xml.Load($xml_path)
	#Find proper build_config
	$storagemodules_xml = $xml.cproject.storageModule.cconfiguration.storageModule
	for ($i=0; $i -le $storagemodules_xml.length-1; $i++){
		if ($storagemodules_xml[$i].moduleId -eq "cdtBuildSystem") {
			if ($storagemodules_xml[$i].configuration.name -eq $build_config) {
				$entries_xml = $storagemodules_xml[$i].configuration.sourceEntries.entry
				[string[]]$exclusions_array = @()
				for ($j=0; $j -le $entries_xml.length-1; $j++) {
					if ($entries_xml[$j].name -ne "" -and $entries_xml[$j].excluding -ne $null ) {
						$exclusion = $entries_xml[$j].excluding.Replace("/","\").Split('|',[System.StringSplitOptions]::RemoveEmptyEntries)
						for ($k=0; $k -le $exclusion.length-1; $k++) {
							$exclusion[$k] = "*"+$exclusion[$k]+"*"
						}
						$exclusions_array += $exclusion
					}
				}
				#Exclusion found stoping
				break
			}
		}
	}
	$exclusions_array
}

function MoveLibInTarget {
	param([array]$build_configs, [string]$optimization_level, [string]$maven_base_dir)
	for ($i=0; $i -le $build_configs.length-1; $i++) {
		$build_config = $build_configs[$i]
		New-Item -ItemType directory -Force -Path "$maven_base_dir\target\libraries\$build_config\$optimization_level" | Out-Null
		Copy-Item "$maven_base_dir\$build_config\libmal.a" -Destination "$maven_base_dir\target\libraries\$build_config\$optimization_level\."	
	}
}

function MoveHeadersInTarget {
	param([array]$exclusions_array, [string]$maven_base_dir, [string]$build_config)
	$headers = Get-ChildItem $maven_base_dir -recurse -include *.h | Select-Object Directory,Name
	foreach ($file_obj in $headers) {
		$copy = $true
		
		foreach ($exclusion in $exclusions_array){
			if ($file_obj.Directory.FullName -like $exclusion) {
				$copy = $false
			}
		}
		# Exclude target where other header files will be copied
		if ($file_obj.Directory.FullName -like "*target*") {
			$copy = $false
		}
		
		if ($copy -eq $true) {
			$directory = $file_obj.Directory.FullName
			$file_name = $file_obj.Name
			$file_path = "$directory\$file_name"
			
			$destination_directory = $directory.Replace("$maven_base_dir","")
			New-Item -ItemType directory -Force -Path "$maven_base_dir\target\libraries\$build_config\headers\$destination_directory" | Out-Null
			Copy-Item "$file_path" -Destination "$maven_base_dir\target\libraries\$build_config\headers\$destination_directory\."
		}
	}
}

<#=========================================
               Compile script
===========================================#>
#Retreive header for each build config
for ($i=0; $i -lt $build_configs.length; $i++) {
	$exclusions_array = RetreiveExcludeFromXML $xml_path $build_configs[$i]
	MoveHeadersInTarget $exclusions_array $maven_base_dir $build_configs[$i]
}
#Set debug level to none
ChangeDebugLevelInXML $xml_path $debug_level_build $build_configs
#Create parameter string
$build_parameters = CreateBuildParametersString $build_configs $workspace_path "mal"
#Create executable string
$exe = $eclipse_path + "\eclipsec.exe"
#For each optimisation level set level in XML, compile, save files at proper location
for ($i=0; $i -le $optimization_levels.length-1; $i++){
	$level = $optimization_levels[$i]
	ChangeOptimizationLevelInXML $xml_path $base_optimization_level $level $build_configs
	Write-Output "Building library with optimization level $level..."
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
	MoveLibInTarget $build_configs $optimization_levels[$i] $maven_base_dir
}
#Restore debug level to max
ChangeDebugLevelInXML $xml_path $debug_level_restore
