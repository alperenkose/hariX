#!/bin/bash
#
# Shell script which detects the Linux distro it's running on
#
# Returned distro       Version the script was tested on
# ---------------------------------------------------------
# opensuse              openSuSE 11.0 (no lsb_release) and 11.2 (lsb_release)
# fedora                Fedora 12
# centos                CentOS 5.4
# kubuntu               Kubuntu 9.10
# debian                Debian 5.0.3
# arch                  Arch
# slackware             Slackware 13.0.0.0.0
# mandriva              Mandriva 2009.1
# debian		            Knoppix 6.2
# linuxmint		         Mint 8
#
# 10/02/17 framp at linux-tips-and-tricks dot de

# Based on the queryDistro.sh script by "framp" at "linux-tips-and-tricks dot de"
# Tested on SLES 10, SLED 11, RHEL 5.5, Ubuntu 9.10, Arch Linux
# Since Arch Linux is a rolling release distribution release is assumed to be last system upgrade
# Alp Eren Koese
# alperenkose@gmail.com


detectedDistro="Unknown"
detectedDesc="Unknown"
detectedRelease="Unknown"
regExpLsbDist="Distributor ID:[[:space:]]*([^ ]*)"
regExpLsbDesc="Description:[[:space:]]*([^ ]*)"
regExpLsbFile="/etc/(.*)[-_]"
regExpLsbRel="Release:[[:space:]]*([^ ]*)"
regExpKerRel="[0-9]+\.[0-9]+\.[0-9]+.*"


if [ `which lsb_release 2>/dev/null` ]; then       # lsb_release available
   lsbDist=`lsb_release -i`
   if [[ $lsbDist =~ $regExpLsbDist ]]; then
      detectedDistro=${BASH_REMATCH[1]}
   else
      echo "??? Should not occur: Don't find distro name in lsb_release output ???"
      exit 1
   fi

   lsbRel=`lsb_release -r`
   if [[ $lsbRel =~ $regExpLsbRel ]]; then
      detectedRelease=${BASH_REMATCH[1]}
   else
      echo "??? Should not occur: Don't find release name in lsb_release output ???"
      exit 1
   fi

# Description and Codename shall be fetched as well..

else                                               # lsb_release not available
   etcFiles=`ls /etc/*[-_]{release,version} 2>/dev/null`
   for file in $etcFiles; do
      if [[ $file =~ $regExpLsbFile ]]; then
         detectedDistro=${BASH_REMATCH[1]}

	 if [ $detectedDistro == "mandrake" ] ; then
		detectedDistro='Mandrake'
		#PSUEDONAME=`cat /etc/mandrake-release | sed s/.*\(// | sed s/\)//`
		detectedRelease=`cat /etc/mandrake-release | sed s/.*release\ // | sed s/\ .*//`
       	 elif [ $detectedDistro == "debian" ] ; then
		detectedDistro="Debian"
		detectedRelease="`cat /etc/debian_version`"
	 elif [ $detectedDistro == "arch" ] ; then
		detectedDistro="ArchLinux"
		detectedRelease="`grep 'system upgrade' /var/log/pacman.log | tail -1 | sed -e 's/^.*\[\([^ ]*\).*/\1/'`"
	 fi
# We need Additional Distributions here!!!
	 break

      else
         echo "??? Should not occur: Don't find any etcFiles ???"
         exit 1
      fi
   done
fi

kerRel=`uname -r`
if [[ $kerRel =~ $regExpKerRel ]]; then
    kerRel=${BASH_REMATCH}
else
    echo "??? Should not occur: Don't find kernel release in uname -r output ???"
    exit 1
fi

kerHard=`uname -m`
if [[ $kerHard == "" ]]; then
    echo "??? Should not occur: Couldn't find machine hardware for the kernel release in uname -m ???"
    exit 1
fi

#detectedDistro=`echo $detectedDistro | tr "[:upper:]" "[:lower:]"`

case $detectedDistro in
    SUSE)
	regExpOpenSuse="Description:[[:space:]]*openSUSE[[:space:]]*(.*)"
	regExpSled="Description:[[:space:]]*SUSE Linux Enterprise Desktop(.*)"
	regExpSles="Description:[[:space:]]*SUSE Linux Enterprise Server(.*)"

	lsbDesc=`lsb_release -d`
	if [[ $lsbDesc =~ $regExpOpenSuse ]]; then
	    detectedRelease="openSUSE $detectedRelease"
	elif [[ $lsbDesc =~ $regExpSled ]]; then
	    detectedRelease="SLED $detectedRelease"
	elif [[ $lsbDesc =~ $regExpSles ]]; then
	    detectedRelease="SLES $detectedRelease"
	fi
 	;;
    RedHatEnterpriseAS | RedHatEnterpriseWS | RedHatEnterpriseServer | RedHatEnterpriseClient)
	detectedDistro="RedHat"
	detectedRelease="RHEL $detectedRelease"
	;;
    linux)
	detectedDistro="linuxmint" ;;
esac



echo $detectedDistro >> distro.txt
echo $detectedRelease >> distro.txt
echo $kerRel >> distro.txt
echo $kerHard >> distro.txt
