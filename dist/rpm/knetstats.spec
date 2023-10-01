Name:		knetstats
Version:	2.0.0.alpha
Release:	1
Summary:	Network interface and statistics viewer

Group:		Productivity/Network/Viewers
License:	GPL-2
URL:		https://github.com/telans/KNetStats
Source:	    https://github.com/telans/KNetStats/archive/v%{version}.tar.gz

BuildRequires: cmake
BuildRequires: pkgconfig
BuildRequires: qt5-qtbase-devel
BuildRequires: kf5-kwidgetsaddons-devel
BuildRequires: extra-cmake-modules

%description
KNetStats is a simple Qt network interface and statistics viewer with an associated tray icon to monitor activity.

%prep
%autosetup -n KNetStats

%build
%cmake
%cmake_build

%install
%cmake_install

%files
/usr/bin/*
/usr/share/icons/*
/usr/share/applications/*
/usr/share/metainfo/*
