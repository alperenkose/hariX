
#if !defined( _OS_INFO_ )
#define _OS_INFO_

class OsInfo
{
private:
  std::string distroName_, distroRelease_, kernelRelease_, hardwareArch_;
public:
  OsInfo(std::string distribution, std::string release, std::string kernel, std::string arch) :
	distroName_(distribution), distroRelease_(release), kernelRelease_(kernel), hardwareArch_(arch)
  {
  }

  void setDistro (std::string distribution)
  {
	distroName_ = distribution;
  }

  std::string getDistro (void)
  {
	return distroName_;
  }

  void setRelease (std::string release)
  {
	distroRelease_ = release;
  }

  std::string getRelease (void)
  {
	return distroRelease_;
  }
  
  void setKernel (std::string kernel)
  {
	kernelRelease_ = kernel;
  }

  std::string getKernel (void)
  {
	return kernelRelease_;
  }
  
  void setArch (std::string arch)
  {
	hardwareArch_ = arch;
  }

  std::string getArch (void)
  {
	return hardwareArch_;
  }

};


#endif	// _OS_INFO_
