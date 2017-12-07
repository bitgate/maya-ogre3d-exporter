# Maya Ogre Exporter
A Maya extension to export models and animations to Ogre binary format. Updated to work with Maya 2018.

## Original authors
Bitgate, Inc. has not written the majority of this extension, but only improved upon it or fixed issues to make it compatible with the latest version of Maya. The original authors, in chronological order, are:
 - Francesco Giordana, sponsored by Anygma N.V;
 - Filmakademie Baden-Wuerttemberg, Institute of Animation's R&D Lab;

## Changes from the original
The original code did not compile on VS2017 due to use of deprecated code, so for a start the deprecated code has been replaced by its newer replacements. The build process has been simplified greatly, only being one CMake file now and using a MAYA_HOME variable to use the correct Maya version.

The installer has been removed from this repository as it does not serve enough purpose to be kept. As a Github repository is now available, releases can be obtained directly in binary form from the releases section.

The copyright header has been altered to add that this repository is currently maintained by Bitgate, Inc. and contributions are welcome.

## Ideology
Working with Ogre3d can be a pain when working with modern art tools, as the vast majority is outdated and does not work with current versions without major changes. We've decided to clean up old and dusty libraries and extensions to make the workflow of Ogre with art tools (such as Maya) more enjoyable. Having the latest Maya version work is a big step in the right direction.

## Installation
To install the Maya extension, grab a copy over at the releases page, and extract it. 

- Copy the `ogreExporter.mll` file, which is a renamed DLL file, as well as the `OgreMain.dll` to the Maya binary folder (for 2018 on Windows this resolves to `C:\Program Files\Autodesk\Maya2018\bin`).
- Copy the plugin file, `ogreExporter.mel`, to the scripts directory (most likely at `C:\Users\You\Documents\maya\2018\scripts`).
- If you have a `userSetup.mel` file in your Maya scripts directory, simply add `source ogreExporter.mel;` to the end of the file. If you do not have a `userSetup.mel` file yet, please copy the included file to your scripts directory.

## Usage
### Through the UI
You can open the exporter window, if the extension is correctly installed, at the top menu bar. There should be a menu item named `Ogre` which has an `Export` option. Opening that brings up the Ogre Exporter tool window.

Most of the options are for advanced use, and are not required at all. Tick the options you need for your model, fill in the path names and press export.

## Through script command line
You can also use `ogreExport` from the Maya command interpreter. The options below are copied verbatim from the (old) Readme.txt:

```
ogreExport 	generalOptions 

		["-mesh" meshFilename meshOptions]
			export mesh to .mesh binary file

		["-mat" matFilename matOptions]
			export materials to .material script file

		["-skel" skelFilename]
			export skeleton to .skeleton binary file
	
		["-skeletonAnims" skelAnimsOptions ["-skeletonClip" clipName clipOptions] ["-clip" ...] [...] ]
			export skeleton animations to the .skeleton file 
			[requires -skel]

		["-vertexAnims" vertexAnimOptions ["-vertexClip" clipName clipOptions] ["-clip" ...] [...] ]
			export vertex animations as morph animations to the .mesh file 
			[requires -mesh]

		["-blendShapes"] bsOptions
			export blend shapes as mesh poses to the .mesh file 
			[requires -mesh]

		["-BSAnims" ["-BSClip" clipName clipOptions] ["-clip" ...] [...] ]
			export blend shape animations as pose animations to the .mesh file 
			[requires -mesh]

		["-particles" particlesFilename]
			export particles to .particle file


generalOptions:
	"-sel" | "-all"		export whole scene or only selected objects
	"-world" | "-obj"	export in world or object coordinates
	"-lu " "pref | mm | cm | m | in | ft | yd"	select length unit for export
							("-lu pref" means to get unit from scene
								preferences)
	"-scale" s		scale the whole mesh by s

	
meshOptions:
	["-shared"]				export using shared geometry
	["-v"]					export vertex bone assignements
	["-n"]					export vertex normals
	["-c"]					export vertex colours
	["-t"]					export texture coordinates
	["-edges"]				generate mesh edge list
	["-tangents" "TEXCOORD | TANGENT"]	generate tangents
	["-tangentsplitmirrored"]	split tangents mirrored
	["-tangentsplitrotated"]	split tangents rotated
	["-tangentuseparity"]		use parity for tangents

matOptions:
	["-matPrefix" prefix]	add prefix to all exported materials names [optional]
	["-copyTex" outDir]	copy textures used in the exported materials to outDir [optional]
	["-lightOff"]		export materials with lighting off [optional]

skelAnimsOptions:
	"-skelBB"		include skeleton animations in bounding box calculation
	"-np" ( "curFrame" | "bindPose" | "frame" n )	specify neutral pose, can be current frame or bind pose or specified frame

bsOptions:
	["-bsBB"]		include blend shapes in bounding box calculation

vertexAnimOptions:
	["-vertBB"]		include vertex animations in bounding box calculation

clipOptions:
	"startEnd" s e ("frames" | "seconds") | "timeSlider"	specify clip range with start/end time or use time slider range
```