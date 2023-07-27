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
//! \file "mesh.h"
//! \brief Mesh header for Ogre Exporter.
//!
//! \author     Francesco Giordana <fra.giordana@tiscali.it>
//! \author     Marcel Reinhard <marcel.reinhardt@filmakademie.de>
//! \version    1.0
//! \date       21.10.2010 (last updated)
//!

#ifndef _MESH_H
#define _MESH_H

#include "submesh.h"
#include "skeleton.h"
#include "mayaExportLayer.h"
#include "vertex.h"

namespace OgreMayaExporter
{
	/***** structures to store shared geometry *****/
	typedef struct dagInfotag
	{
		long offset;
		long numVertices;
		MDagPath dagPath;
		BlendShape* pBlendShape;
	} dagInfo;

	typedef struct sharedGeometrytag
	{
		std::vector<vertex> vertices;
		std::vector<dagInfo> dagMap;
	} sharedGeometry;

	typedef std::unordered_map<int,int> submeshPoseRemapping;

	typedef std::unordered_map<int,submeshPoseRemapping> poseRemapping;


	/***** Class Mesh *****/
	class Mesh
	{
	public:
		//constructor
		Mesh(const MString& name = "");
		//destructor
		~Mesh();
		//clear data
		void clear();
		//get pointer to linked skeleton
		Skeleton* getSkeleton();
		//load mesh data from a maya Fn
		MStatus load(const MDagPath& meshDag,ParamList &params);
		//load vertex animations
		MStatus loadAnims(ParamList &params);
		//load blend shape deformers
		MStatus loadBlendShapes(ParamList &params);
		//load blend shape animations
		MStatus loadBlendShapeAnimations(ParamList& params);
		//write to a OGRE binary mesh
		MStatus writeOgreBinary(ParamList &params);

		MString getName() {return m_name;}

	protected:
		//get uvsets info from the maya mesh
		MStatus getUVSets(const MDagPath& meshDag);
		//get skin cluster linked to the maya mesh
		MStatus getSkinCluster(const MDagPath& meshDag,ParamList& params); 
		//get blend shape deformer linked to the maya mesh
		MStatus getBlendShapeDeformer(const MDagPath& meshDag,ParamList& params);
		//get connected shaders
		MStatus getShaders(const MDagPath& meshDag);
		//get vertex data
		MStatus getVertices(const MDagPath& meshDag,ParamList& params);
		//get vertex bone assignements
		MStatus getVertexBoneWeights(const MDagPath& meshDag,OgreMayaExporter::ParamList &params);
		//get faces data
		MStatus getFaces(const MDagPath& meshDag,ParamList& params);
		//build shared geometry
		MStatus buildSharedGeometry(const MDagPath& meshDag,ParamList& params);
		//create submeshes
		MStatus createSubmeshes(const MDagPath& meshDag,ParamList& params);
		//load a vertex animation clip
		MStatus loadClip(MString& clipName,float start,float stop,float rate,ParamList& params);
		//load a vertex animation track for the whole mesh
		MStatus loadMeshTrack(Animation& a,std::vector<float>& times,ParamList& params);
		//load all submesh animation tracks (one for each submesh)
		MStatus loadSubmeshTracks(Animation& a,std::vector<float>& times,ParamList& params);
		//load a keyframe for the whole mesh
		MStatus loadKeyframe(Track& t,float time,ParamList& params);
		//write shared geometry data to an Ogre compatible mesh
		MStatus createOgreSharedGeometry(Ogre::MeshPtr pMesh,ParamList& params);
		//create an Ogre compatible vertex buffer
		MStatus createOgreVertexBuffer(Ogre::MeshPtr pMesh,Ogre::VertexDeclaration* pDecl,const std::vector<vertex>& vertices);
		//create Ogre poses for pose animation
		MStatus createOgrePoses(Ogre::MeshPtr pMesh,ParamList& params);
		//create vertex animations for an Ogre mesh
		MStatus createOgreVertexAnimations(Ogre::MeshPtr pMesh,ParamList& params);
		//create pose animations for an Ogre mesh
		MStatus createOgrePoseAnimations(Ogre::MeshPtr pMesh,ParamList& params);

		//internal members
		MString m_name;
		long m_numTriangles;
		std::vector<uvset> m_uvsets;
		std::vector<Submesh*> m_submeshes;
		Skeleton* m_pSkeleton;
		sharedGeometry m_sharedGeom;
		std::vector<Animation> m_vertexClips;
		std::vector<Animation> m_BSClips;
		//temporary members (existing only during translation from maya mesh)
		std::vector<vertexInfo> newvertices;
		std::vector<MFloatArray> newweights;
		std::vector<MIntArray> newjointIds;
		MPointArray newpoints;
		MFloatVectorArray newnormals;
		MStringArray newuvsets;
		MFnSkinCluster* pSkinCluster;
		BlendShape* pBlendShape;
		MObjectArray shaders;
		MIntArray shaderPolygonMapping;
		std::vector<faceArray> polygonSets;
		bool opposite;
		poseRemapping m_poseRemapping;
	};

}; // end of namespace

#endif