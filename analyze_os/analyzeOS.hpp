// Hey Emacs -*- C++ -*-
/*!
  \file   analyzeOS.hpp
  \date   Fri Jul 16 09:57:47 2010
  
  \brief  AnalyzeOsWidget Class Interface
*/
#if !defined( _ANALYZEOS_ )
#define _ANALYZEOS_

#include <Wt/WContainerWidget>
#include <Wt/WDialog>

namespace Wt {
  class WPanel;
  class WTable;
}

class OsInfo;

//! Analyze OS Page representation
/*! 
  Contains all the elements forming the Analyze OS page.

  This is where new OSes are added to the database
  together with their PCI device support list (modules.pcimap).

  An instance exists for session life long, and each session having their own instance.
 */
class AnalyzeOsWidget : public Wt::WContainerWidget
{
private:
  //! Constructor.
  /*! 
	Private to accept calls just from Instance() function.
	This way creation of AnalyzeOsWidget objects with direct calls to constructor is rejected.
	
	\param parent Parent container of AnalyzeOsWidget instance.
  */
  AnalyzeOsWidget( Wt::WContainerWidget* parent);

  //! Pointer to the only instance of AnalyzeOsWidget per session.
  static AnalyzeOsWidget* instance_;

  Wt::WContainerWidget* parent_; /*!< Parent container of AnalyzeOsWidget instance. */

  //! Holds the detected OS information.
  OsInfo* osDist_;

  //! Path to the uploaded 'distro.txt' file.
  std::string os_file_;
  //! Path to the uploaded 'modules.pcimap' file.
  std::string pcimap_file_;



  /**
   * \name Upload Screen
   * Private Members and Attributes used in Upload Screen section of Analyze OS 
   * @{
   */
  //! To build the layout structure of Anayze OS upload page.
  Wt::WTable* layoutAnalyze_;

  //! File Upload area declaration for distro.txt.
  Wt::WFileUpload *uploadOs_;
  //! File Upload area declaration for modules.pcimap.
  Wt::WFileUpload *uploadPcimap_;
  //! 'Upload' button declaration.
  Wt::WPushButton *bUpload_;	/*!< Binded to bUpload_Click() function on click event. */

  
  //! Triggered on 'Upload' button click.
  /*!
	Called on #bUpload_ button click event.

	Calls 'upload' functions of #uploadOs_ and #uploadPcimap_ File Uploads, causing
	them to signal their 'uploaded' events which will trigger respectively
	osUploaded() and pcimapUploaded() functions binded to the events.
	Binding of 'uploaded' events to the functions are shown below:
	\code
	uploadOs_->uploaded().connect(this, &AnalyzeOsWidget::osUploaded);
	uploadPcimap_->uploaded().connect(this, &AnalyzeOsWidget::pcimapUploaded);
	\endcode
  */
  void bUpload_Click();

  static bool isOsUploaded_;		/*!< Set to true after a successful upload of 'distro.txt' file. */
  static bool isPcimapUploaded_; 	/*!< Set to true after a successful upload of 'modules.pcimap' file. */

  //! Called on uploaded() event of #uploadOs_ File Upload.
  /*! 
	Validates that the name of the uploaded file is equal to "distro.txt".

	If successful, sets variable #isOsUploaded_ to true and calls uploadDone() function.
  */
  void osUploaded();
  

  //! Called on uploaded() event of #uploadPcimap_ File Upload.
  /*! 
	Validates that the name of the uploaded file is equal to "modules.pcimap".

	If successful, sets variable #isPcimapUploaded_ to true and calls uploadDone() function.
  */
  void pcimapUploaded();


  //! Check upload status and change to Analyze Result screen.
  /*! 
	If both #isOsUploaded_ and #isPcimapUploaded_ keys are true,
	read Operating System information and change to Analyze Result screen.
  */
  void uploadDone();


  //! Read OS information and store it.
  /*! 
	Reads the OS information from uploaded file(#os_file_) and
	stores it in an OsInfo object. Then provides information to
	Analyze Result screen by fillDetectedOs() function call.
  */
  void readOs();
  /**
   * Upload Screen end
   * @}
   */





  
  /**
   * \name Result Screen
   * Private Members and Attributes used in Result Screen section of Analyze OS
   * @{
   */
  
  //! Fill Edit regions of Analyze Result screen.
  /*! 
	Gets info from OsInfo instance and puts them into edit regions
	#editOs_, #editRel_, #editKer_, #editArch_ respectively.
  */
  void fillDetectedOs();

  //! To build the layout structure of Anayze OS Result page.
  Wt::WTable* layoutResult_;

