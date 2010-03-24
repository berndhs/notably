
%define notably_ver 0.3
%define notably_dir notably-0.3.1

Name:    notably
Version: 0.3.1
Release: 1%{?dist}
Summary: Notably note taking application
# matches up (pretty much) with qt4
License: GPLv3
Group:   Applications/Productivity
Packager: Bernd Stramm <bernd.stramm@gmail.com>
Url:     http://notably.sourceforge.net/
Source: notably-0.3.1.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

# TODO: patches (?)

#Obsoletes:
#Provides: 

BuildRequires:  qt4-devel > 4.6

%description
Notably is a note taking and organizing application. It is designed to 
allow quick entry of notes, and organization of these notes. To organize notes,
they can be groups into books. To help with finding notes, tags can 
be put on them.


%prep
%setup

%build
%{_qt4_qmake} notably.pro
make %{?_smp_mflags}

%install

rm -rf $RPM_BUILD_ROOT
install -p -d $RPM_BUILD_ROOT%{_bindir}
install -p -m 755 viewer/notably $RPM_BUILD_ROOT%{_bindir}

%clean
rm -rf %{buildroot}


%post 

%postun 


%files
%defattr(-,root,root,-)
%doc README
%doc LICENSE.txt
%doc qrcfiles/userman.html
%{_bindir}/notably

%changelog

* Wed Mar 24 2010 Bernd Stramm <bernd.stramm@gmail.com> 
- Initial RPM release




