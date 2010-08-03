/*!
  \file   mainboards.hpp
  \date   Tue Jul 27 11:09:13 2010
  
  \brief  MainboardsWidget Class Interface
  
*/
#if !defined( _MAINBOARDS_ )
#define  _MAINBOARDS_

#include <Wt/WContainerWidget>

namespace Wt {
  class WPanel;
  class WStandardItemModel;
  class WSortFilterProxyModel;
  class WDialog;
}

class PciDevice;



//! Mainboards Page representation
/*! 
  Contains all the elements forming the Mainboards page.

  Mainboards stored in database are listed here. Saved Mainboard
  configurations can be viewed and deleted. Also it can be checked
  how a Mainboard is supported by a selected Operating System.

  An instance is created to show the page in each time, and instance is deleted
  when browsing to another page.
*/
class MainboardsWidget : public Wt::WContainerWidget
{
  
private:

  //! Constructor.
  /*!
	Private to accept calls just from Instance() function.
	This way creation of MainboardsWidget objects with direct calls to constructor is rejected.

	Create the layout of page and set source models for mainboards selection box and OS combo boxes.
	
	\param parent Parent container of MainboardsWidget instance.
  */
  MainboardsWidget( Wt::WContainerWidget* parent);


  //! Destructor.
  /*! 
	Clear #instance_ variable.
  */
  ~MainboardsWidget();

  Wt::WContainerWidget* parent_; /*!< Parent container of MainboardsWidget instance. */

  //! Pointer to the only instance of MainboardsWidget per session.
  static MainboardsWidget* instance_;


  
  //! Hold list of mainboards previously saved in database. ( boardID, boardName )
  std::multimap<std::string, std::string> mainboard_list_; /*!< Holds actual mainboards retrieved from database.*/

  
  //! Source for #boardsProxyModel_ holding stored mainboards.
  /*!
	This model is filled from the #mainboard_list_. Column 0 of model is
	holding the mainboard IDs from database and column 1 is holding the name of mainboards.
	\sa fillMainboards().
  */
  Wt::WStandardItemModel* mainboards_;


  //! Source for Mainboards selection box.
  /*! 
	This proxy model does not store data itself,
	but presents data from a source model(in this case it's #mainboards_).
	It allows sorting and filtering of the source model data without actually altering the source model.

	Source model for #boardsSelection_.
  */
  Wt::WSortFilterProxyModel *boardsProxyModel_;


  //! Selection box of Mainboards.
  /*!
	Provides a list box with single selection holding list of mainboards.
	Uses #boardsProxyModel_ as it's source.
  */
  Wt::WSelectionBox *boardsSelection_;



  //! Retrieve previously saved mainboards.
  /*!
	Retrieve mainboards from database and put them into #mainboard_list_.
	Then fill in the source model #mainboards_. 
	
	\param boardsModel Source model to be filled in.
  */
  void fillMainboards();

  
  
  //! Triggered on 'Show' button click.
  /*!
	Retrieve mainboard ID and call showBoardConfiguration() function.
  */
  void bBoardDevices_Click();


  //! Switch to PcimapResultWidget to display mainboard configuration.
  /*!
	Retrieve PCI devices belonging to the mainboard with ID = board_id.
	And switch to Query Devices Result page and delete current instance of MainboardsWidget.
	
	\param board_id ID of Mainboard to be displayed.
  */
  void showBoardConfiguration( std::string board_id  );



  //! Retrieve list of devices for the mainboard with passed ID.
  /*!
	First query Unique IDs of the devices belonging to the mainboard.
	Then get their codes and make list of \ref PciDevice's.
	
	\param board_id ID of Mainboard to query it's devices.
	
	\return List of PCI Devices.
  */
  std::vector<PciDevice> getBoardDeviceList( std::string board_id  );


  //! Triggered on 'Delete' button click.
  /*! 
	Ask user for confirmation of deletion with #dialogWarn_ window
	and bind the dialog window to dialogWarn_Close() function on close event.
  */
  void bDeleteBoard_Click();


  //! Dialog window used for confirmation of delete and success/failure status.
  /*!
	Used for two purposes:
	- Ask user for confirmation of mainboard deletion.
	- Display success or failure of mainboard deletion.

	\sa bDeleteBoard_Click(), deleteBoard(), dialogWarn_Close()
  */
  Wt::WDialog *dialogWarn_;

  
  //! Triggered on mainboard delete confirmation dialog close.
  /*!
	Depending on code value:
	<ul>
	<li>WDialog::Accepted\n
	User confirmed deletion of mainboard, so call deleteBoard() function.\n
	<li>WDialog::Rejected\n
	Do nothing.
	</ul>
	\param code #dialogWarn_ close status; WDialog::Accepted or WDialog::Rejected.
  */
  void dialogWarn_Close( Wt::WDialog::DialogCode code );


