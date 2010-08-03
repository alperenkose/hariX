/* os_info.hpp --- 
 *
 * Copyright (C) 2010 Alp Eren Köse
 *
 * Author: Alp Eren Köse <alperenkose@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
/*!
  \file   os_info.hpp
  \date   Wed Jul 28 17:09:29 2010
  
  \brief  OsInfo Class Implementation
  
*/
#if !defined( _OS_INFO_ )
#define _OS_INFO_


//! Represents an Operating System object
/*! 
  Represents information about an Operating System holding
  values of Distribution name, Release name, Kernel version and it's architecture.
*/
class OsInfo
{
  
private:
  
  std::string distroName_,		/*!< Holds the Distribution name. e.g. Ubuntu */
	releaseName_,				/*!< Holds the Release name. e.g. 9.10 */
	kernelVersion_,				/*!< Holds the Kernel version. e.g. 2.6.32 */
	hardwareArch_;				/*!< Holds the Architecture the Kernel compiled for. e.g. i686 */

  
public:

  //! Constructor.
  /*!
	Create an OS object with provided values.
	
	\param distribution Name of an OS Distribution.
	\param release Release name of the Distribution.
	\param kernel Kernel version of the Release.
	\param arch Architecture the Kernel compiled for.
  */
  OsInfo(std::string distribution, std::string release, std::string kernel, std::string arch) :
	distroName_(distribution), releaseName_(release), kernelVersion_(kernel), hardwareArch_(arch)
  {
  }


  //! Change name of the Distribution.
  /*!
	New value assigned to the #distroName_.
	
	\param distribution Distribution Name of an OS.
  */
  void setDistro (std::string distribution)
  {
	distroName_ = distribution;
  }


  //! Retrieve name of the Distribution.
  /*! 
	
	\return Distribution name of the OS.
  */
  std::string getDistro (void)
  {
	return distroName_;
  }


  //! Change name of the Release.
  /*!
	New value assigned to the #releaseName_.
	
	\param release Release Name of a Distribution.
  */
  void setRelease (std::string release)
  {
	releaseName_ = release;
  }


  //! Retrieve name of the Release.
  /*! 
	
	\return Release name of the Distribution.
  */
  std::string getRelease (void)
  {
	return releaseName_;
  }


  //! Change Kernel version.
  /*!
	New value assigned to the #kernelVersion_.
	
	\param kernel Kernel Version of a Release.
  */
  void setKernel (std::string kernel)
  {
	kernelVersion_ = kernel;
  }


  //! Retrieve Kernel version of the Release.
  /*! 
	
	\return Kernel version of the Release.
  */
  std::string getKernel (void)
  {
	return kernelVersion_;
  }


  //! Change the Kernel Architecture.
  /*!
	New value assigned to the #hardwareArch_.
	
	\param arch Machine Architecture.
  */
  void setArch (std::string arch)
  {
	hardwareArch_ = arch;
  }


  //! Retrieve the architecture the Kernel compiled for.
  /*!
	
	\return The Architecture the Kernel compiled for.
  */
  std::string getArch (void)
  {
	return hardwareArch_;
  }

};


#endif	// _OS_INFO_
