function CreateBuildParametersString {
	param([array]$build_configs, [string]$workspace_path, [string]$project)
	$parameters_array = @()
	$parameters_array += "--launcher.suppressErrors"
	$parameters_array += "-nosplash"
	$parameters_array += "-application"
	$parameters_array += "org.eclipse.cdt.managedbuilder.core.headlessbuild"
	$parameters_array += "-data"
	$parameters_array += $workspace_path
	for ($i=0; $i -le $build_configs.length-1; $i++) {
		$parameters_array += "-cleanBuild"
		$parameters_array += $project + "/" + $build_configs[$i]
	}	
	$parameters_array
}
