/*!
  \file   pcimapResult.hpp
  \date   Thu Jul 22 17:23:51 2010
  
  \brief  PcimapResultWidget Class Interface
*/
#if !defined ( _PCIMAPRESULT_ )
#define _PCIMAPRESULT_

#include <Wt/WContainerWidget>
#include <Wt/WDialog>

namespace Wt{
  class WPanel;
  class WStandardItemModel;
  class WStandardItem;
  class WTableView;
  class WModelIndex;
}

class PciDevice;


//! Query Devices Results Page representation
/*!
  Contains all the elements forming the Query Devices Results page.

  This is a multi purpose widget, mainly showing the list of PCI Devices provided to it.
  - If it is just provided with a device list, then it only displays them and their modules.
  - If provided with an additional mainboard name, it displays it's name and will not allow
  saving of the same mainboard.
  - And if provided with a unique ID representing an OS with addition to above, then it will
  put an extra Support column to show if the device is supported with the given OS.

  An instance is created to show the page in each time, and instance is deleted
  when browsing to another page.
*/
class PcimapResultWidget : public Wt::WContainerWidget
{
private:

  //! Constructor.
  /*!
	Private to accept calls just from Instance() function.
	This way creation of PcimapResultWidget objects with direct calls to constructor is rejected.
	
	\param lspci_list List of PCI Devices' Codes.
	\param parent Parent container of PcimapResultWidget instance.
	\param board_name Name of the mainboard to be displayed.
	\param ukernel_id Unique ID representing the OS to be queried for support.
  */
  PcimapResultWidget( std::vector<PciDevice>& lspci_list,
					  WContainerWidget* parent,
					  std::string board_name="", 	// passed if showing a mainboard configuration
					  std::string ukernel_id=""); 	// passed if a specific OS support queried..


  //! Destructor.
  /*! 
	Clear #instance_ and #instance_count variables,
	and the list of PCI Devices (#lspci_list_).
  */
  ~PcimapResultWidget();
  
  WContainerWidget* parent_;	/*!< Parent container of PcimapResultWidget instance. */

  //! Pointer to the only instance of PcimapResultWidget per session.
  static PcimapResultWidget* instance_;

  
  static int instance_count;	/*!< Number of instances. \warning Not of any use just for testing! */


  //! Hold the list of PCI Devices' Codes passed as parameter.
  std::vector<PciDevice> lspci_list_;

  //! Hold the Unique ID representing the OS to be queried for support.
  std::string uKernelId_;



  
  /*
   * Device List Panel
   * @{
   */
  
  //! Panel to enclose the #lspciTable_ for device listing.
  Wt::WPanel* panelTable_;		// this might be done local..


  //! Table listing the names of provided devices.
  /*!
	Binded to lspciTableRowSelected() function on a double click of a line:
	\code
	lspciTable_->doubleClicked().connect(this, &PcimapResultWidget::lspciTableRowSelected);
	\endcode

	\sa #lspciModel_
  */
  Wt::WTableView* lspciTable_;

  
  //! Source for #lspciTable_ holding the actual values to be shown.
  /*! 
	\sa pciDeviceToRowItem()
  */
  Wt::WStandardItemModel* lspciModel_;


  
  //! Create a line to be added to #lspciModel_ from given PCI Device.
  /*!
	Queries the database for Class and Device names with the given PCI Codes,
	also if #uKernelId_ is provided to the instance, adds an additional Support
	column showing whether the device is supported with current OS.
	
	\param current_item An element of #lspci_list_.
	
	\return A line to be added to the #lspciModel_.
  */
  std::vector<Wt::WStandardItem*> pciDeviceToRowItem( const PciDevice* current_item );
  
  /*
   * @}
   */



  
  //! Triggered on a double click of a line in #lspciTable_.
  /*!
	First calls the fillDeviceDetails() function, then
	queries which Operating Systems support the selected device, while also retrieving the
	modules for the purpose. Then adds the list of returned OSes to #osSupportModel_ by
	calling osToSupportRow() function.
	
	\param WModelIndex Index of selected item in #lspciTable_.
	\param WMouseEvent Not of any use in project.
  */
  void lspciTableRowSelected(Wt::WModelIndex, Wt::WMouseEvent);


  

  
  /*
   * Device Details and OS Support Panel
   * @{
   */

  //! Panel to enclose Device and OS Support Details.
  Wt::WPanel* panelSupport_;	// this might be done local..


