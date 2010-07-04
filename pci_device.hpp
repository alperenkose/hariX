// Hey Emacs -*- C++ -*-

#if !defined( _PCI_DEVICE_ )
#define _PCI_DEVICE_


class PciDevice
{
private:
  std::string vendor_, device_, subvendor_, subdevice_, classc_, subclass_, progif_, classMask_, module_;
  std::string vendorName_, deviceName_, subsysName_, className_, subclassName_, progifName_;
  
public:
  PciDevice()
  {
  }
  
  void setModule ( const std::string module_name )
  {
	module_ = module_name;
  }
  std::string getModule () const
  {
	return module_;
  }

  void setVendor ( const std::string vendor_code )
  {
	vendor_ = vendor_code;
  }
  std::string getVendor () const
  {
	return vendor_;
  }

  void setDevice ( const std::string device_code )
  {
	device_ = device_code;
  }
  std::string getDevice () const
  {
	return device_;
  }

  void setSubvendor ( const std::string subvendor_code )
  {
	subvendor_ = subvendor_code;
  }
  std::string getSubvendor () const
  {
	return subvendor_;
  }

  void setSubdevice ( const std::string subdevice_code )
  {
	subdevice_ = subdevice_code;
  }
  std::string getSubdevice () const
  {
	return subdevice_;
  }

  void setClass ( const std::string class_code )
  {
	classc_ = class_code;
  }
  std::string getClass () const
  {
	return classc_;
  }

  void setSubclass ( const std::string subclass_code )
  {
	subclass_ = subclass_code;
  }
  std::string getSubclass () const
  {
	return subclass_;
  }

  void setProgif ( const std::string progif_code )
  {
	progif_ = progif_code;
  }
  std::string getProgif () const
  {
	return progif_;
  }

  void setClassMask ( const std::string class_mask )
  {
	classMask_ = class_mask;
  }
  std::string getClassMask () const
  {
	return classMask_;
  }

  // void printPciDevice () const
  // {
  // 	wApp->log("debug") << module_ << " " << vendor_ << " " << device_ << " " << subvendor_ << " "
  // 					   << subdevice_ << " " << classc_ << " " << classMask_;
  // }
  
};

#endif // _PCI_DEVICE_
