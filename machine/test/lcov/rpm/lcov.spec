Summary: LTP GCOV extension code coverage tool
Name: lcov
Version: 1.6
Release: 1
License: GPL
Group: Development/Tools
URL: http://ltp.sourceforge.net/coverage/lcov.php
Source0: http://ltp.sourceforge.net/coverage/tools/lcov-%{version}.tar.gz
BuildRoot: /var/tmp/%{name}-%{version}-root
BuildArch: noarch

%description
LCOV is an extension of GCOV, a GNU tool which provides information
about what parts of a program are actually executed (i.e. "covered")
while running a particular test case. The extension consists of a set
of PERL scripts which build on the textual GCOV output to implement
HTML output and support for large projects.

%prep
%setup -q -n lcov-%{version}

%build
exit 0

%install
rm -rf $RPM_BUILD_ROOT
make install PREFIX=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr/bin
/usr/share
/etc

%changelog
* Mon Aug 20 2007 Peter Oberparleiter (Peter.Oberparleiter@de.ibm.con)
- fixed "Copyright" tag
* Mon Jul 14 2003 Peter Oberparleiter (Peter.Oberparleiter@de.ibm.con)
- removed variables for version/release to support source rpm building
- added initial rm command in install section
* Mon Apr 7 2003 Peter Oberparleiter (Peter.Oberparleiter@de.ibm.com)
- implemented variables for version/release
* Fri Oct 8 2002 Peter Oberparleiter (Peter.Oberparleiter@de.ibm.com)
- created initial spec file
