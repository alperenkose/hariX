// Hey Emacs -*- C++ -*-
/*!
  \file   pcimapQuery.hpp
  \date   Thu Jul 22 14:24:26 2010
  
  \brief  PcimapQueryWidget Class Interface
*/
#if !defined( _PCIMAPQUERY_ )
#define _PCIMAPQUERY_

#include <Wt/WContainerWidget>

class PciDevice;


//! Query Devices Page representation
/*!
  Contains all the elements forming the Query Devices page.

  Here the list of PCI devices('lspci -mn' output), user put in a text area, are read
  and passed to the PcimapResultWidget (Query Devices Result page).

  An instance exists for session life long, and each session having their own instance.
*/
class PcimapQueryWidget : public Wt::WContainerWidget
{
private:
  
  //! Constructor.
  /*! 
	Private to accept calls just from Instance() function.
	This way creation of PcimapQueryWidget objects with direct calls to constructor is rejected.
	
	\param parent Parent container of PcimapQueryWidget instance.
  */
  PcimapQueryWidget( Wt::WContainerWidget* parent);


  //! Pointer to the only instance of PcimapQueryWidget per session.
  static PcimapQueryWidget* instance_;

  
  Wt::WContainerWidget* parent_; /*!< Parent container of PcimapQueryWidget instance. */

  
  //! Text area that holds the pci device list(lspci -mn).
  Wt::WTextArea* pcimapList_;

  
  //! 'Query Devices' button declaration.
  Wt::WPushButton* bQuery_;		/*!< Binded to readLspciList() function on click event. */

  
  //! Holds the list of PCI devices read temporarily.
  static std::vector<PciDevice> lspci_list_;


  
  //! Triggered on 'Query Devices' button click.
  /*! 
	Read the list of PCI devices from #pcimapList_ and put them into #lspci_list_.
	Finally redirect to PcimapResultWidget (Query Devices Result page).
   */
  void readLspciList();

  

  //! Delete the temporary held PCI Device list.
  /*! 
	Deletes the temporary held PCI Device list in #lspci_list_.
   */
  void destroyLspciList();


  
public:
  //! Retrieve an PcimapQueryWidget instance.
  /*!
	Ensures that there is only one instance of PcimapQueryWidget exists per session!
	
	\param parent Parent container of PcimapQueryWidget instance.
	
	\return Pointer to an PcimapQueryWidget instance.
  */
  static PcimapQueryWidget* Instance( WContainerWidget* parent = 0 );

  
  //! Clear the Text area and PCI Device list.
  /*! 
	Clear contents of #pcimapList_ and temprorarily stored PCI Device list.
   */
  void resetAll();


  //! Get the list of temprorarily stored PCI Devices.
  /*! 
	\warning Not used in the project for now!
	
	\return A reference to temporarily stored PCI Device list.
  */
  static std::vector<PciDevice>& getLspciList();

};

#endif	// _PCIMAPQUERY_
