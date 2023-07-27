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
//! \file "submesh.h"
//! \brief Submesh header for Ogre Exporter.
//!
//! \author     Francesco Giordana <fra.giordana@tiscali.it>
//! \author     Marcel Reinhard <marcel.reinhardt@filmakademie.de>
//! \version    1.0
//! \date       21.10.2010 (last updated)
//!

#ifndef _SUBMESH_H
#define _SUBMESH_H

#include "mayaExportLayer.h"
#include "paramList.h"
#include "materialSet.h"
#include "animation.h"
#include "vertex.h"
#include "blendshape.h"

namespace OgreMayaExporter
{
	/***** Class Submesh *****/
	class Submesh
	{
	public:
		//constructor
		Submesh(const MString& name = "");
		//destructor
		~Submesh();
		//clear data
		void clear();
		//load data
		MStatus loadMaterial(MObject& shader,MStringArray& uvsets,ParamList& params);
		MStatus load(const MDagPath& dag,std::vector<face>& faces, std::vector<vertexInfo>& vertInfo, MPointArray& points,
			MFloatVectorArray& normals, MStringArray& texcoordsets,ParamList& params,bool opposite = false); 
		//load a keyframe for the whole mesh
		MStatus loadKeyframe(Track& t,float time,ParamList& params);
		//get number of triangles composing the submesh
		long numTriangles();
		//get number of vertices
		long numVertices();
		//get submesh name
		MString& name();
		//write submesh data to an Ogre compatible mesh
		MStatus createOgreSubmesh(Ogre::MeshPtr pMesh,const ParamList& params);
		//create an Ogre compatible vertex buffer
		MStatus createOgreVertexBuffer(Ogre::SubMesh* pSubmesh,Ogre::VertexDeclaration* pDecl,const std::vector<vertex>& vertices);

	public:
		//public members
		MString m_name;
		Material* m_pMaterial;
		long m_numTriangles;
		long m_numVertices;
		std::vector<long> m_indices;
		std::vector<vertex> m_vertices;
		std::vector<face> m_faces;
		std::vector<uvset> m_uvsets;
		bool m_use32bitIndexes;
		MDagPath m_dagPath;
		BlendShape* m_pBlendShape;
		MBoundingBox m_boundingBox;
	};

}; // end of namespace

#endif