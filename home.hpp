/*!
  \file   home.hpp
  \date   Thu Jul 15 13:23:44 2010
  
  \brief  HomeWidget Class Interface
  
  
*/
#if !defined ( _HOME_ )
#define  _HOME_

#include <Wt/WContainerWidget>
#include <Wt/WDialog>

namespace Wt {
  class WOverlayLoadingIndicator;
}

//! Home Page representation
/*! 
  Contains all the elements forming the Home page.
  
  Only one instance exists per session.
 */
class HomeWidget : public Wt::WContainerWidget
{
private:
  //! Constructor.
  /*!
	Private to accept calls just from #Instance() function.
	This way creation of HomeWidget objects with direct calls to constructor is rejected.
	
	\param parent Parent container of HomeWidget instance
  */
  HomeWidget( Wt::WContainerWidget* parent);

  
  //! Pointer to the only instance of HomeWidget per session.
  static HomeWidget* instance_;

  
  Wt::WContainerWidget* parent_; /*!< Parent container of HomeWidget instance. */


  //! 'Update PCI IDs' button declaration.
  Wt::WPushButton *bPciIds_;
  

  //! Triggered on 'Update PCI IDs' button click.
  /*!
	Called on #bPciIds_ button click event.

	Downloads up-to-date 'pci.ids' file from http://pciids.sourceforge.net/v2.2/pci.ids
	and updates the database with current PCI IDs.
   */
  void bPciIds_Click();


  //! Dialog window to display succes or failure of 'pci.ids' download.
  Wt::WDialog *dialogDownload_;


  //! Triggered on #dialogDownload_ close.
  /*!
	Depending on code value:
	<ul>
	<li>WDialog::Accepted\n
	Succesful download of 'pci.ids' file, update PCI IDs calling the update_pci_ids() function.\n
	Check whether update is successful and display #dialogPciIds_ dialog window.
	<li>WDialog::Rejected\n
	Download failure, do nothing.
	</ul>
	\param code #dialogDownload_ close status; WDialog::Accepted or WDialog::Rejected.
   */
  void dialogDownload_Close( Wt::WDialog::DialogCode code );
  


  //! Dialog window to display succes or failure of PCI IDs update.
  Wt::WDialog *dialogPciIds_;
  

  //! Triggered on #dialogPciIds_ close.
  void dialogPci_Close( Wt::WDialog::DialogCode code );



  //! Holds the proxy address string during session.
  std::string settingsProxy_;


  //! Reads the previously stored proxy address in server.
  /*! 
	Reads the proxy address stored in '/tmp/proxy.harix' and puts it into #settingsProxy_ variable.
   */
  void readProxyFile();


  //! Region to edit proxy address.
  Wt::WLineEdit* editProxy_;


  //! Triggered on 'Save' button click for proxy address.
  /*! 
	Stores the given proxy address in #editProxy_ to '/tmp/proxy.harix',
	and displays #dialogWarn_ in case of failure.
   */
  void changeProxy();


  //! Dialog window to display failure of proxy address saving.
  Wt::WDialog *dialogWarn_;


  //! Triggered on #dialogWarn_ close.
  void dialogWarn_Close( Wt::WDialog::DialogCode code );


  Wt::WOverlayLoadingIndicator *loading_; /*!< Busy updating indicator while updating PCI IDs. */
  
public:
  //! Retrieve an HomeWidget instance.
  /*!
	Ensures that there is only one instance of HomeWidget exists per session!
	
	\param parent Parent container of HomeWidget instance
	
	\return Pointer to an HomeWidget instance.
  */
  static HomeWidget* Instance( WContainerWidget* parent = 0 );
};


#endif // _HOME_


