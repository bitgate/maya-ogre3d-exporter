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
//! \file "blendshape.h"
//! \brief Blendshape header for Ogre Exporter.
//!
//! \author     Francesco Giordana <fra.giordana@tiscali.it>
//! \author     Marcel Reinhard <marcel.reinhardt@filmakademie.de>
//! \author     Volker Helzle <marcel.reinhardt@filmakademie.de>
//! \version    1.0
//! \date       21.10.2010 (last updated)
//!

#ifndef _BLENDSHAPE_H
#define _BLENDSHAPE_H

#include "mayaExportLayer.h"
#include "paramList.h"
#include "animation.h"
#include "vertex.h"

namespace OgreMayaExporter
{
	typedef struct
	{
		MPlugArray srcConnections;
		MPlugArray dstConnections;
	} weightConnections;

	typedef struct
	{
		int targetIndex;
		std::vector<pose> poses;
	} poseGroup;

	// Blend Shape Class
	class BlendShape
	{
	public:
		// Constructor
		BlendShape();
		// Destructor
		~BlendShape();
		// Clear blend shape data
		void clear();
		// Load blend shape deformer from Maya
		MStatus load(MObject &blendShapeObj);
		// Load blend shape poses for shared geometry
		MStatus loadPosesShared(MDagPath& meshDag,ParamList &params,std::vector<vertex> &vertices,
			long numVertices,long offset=0);
		// Load blend shape poses for a submesh
		MStatus loadPosesSubmesh(MDagPath& meshDag,ParamList &params,std::vector<vertex> &vertices,
			std::vector<long>& indices,long targetIndex=0);
		//load a blend shape animation track
		Track loadTrack(float start,float stop,float rate,ParamList& params,int targetIndex,int startPoseId);
		// Get blend shape deformer name
		MString getName();
		// Get blend shape poses
		std::unordered_map<int, poseGroup>& getPoseGroups();
		// Set maya blend shape deformer envelope
		void setEnvelope(float envelope);
		// Restore maya blend shape deformer original envelope
		void restoreEnvelope();
		// Break connections to this blendshape
		void breakConnections();
		// Restore connections on this blendshape
		void restoreConnections();
		// Public members
		MFnBlendShapeDeformer* m_pBlendShapeFn;		

	protected:
		// Internal methods
		//load a blend shape pose for shared geometry
		MStatus loadPoseShared(MDagPath& meshDag,ParamList& params,std::vector<vertex>& vertices,
			long numVertices,long offset,MString poseName, int blendShapeIndex);
		//load a blend shape pose for a submesh
		MStatus loadPoseSubmesh(MDagPath& meshDag,ParamList& params,std::vector<vertex>& vertices,
			std::vector<long>& indices,MString poseName,int targetIndex, int blendShapeIndex);
		//load a blend shape animation keyframe
		vertexKeyframe loadKeyframe(float time,ParamList& params,int targetIndex,int startPoseId);

		// Protected members
		//original values to restore after export
		float m_origEnvelope;
		std::vector<float> m_origWeights;
		//blend shape poses
		std::unordered_map<int, poseGroup> m_poseGroups;
		//blend shape target (shared geometry or submesh)
		target m_target;
		//blend shape weights connections
		std::vector<weightConnections> m_weightConnections;
		// Array to determine if poses need to be optimized
		MIntArray poseOptArray;	
	};


}	// end namespace

#endif