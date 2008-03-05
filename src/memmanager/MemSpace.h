/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Jean-Francois Blanchard-Dionne <jean-francois.blanchard-dionne@polymtl.ca>
 *                                                                              
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *                                                                                
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                              
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef MEMSPACE_H
#define MEMSPACE_H

#include "MemKey.h"
#include "MemData.h"

//! Shared memory space frontend
/*
 *  This class hides the fucntionnalities of the shared memory management.
 */
class MemSpace {
public:

	//! Constructor
	/*!
	 * Create a new shared memory space or links to an existing one
	 * 
	 * \param key the key identifiying the shared memory space
	 */
    MemSpace(MemKey* key);

    //! Pointer copy constructor
    /*!
     * This constructor \b does \b not create a new MemSpace, it copie the base address dans the key.
     * 
     * \param space a pointer to a reference MemSpace
     */ 
    MemSpace(MemSpace* space);

    //! Reference copy constructor
    /*!
     * This constructor \b does \b not create a new MemSpace, it copie the base address dans the key.
     * 
     * \param space a reference to a reference MemSpace
     */ 
    MemSpace(MemSpace& space);

    //! Destructor
    /*!
     * Unlinks the shared memory space from this process.
     */
    ~MemSpace();

    //! Returns the key associated with this MemSpace
    /*!
     * \return a MemKey containing the information for this MemSpace
     */
    MemKey* getKey();

    //! Changes the data in the MemSpace
    /*!
     * \param Data a pointer to the new data
     * \param size the size of the new data
     * \return the success of the operation
     */
    bool putData(void * Data, int size);

    //! Gets the data in the Shared Memory
    /*!
     * \return the data contained int the shared memory in the form of a MemData
     */
    MemData* fetchData( );

private:
	
	//! Writes data to the shared memory space
	/*!
	 *	The purpose of this fucntion is to concentrate os specific code
	 * 
	 * \param data a pointer to the data to be written
	 * \param size the size of data
	 */
    void writeSpace(char * data, int size);
    
    //! Read from the shared memory space
    /*!
     * The purpose of this fucntion is to concentrate os specific code
     * 
     * \param a pointer to a MemData object
     */
    void readSpace(MemData* data);

    // Default constrcutor
    /*!
     * The default constrcutor is declared private to prevent the declaration of a MemSpace without a MemKey
     */
    MemSpace();
    
    //! The base address of the shared memory space
    char * baseAddress;
    
    //! The Memkey Associated with this MemSpace
    MemKey* theKey;
    
};
#endif //MEMSPACE_H
