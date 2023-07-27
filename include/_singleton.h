/*
---------------------------------------------------------------------------------------------
-                    MAYA OGRE EXPORTER                                       -
---------------------------------------------------------------------------------------------
- Description:  This is a plugin for Maya, that allows the export of animated               -
-               meshes in the OGRE file format. All meshes will be combined                 -
-               together to form a single OGRE mesh, each Maya mesh will be                 -
-               translated as a submesh. Multiple materials per mesh are allowed            -
-               each group of triangles sharing the same material will become               -
-               a separate submesh. Skeletal animation and blendshapes are                  -
-               supported, or, alternatively, vertex animation as a sequence                -
-               of morph targets.                                                           -
-               The export command can be run via script too, for instructions              -
-               on its usage please refer to the Instructions.txt file.           -
- Note:     The particles exporter is an extra module submitted by the OGRE           -
-         community, it still has to be reviewed and fixed.                     -   
---------------------------------------------------------------------------------------------
- Original version by Francesco Giordana, sponsored by Anygma N.V. (http://www.nazooka.com) -
- The previous version was maintained by Filmakademie Baden-Wuerttemberg,           -
- Institute of Animation's R&D Lab (http://research.animationsinstitut.de)          -
-                                             -
- The current version (at https://www.github.com/bitgate/maya-ogre3d-exporter) is     -
- maintained by Bitgate, Inc. for the purpose of keeping Ogre compatible with the latest  -
- technologies.                                       -
---------------------------------------------------------------------------------------------
- Copyright (c) 2011 MFG Baden-Württemberg, Innovation Agency for IT and media.             -
- Research and Development at the Institute of Animation is a cooperation between           -
- MFG Baden-Württemberg, Innovation Agency for IT and media and                             -
- Filmakademie Baden-Württemberg as part of the "MFG Visual Experience Lab".                -
---------------------------------------------------------------------------------------------
- This program is free software; you can redistribute it and/or modify it under       -
- the terms of the GNU Lesser General Public License as published by the Free Software    -
- Foundation; version 2.1 of the License.                         -
-                                             -
- This program is distributed in the hope that it will be useful, but WITHOUT       -
- ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS       -
- FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.   -
-                                               -
- You should have received a copy of the GNU Lesser General Public License along with   -
- this program; if not, write to the Free Software Foundation, Inc., 59 Temple        -
- Place - Suite 330, Boston, MA 02111-1307, USA, or go to                 -
- http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html                  -
---------------------------------------------------------------------------------------------
*/

//!
//! \file "singleton.h"
//! \brief Singleton header for Ogre Exporter.
//!
//! \author     Francesco Giordana <fra.giordana@tiscali.it>
//! \author     Marcel Reinhard <marcel.reinhardt@filmakademie.de>
//! \version    1.0
//! \date       21.10.2010 (last updated)
//!

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <assert.h>

// Copied frome Ogre::Singleton, created by Steve Streeting for Ogre

namespace OgreMayaExporter 
{
    /** Template class for creating single-instance global classes.
    */
  //  template <typename T> class Singleton
  //  {
  //  protected:
  //      static T* ms_Singleton;

  //  public:
  //      Singleton(){
  //          assert( !ms_Singleton );
		//    ms_Singleton = static_cast< T* >( this );
  //      }
  //      ~Singleton(){
		//	assert( ms_Singleton );
		//	ms_Singleton = 0;  
		//}
		//static T& getSingleton(){
		//	assert( ms_Singleton );  
		//	return ( *ms_Singleton ); 
		//}
  //      static T* getSingletonPtr(){ 
		//	return ms_Singleton; 
		//}
  //  };



   template <typename T> class Singleton
    {
	private:
		/** \brief Explicit private copy constructor. This is a forbidden operation.*/
		Singleton(const Singleton<T> &);

		/** \brief Private operator= . This is a forbidden operation. */
		Singleton& operator=(const Singleton<T> &);
    
	protected:
        static T* ms_Singleton;

    public:
        Singleton( void )
        {
            assert( !ms_Singleton );
#if defined( _MSC_VER ) && _MSC_VER < 1200	 
            int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
            ms_Singleton = (T*)((int)this + offset);
#else
	    ms_Singleton = static_cast< T* >( this );
#endif
        }
        ~Singleton( void )
            {  assert( ms_Singleton );  ms_Singleton = 0;  }
        static T& getSingleton( void )
		{	assert( ms_Singleton );  return ( *ms_Singleton ); }
        static T* getSingletonPtr( void )
		{ return ms_Singleton; }
    };




}; // end namespace
#endif