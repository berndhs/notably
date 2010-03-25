
%define notably_ver 0.3
%define notably_dir notably-0.3.1

Name:    notably
Version: 0.3.1
Release: 1
Summary: Notably note taking application

License: GPLv3
Group:   Applications/Productivity
Packager: Bernd Stramm <bernd.stramm@gmail.com>
Url:     http://notably.sourceforge.net/
Source: notably-0.3.1.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

# TODO: patches (?)
%define myqmake qmake


#Obsoletes:
#Provides: 
%if 0%{?fedora_version} || 0%{?rhel_version} || 0%{?centos_version}
BuildRequires:  qt4-devel
BuildRequires:  gcc-c++
%if 0%{?fedora_version} || 0%{?rhel_version}
%define myqmake %{_qt4_qmake}
%endif
%endif

%if 0%{?suse_version}
BuildRequires: libqt4-devel
BuildRequires: libQtWebKit-devel
BuildRequires: gcc-c++
%endif
%if 0%{?mandriva_version}
BuildRequires: qt4-devel
BuildRequires: gcc-c++
%endif

%description
Notably is a note taking and organizing application. It is designed to 
allow quick entry of notes, and organization of these notes. To organize notes,
they can be groups into books. Notes can be tagged to make them 
easier to find.


%prep
%setup

%build
%{myqmake} notably.pro
make 

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