  //! Group OS info edit regions in here.
  Wt::WGroupBox* groupDetectedOs_;

  
  //! Edit region holding Operating System Name.
  /*! 
	Editable on click with Save and Cancel buttons.

	Signals it's valueChanged() event on 'Save' click,
	which is binded to changeOsName() function with the new value
	passed as parameter.
	\code
	editOs_->valueChanged().connect( this, &AnalyzeOsWidget::changeOsName );
	\endcode
   */
  Wt::WInPlaceEdit *editOs_;

  
  //! Edit region holding Release Name.
  /*! 
	Editable on click with Save and Cancel buttons.

	Signals it's valueChanged() event on 'Save' click,
	which is binded to changeRelease() function with the new value
	passed as parameter.
	\code
	editRel_->valueChanged().connect( this, &AnalyzeOsWidget::changeRelease );
	\endcode
  */
  Wt::WInPlaceEdit *editRel_;

  
  //! Edit region holding Kernel Version.
  /*! 
	Editable on click with Save and Cancel buttons.

	Signals it's valueChanged() event on 'Save' click,
	which is binded to changeKernel() function with the new value
	passed as parameter.
	\code
	editKer_->valueChanged().connect( this, &AnalyzeOsWidget::changeKernel );
	\endcode
  */
  Wt::WInPlaceEdit *editKer_;


  //! Edit region holding Kernel Machine Architecture.
  /*! 
	Editable on click with Save and Cancel buttons.

	Signals it's valueChanged() event on 'Save' click,
	which is binded to changeArch() function with the new value
	passed as parameter.
	\code
	editArch_->valueChanged().connect( this, &AnalyzeOsWidget::changeArch );
	\endcode
  */
  Wt::WInPlaceEdit *editArch_;


  //! 'Check Record' button declaration.
  Wt::WPushButton *bCheckOs_;	/*!< Binded to bCheckOs_Click() function on click event. */


  //! Triggered on 'Check Record' button click.
  /*!
	Called on #bCheckOs_ button click event.
	
	Checks if an OS is already stored in database calling queryOsKernelId()
	with the current OS information passed.

	And displays #layoutCheckResult_ whether to update or add OS.
   */
  void bCheckOs_Click();


  //! Triggered when 'OS Name' edited.
  /*!
	Called when the value of #editOs_ is changed.
	
	\param os_name New value in #editOs_.
  */
  void changeOsName( Wt::WString os_name );

  
  //! Triggered when 'Release' edited.
  /*!
	Called when the value of #editRel_ is changed.
	
	\param os_name New value in #editRel_.
  */
  void changeRelease( Wt::WString release );


  //! Triggered when 'Kernel' edited.
  /*!
	Called when the value of #editKer_ is changed.
	
	\param os_name New value in #editKer_.
  */
  void changeKernel( Wt::WString kernel );


  //! Triggered when 'Arch.' edited.
  /*!
	Called when the value of #editArch_ is changed.
	
	\param os_name New value in #editArch_.
  */
  void changeArch( Wt::WString arch );
  

  //! Layout structure of OS Check Record result.
  /*! 
	Holding #txtOsResult_, #bOsAddUpdate_ and #bOsCancel_ elements.
  */
  Wt::WTable* layoutCheckResult_;

  Wt::WText* txtOsResult_;		/*!< Holding message displaying if OS found in database or not. */

  //! Declaration of 'Add OS!'/'Update Pcimap!' button.
  Wt::WPushButton *bOsAddUpdate_; /*!< Binded to bOsAddUpdate_Click() function on click event. */

  //! Triggered on 'Add OS!' or 'Update Pcimap!' button click. 
  /*! 
	Called on #bOsAddUpdate_ button click event.

	Updates OS and pcimap information in database by calling os_pci_module() function
	with OS information and modules.pcimap file.
	
	Displays #dialogResult_ dialog window appropriately whether update/addition was successful or not.
  */
  void bOsAddUpdate_Click();


  //! Declaration of 'Cancel / Edit' button.
  Wt::WPushButton *bOsCancel_;	/*!< Binded to bOsCancel_Click() function on click event. */
  
  //! Triggered on 'Cancel / Edit' button click. 
  /*! 
	Called on #bOsCancel_ button click event.

	Hides #layoutCheckResult_.
  */
  void bOsCancel_Click();


  //! Dialog window displaying success or failure of OS-Pcimap update.
  Wt::WDialog*  dialogResult_;


  //! Triggered on #dialogResult_ close.
  /*!
	If insertion/update is successful go to home page.
	
	\param code #dialogResult_ close status; WDialog::Accepted or WDialog::Rejected.
  */
  void redirectAndDestroyDialog( Wt::WDialog::DialogCode code );


  //! Go to Home page.
  /*! 
	Called from redirectAndDestroyDialog() function.
   */
  void goHome();
  
  /**
   * Result Screen end
   * @}
   */



public:

  //! Retrieve an AnalyzeOsWidget instance.
  /*!
	Ensures that there is only one instance of AnalyzeOsWidget exists per session!
	
	\param parent Parent container of AnalyzeOsWidget instance
	
	\return Pointer to an AnalyzeOsWidget instance.
  */
  static AnalyzeOsWidget* Instance( WContainerWidget* parent = 0 );


  
  //! Reset instance to initial state.
  /*! 
	Resets File Upload areas and Sets display to Upload Screen.
   */
  void resetAll();
  
};

#endif	// _ANALYZEOS_