  //! Delete the selected mainboard.
  /*!
	First get the ID of selected mainboard, then delete the mainboard from database
	calling function deleteBoardModel().
	Display #dialogWarn_ about success status, and finally refresh the page.
  */
  void deleteBoard();
  

  //! Recreate the page.
  /*! 
	Call resetAll() function and redirect to same page.
  */
  void refreshPage();



  //! Combo box holding list of Distributions from #osModel_.
  /*! 
	Binded to osSelectionChanged() function on selection change.

	\sa fillOsModel()
   */
  Wt::WComboBox *comboOS_;

  //! Combo box holding list of Releases from #releaseModel_.
  /*!
	Binded to releaseSelectionChanged() function on selection change.

	\sa fillReleaseModel()
  */
  Wt::WComboBox	*comboRel_;

  //! Combo box holding list of Kernels from #kernelModel_.
  /*! 

	\sa fillKernelModel()
  */
  Wt::WComboBox	*comboKer_;
  
  Wt::WStandardItemModel *osModel_,	/*!< Source model for #comboOS_ holding OS Dists retrieved from database.
									 \sa fillOsModel() */
	*releaseModel_,					/*!< Source model for #comboRel_ holding Releases retrieved from database.
									 \sa fillReleaseModel() */
	*kernelModel_;					/*!< Source model for #comboKer_ holding Kernels retrieved from database.
									 \sa fillKernelModel() */


  //! Fill in the #osModel_.
  /*! 
	Retrieve list of Distributions in database(OS Distribution ID - OS name) and put them in #osModel_.
   */
  void fillOsModel();

  //! Fill in the #releaseModel_.
  /*!
	Retrieve list of Releases of a Distribution(Release ID - Release name) with passed OS Distribution ID,
	and put them in #releaseModel_.
	
	\param os_id ID of an OS Distribution.
  */
  void fillReleaseModel( std::string os_id );

  //! Fill in the #kernelModel_.
  /*!
	Retrieve list of Kernels of a Release(Unique Kernel ID - Kernel version/arch)
	with passed Release ID,	and put them in #kernelModel_.
	
	\param release_id ID of a Distribution Release.

	\warning Unique Kernel ID, means a unique ID for each
	OS(Distribution-Release-Kernel all together) in database.
  */
  void fillKernelModel( std::string release_id );

  

  //! Triggered on selection change of #comboOS_.
  /*!
	If an OS Distribution is selected, then calls fillReleaseModel() function
	with selected distribution ID.
	
	\param selected_index Selected index of Distribution combo box, not used!
  */
  void osSelectionChanged( int selected_index ); 		// Signal..


  //! Triggered on selection change of #comboRel_.
  /*!
	If a Distribution Release is selected, then calls fillKernelModel() function
	with selected release ID.
	
	\param selected_index Selected index of Release combo box, not used!
  */
  void releaseSelectionChanged( int selected_index );	// Signal..


  //! 'Check' button declaration.
  Wt::WPushButton *bCheckOs_;


  //! Triggered on 'Check' button click.
  /*! 
	Called on #bCheckOs_ button click event.

	Get the list of PCI Devices belonging to the selected mainboard and
	switch to the PcimapResultWidget providing Device List, Mainboard name and
	OS name. Which will result in an additional Support column in
	PcimapResultWidget::lspciTable_ showing if the device at line is supported by passed OS.
   */
  void bCheckOs_Click();



  //! Retrieve selected Mainboard's ID.
  /*! 
	Get index of mainboard currently selected in #boardsSelection_ by
	mapping it's index to #boardsProxyModel_ on column 0(holding IDs) and
	consecutively mapping it to #mainboards_ to get the selected mainboard's ID.
	
	\return ID of selected Mainboard.
  */
  std::string getBoardIdentifier();



  //! Retrieve the selected OS's Unique ID in ComboBoxes.
  /*! 
	Get the Unique Kernel ID(representing an OS) from #kernelModel_ of
	#comboKer_.
	
	\return An ID representing the selected OS from ComboBoxes.
  */
  std::string getOsIdentifier();

  
public:

  //! Create and Retrieve an instance.
  /*!
	Check whether an instance exists, create an instance
	by calling the constructor and return it to the caller.
	
	\param parent Parent container of MainboardsWidget instance.
	
	\return Pointer to an MainboardsWidget instance.

	\todo Might be better to divide this into two as in PcimapResultWidget!
  */
  static MainboardsWidget* Instance( WContainerWidget* parent = 0 );


  //! Remove page and delete it.
  /*! 
	Removes the widget from HarixApp::stackedContent_ and
	deletes the instance.
  */
  void resetAll();

};


#endif //  _MAINBOARDS_