  Wt::WLineEdit *editVendor_,	/*!< Show Vendor Code for the selected device. \sa fillDeviceDetails() */
	*editDevice_,				/*!< Show Device Code for the selected device. \sa fillDeviceDetails() */
	*editSubvendor_,			/*!< Show Subvendor Code for the selected device. \sa fillDeviceDetails() */
	*editSubdevice_,			/*!< Show Subdevice Code for the selected device. \sa fillDeviceDetails() */
	*editClass_,				/*!< Show Class Code for the selected device. \sa fillDeviceDetails() */
	*editSubclass_,				/*!< Show Subclass Code for the selected device. \sa fillDeviceDetails() */
	*editProgif_;				/*!< Show Prog-if Code for the selected device. \sa fillDeviceDetails() */

  
  //! Table listing the OSes with their Modules that support the selected device.
  Wt::WTableView* osSupportTable_;


  //! Source for #osSupportTable_ holding the actual values to be shown.
  /*! 
	\sa osToSupportRow()
  */
  Wt::WStandardItemModel* osSupportModel_;

  

  //! Fill in the fields in Device Details to show PCI Device Codes.
  /*!
	\param selected_device Selected item in #lspciTable_.
  */
  void fillDeviceDetails( const PciDevice& selected_device );

  

  //! Create a line to be added to #osSupportModel_.
  /*!
	Create a line to be added to #osSupportModel_ with received
	OS information from lspciTableRowSelected() function.
	
	\param os Holding values of OS, Release, Kernel, Arch. and Driver name.
	
	\return A line to be added to the #osSupportModel_.
  */
  std::vector<Wt::WStandardItem*> osToSupportRow( const std::vector<std::string>& os );
  
  /*
   * @}
   */



  //! Triggered on 'Save Mainboard' button click.
  /*! 
	Displays the #dialogBoard_ dialog window.
	Binded to dialogBoard_Close() function on close event.
   */
  void bSaveBoard_Click();


  //! Dialog window asking for the mainboard name to be saved.
  Wt::WDialog* dialogBoard_;

  //! Edit area for the user to input the mainboard name.
  Wt::WLineEdit* editBoard_;


  //! Triggered on #dialogBoard_ close.
  /*!
	if 'Ok' button is clicked, call storeMainBoard() function with board name as parameter.
	
	\param dialog_code #dialogBoard_ close status; WDialog::Accepted or WDialog::Rejected.
  */
  void dialogBoard_Close( Wt::WDialog::DialogCode dialog_code );


  
  //! Store Mainboard configuration in database.
  /*!
	Retrieve unique device IDs of listed PCI Devices from getUniqueDevIdList() function
	to match and store them in database with the unique board ID of currently
	recorded Mainboard.

	Finally display #dialogWarn_ about status of save.
	
	\param board_name Name of Mainboard to be recorded.
	\sa queryBoardModelId() , insertBoardModel() , insertBoardDevices()
  */
  void storeMainBoard( std::string board_name );

  
  //! Dialog window showing status of mainboard save.
  Wt::WDialog *dialogWarn_;
  
  Wt::WText *txtWarn_;			/*!< #dialogWarn_ text area displaying failure or success. */



  //! Retrieve unique device IDs of listed PCI Devices.
  /*! 
	Check if the currently listed devices(#lspci_list_) are recorded in database,
	record them if not, and retrieve Unique Device IDs representing each of them.

	Functions checkPciSpcId() and checkUniqueDeviceId() checks and records devices
	in `pci_all' and `all_devices' database tables respectively.
	
	\return List of Unique Device IDs.
  */
  std::vector<std::string> getUniqueDevIdList();
  
  
public:
  
  //! Create and Retrieve an instance.
  /*!
	Create an instance by calling the constructor and return it to the caller.

	If a mainboard configuration is listed then the board_name parameter is
	passed as well.
	If an OS support wants to be queried then both board_name and ukernel_id
	parameters are passed.
	
	\param lspci_list List of PCI Devices' Codes to be shown.
	\param parent Parent container of PcimapResultWidget instance.
	\param board_name Name of the mainboard to be displayed(Optional).
	\param ukernel_id Unique ID representing the OS to be queried for support(Optional).
	
	\return Pointer to an PcimapQueryWidget instance.
  */
  static PcimapResultWidget* Instance(std::vector<PciDevice> lspci_list,
									  WContainerWidget* parent,
									  std::string board_name="",
									  std::string ukernel_id="");


  //! Check whether an instance already exists.
  /*! 
	Return the instance if it already exists, otherwise return NULL.
	
	\return Pointer to an existing PcimapQueryWidget instance.
  */
  static PcimapResultWidget* Instance();


  //! Remove page and delete it.
  /*! 
	Removes the widget from HarixApp::stackedContent_ and
	deletes the instance.
   */
  void resetAll();
  
};

#endif // _PCIMAPRESULT_
