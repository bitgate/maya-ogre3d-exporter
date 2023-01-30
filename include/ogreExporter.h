/*
---------------------------------------------------------------------------------------------
-							       MAYA OGRE EXPORTER                                       -
---------------------------------------------------------------------------------------------
- Description: 	This is a plugin for Maya, that allows the export of animated               -
-              	meshes in the OGRE file format. All meshes will be combined                 -
-              	together to form a single OGRE mesh, each Maya mesh will be                 -
-              	translated as a submesh. Multiple materials per mesh are allowed            -
-              	each group of triangles sharing the same material will become               -
-              	a separate submesh. Skeletal animation and blendshapes are                  -
-              	supported, or, alternatively, vertex animation as a sequence                -
-              	of morph targets.                                                           -
-              	The export command can be run via script too, for instructions              -
-              	on its usage please refer to the Instructions.txt file.  					-
- Note: 		The particles exporter is an extra module submitted by the OGRE         	-
- 				community, it still has to be reviewed and fixed.  		            		-		
---------------------------------------------------------------------------------------------
- Original version by Francesco Giordana, sponsored by Anygma N.V. (http://www.nazooka.com) -
- The previous version was maintained by Filmakademie Baden-Wuerttemberg, 					-
- Institute of Animation's R&D Lab (http://research.animationsinstitut.de)  				-
-																							-
- The current version (at https://www.github.com/bitgate/maya-ogre3d-exporter) is			-
- maintained by Bitgate, Inc. for the purpose of keeping Ogre compatible with the latest	-
- technologies.																				-
---------------------------------------------------------------------------------------------
- Copyright (c) 2011 MFG Baden-Württemberg, Innovation Agency for IT and media.             -
- Research and Development at the Institute of Animation is a cooperation between           -
- MFG Baden-Württemberg, Innovation Agency for IT and media and                             -
- Filmakademie Baden-Württemberg as part of the "MFG Visual Experience Lab".                -
---------------------------------------------------------------------------------------------
- This program is free software; you can redistribute it and/or modify it under				-
- the terms of the GNU Lesser General Public License as published by the Free Software		-
- Foundation; version 2.1 of the License.													-
-																							-
- This program is distributed in the hope that it will be useful, but WITHOUT				-
- ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS				-
- FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.		-
- 																							-
- You should have received a copy of the GNU Lesser General Public License along with		-
- this program; if not, write to the Free Software Foundation, Inc., 59 Temple				-
- Place - Suite 330, Boston, MA 02111-1307, USA, or go to									-
- http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html									-
---------------------------------------------------------------------------------------------
*/

//!
//! \file "ogreExporter.cpp"
//! \brief Ogre Exporter header.
//!
//! \author     Francesco Giordana <fra.giordana@tiscali.it>
//! \author     Marcel Reinhard <marcel.reinhardt@filmakademie.de>
//! \version    1.0
//! \date       21.10.2010 (last updated)
//!

#ifndef OGRE_EXPORTER_H
#define OGRE_EXPORTER_H

#include "mesh.h"
#include "particles.h"
#include "mayaExportLayer.h"
#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <map>
#include <OgreLodStrategyManager.h>

namespace OgreMayaExporter
{
	class OgreExporter : public MPxCommand
	{
	public:
		// Public methods
		//constructor
		OgreExporter();
		//destructor
		virtual ~OgreExporter();
		//override of MPxCommand methods
		static void* creator();
		MStatus doIt(const MArgList& args);
		bool isUndoable() const;
		
		
	protected:
		// Internal methods
		//analyses a dag node in Maya and translates it to the OGRE format, 
		//it is recursively applied until the whole dag nodes tree has been visited
		MStatus translateNode(MDagPath& dagPath);
		//writes animation data to an extra .anim file
		MStatus writeAnim(MFnAnimCurve& anim);
		//writes camera data to an extra .camera file
		MStatus writeCamera(MFnCamera& camera);
		//writes all translated data to a group of OGRE files
		MStatus writeOgreData();
		// writes clip lengths into anim file
		MStatus writeClipLengths(ParamList& params);
		//cleans up memory and exits
		void exit();

	private:
		// private members
		MStatus stat;
		ParamList m_params;
		Mesh* m_pMesh;
		MaterialSet* m_pMaterialSet;
		MSelectionList m_selList;
		MTime m_curTime;

		std::map<double, double> _vA_map;
		std::map<double, double> _cam_vA_map;
		double _lastStop;
		double _firstStart;
		double _lastStop_cam;
		double _firstStart_cam;
		Ogre::Root *mr;
		Ogre::DefaultHardwareBufferManager* bufferManager;
	};




	/*********************************************************************************************
	*                                  INLINE Functions                                         *
	*********************************************************************************************/
	// Standard constructor
	inline OgreExporter::OgreExporter()
		:m_pMesh(0), m_pMaterialSet(0)
	{
		MGlobal::displayInfo("Translating scene to OGRE format");		
		mr = new Ogre::Root("", "", "ogreMayaExporter.log");
		bufferManager = new Ogre::DefaultHardwareBufferManager; // needed because we don't have a rendersystem
	}

	// Routine for creating the plug-in
	inline void* OgreExporter::creator()
	{
		return new OgreExporter();
	}

	// It tells that this command is not undoable
	inline bool OgreExporter::isUndoable() const
	{
		MGlobal::displayInfo("Command is not undoable");
		return false;
	}

}	//end namespace
#endif