Name:    okular
Summary: A document viewer
Version: 4.10.5
Release: 7%{?dist}

License: GPLv2
URL:     https://projects.kde.org/projects/kde/kdegraphics/okular
%global revision %(echo %{version} | cut -d. -f3)
%if %{revision} >= 50
%global stable unstable
%else
%global stable stable
%endif
Source0: http://download.kde.org/%{stable}/%{version}/src/%{name}-%{version}.tar.xz

## upstreamable patches
# okular.desktop : Categories=-Office,+VectorGraphics (http://bugzilla.redhat.com/591089)
Patch50: kdegraphics-4.5.1-okular_dt.patch

# don't hardcode paths in OkularConfig.cmake
Patch51: kdegraphics-4.5.80-OkularConfig-dont-hardcode-paths.patch

# get rif of gcc overflow warning
Patch52: okular-gcc-overflow.patch

# don't build component if ACTIVEAPP_FOUND FALSE 
Patch53: okular-4.10-cmake.patch

Patch54: okular-add-information-about-substituting-font.patch

%if 0%{?fedora}
BuildRequires: chmlib-devel
BuildRequires: ebook-tools-devel
BuildRequires: plasma-mobile-devel
BuildRequires: pkgconfig(ddjvuapi)
%endif
BuildRequires: desktop-file-utils
BuildRequires: kdelibs4-devel >= %{version}
BuildRequires: libkipi-devel >= %{version} 
BuildRequires: libjpeg-devel
BuildRequires: libtiff-devel
BuildRequires: pkgconfig(libspectre)
BuildRequires: pkgconfig(poppler-qt4)
BuildRequires: pkgconfig(qca2)
BuildRequires: pkgconfig(qimageblitz)

# kio_msits migrates from kdegraphics, which had Epoch: 7
%define kio_msits_epoch 7
Requires: %{name}-part%{?_isa} = %{version}-%{release}
%if 0%{?fedora}
Requires: kio_msits = %{kio_msits_epoch}:%{version}-%{release}
%endif
Requires: kde-runtime >= 4.10.5

%description
%{summary}.

%package active
Summary: Document viewer for plasma active
# todo: test/confirm this dep is sufficient (or too much) -- rex
Requires: %{name}-part%{?_isa} = %{version}-%{release}
%description active
%{summary}.

%package devel
Summary:  Development files for %{name}
Requires: %{name}-libs%{?_isa} = %{version}-%{release}
Requires: kdelibs4-devel
%description devel
%{summary}.

%package  libs 
Summary:  Runtime files for %{name} 
# when split occurred
Conflicts: kdegraphics < 7:4.6.95-10
%description libs 
%{summary}.

%package part
Summary: Okular kpart plugin
Requires: %{name}-libs%{?_isa} = %{version}-%{release}
%description part
%{summary}.

%package -n kio_msits
Epoch: %{kio_msits_epoch} 
Summary: A kioslave for displaying WinHelp files
%description -n kio_msits
%{summary}.



%prep
%setup -q

%patch50 -p2 -b .okular_dt
%patch51 -p2 -b .OkularConfig
%patch52 -p1 -b .overflow
%patch53 -p1 -b .cmake
%patch54 -p1 -b .add-information-about-substituting-font

%build
mkdir -p %{_target_platform}
pushd %{_target_platform}
%{cmake_kde4} ..
popd

make %{?_smp_mflags} -C %{_target_platform}


%install
rm -rf %{buildroot}

make install/fast DESTDIR=%{buildroot} -C %{_target_platform}

%find_lang %{name} --with-kde --without-mo


%check
desktop-file-validate %{buildroot}%{_kde4_datadir}/applications/kde4/%{name}.desktop


%post
touch --no-create %{_kde4_iconsdir}/hicolor &> /dev/null || :

%posttrans
gtk-update-icon-cache %{_kde4_iconsdir}/hicolor &> /dev/null || :
update-desktop-database -q &> /dev/null ||:

%postun
if [ $1 -eq 0 ] ; then
touch --no-create %{_kde4_iconsdir}/hicolor &> /dev/null || :
gtk-update-icon-cache %{_kde4_iconsdir}/hicolor &> /dev/null || :
update-desktop-database -q &> /dev/null ||:
fi

%files -f %{name}.lang
%doc COPYING 
%{_kde4_bindir}/okular*
%{_kde4_datadir}/applications/kde4/okular.desktop
%{_kde4_datadir}/applications/kde4/okularApplication_*.desktop
%{_kde4_iconsdir}/hicolor/*/*/*
%{_mandir}/man1/okular.1*

%if 0%{?fedora}
%files active
%{_kde4_appsdir}/plasma/packages/org.kde.active.documentviewer/
%{_kde4_bindir}/active-documentviewer
%{_kde4_datadir}/applications/kde4/active-documentviewer.desktop
# not sure if this is best put here or not -- rex
%{_kde4_libdir}/kde4/imports/org/kde/okular/
%endif

%files devel
%{_kde4_includedir}/okular/
%{_kde4_libdir}/libokularcore.so
%{_kde4_libdir}/cmake/Okular/

%post libs -p /sbin/ldconfig
%postun libs -p /sbin/ldconfig

%files libs
%{_kde4_appsdir}/kconf_update/okular.upd
%{_kde4_libdir}/libokularcore.so.2*

%files part
%{_kde4_appsdir}/okular/
%{_kde4_datadir}/config.kcfg/*.kcfg
%{_kde4_datadir}/kde4/services/libokularGenerator*.desktop
%{_kde4_datadir}/kde4/services/okular[A-Z]*.desktop
%{_kde4_datadir}/kde4/services/okular_part.desktop
%{_kde4_datadir}/kde4/servicetypes/okularGenerator.desktop
%{_kde4_libdir}/kde4/okularGenerator*.so
%{_kde4_libdir}/kde4/okularpart.so
%if 0%{?fedora}
%files -n kio_msits
%{_kde4_libdir}/kde4/kio_msits.so
%{_kde4_datadir}/kde4/services/msits.protocol
%endif


%changelog
* Tue May 07 2019 Jan Grulich <jgrulich@redhat.com> - 4.10.5-7
- Fix patch adding information about substituting font
  Resolves: bz#1458037

* Fri Feb 01 2019 Jan Grulich <jgrulich@redhat.com> - 4.10.5-6
- Fix broken dependency on kde-runtime
  Resolves: bz#1670723

* Thu Jan 17 2019 Jan Grulich <jgrulich@redhat.com> - 4.10.5-5
- Add information about substituting font
  Resolves: bz#1458037

* Fri Jan 24 2014 Daniel Mach <dmach@redhat.com> - 4.10.5-4
- Mass rebuild 2014-01-24

* Fri Dec 27 2013 Daniel Mach <dmach@redhat.com> - 4.10.5-3
- Mass rebuild 2013-12-27

* Tue Sep 10 2013 Than Ngo <than@redhat.com> - 4.10.5-2
- don't build djvulibre in rhel

* Sun Jun 30 2013 Than Ngo <than@redhat.com> - 4.10.5-1
- 4.10.5

* Sat Jun 01 2013 Rex Dieter <rdieter@fedoraproject.org> - 4.10.4-1
- 4.10.4

* Mon May 06 2013 Than Ngo <than@redhat.com> - 4.10.3-1
- 4.10.3

* Tue Apr 09 2013 Than Ngo <than@redhat.com> - 4.10.2-3
- don't build component if ACTIVEAPP_FOUND FALSE

* Fri Apr 05 2013 Than Ngo <than@redhat.com> - 4.10.2-2
- BR on plasma-mobile-devel only in fedora

* Sun Mar 31 2013 Rex Dieter <rdieter@fedoraproject.org> - 4.10.2-1
- 4.10.2

* Sun Mar 03 2013 Rex Dieter <rdieter@fedoraproject.org> - 4.10.1-1
- 4.10.1

* Thu Jan 31 2013 Than Ngo <than@redhat.com> - 4.10.0-1
- 4.10.0
- get rid of gcc overflow warning

* Tue Jan 22 2013 Rex Dieter <rdieter@fedoraproject.org> 4.9.98-2
- filename encoding fix (#747976, kde#313700)

* Sun Jan 20 2013 Rex Dieter <rdieter@fedoraproject.org> - 4.9.98-1
- 4.9.98

* Fri Jan 04 2013 Rex Dieter <rdieter@fedoraproject.org> - 4.9.97-1
- 4.9.97

* Thu Dec 20 2012 Rex Dieter <rdieter@fedoraproject.org> - 4.9.95-1
- 4.9.95

* Tue Dec 04 2012 Rex Dieter <rdieter@fedoraproject.org> - 4.9.90-1
- 4.9.90

* Mon Dec 03 2012 Than Ngo <than@redhat.com> - 4.9.4-1
- 4.9.4

* Sat Nov 03 2012 Rex Dieter <rdieter@fedoraproject.org> - 4.9.3-1
- 4.9.3

* Wed Oct 24 2012 Rex Dieter <rdieter@fedoraproject.org> 4.9.2-2
- rebuild (libjpeg-turbo v8)

* Fri Sep 28 2012 Rex Dieter <rdieter@fedoraproject.org> - 4.9.2-1
- 4.9.2

* Mon Sep 03 2012 Than Ngo <than@redhat.com> -  7 :4.9.1-1
- 4.9.1

* Sat Aug 18 2012 Rex Dieter <rdieter@fedoraproject.org> 4.9.0-3
- followup fix for KXMLGUIClient (hang on close)

* Sun Aug 12 2012 Rex Dieter <rdieter@fedoraproject.org> 4.9.0-2
- KXMLGUIClient memory corruption warning (kde#261538)

* Thu Jul 26 2012 Lukas Tinkl <ltinkl@redhat.com> - 4.9.0-1
- 4.9.0

* Fri Jul 20 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 4.8.97-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_18_Mass_Rebuild

* Wed Jul 11 2012 Rex Dieter <rdieter@fedoraproject.org> - 4.8.97-1
- 4.8.97

* Wed Jun 27 2012 Rex Dieter <rdieter@fedoraproject.org> - 4.8.95-1
- 4.8.95

* Sat Jun 09 2012 Rex Dieter <rdieter@fedoraproject.org> - 4.8.90-1
- 4.8.90

* Sat May 26 2012 Jaroslav Reznik <jreznik@redhat.com> - 4.8.80-1
- 4.8.80

* Wed May 16 2012 Marek Kasik <mkasik@redhat.com> - 4.8.3-4
- Rebuild (poppler-0.20.0)

* Wed May 09 2012 Than Ngo <than@redhat.com> - 4.8.3-3
- add fedora/rhel condition

* Tue May 08 2012 Rex Dieter <rdieter@fedoraproject.org> 4.8.3-2
- rebuild (libtiff)

* Mon Apr 30 2012 Jaroslav Reznik <jreznik@redhat.com> - 4.8.3-1
- 4.8.3

* Fri Mar 30 2012 Rex Dieter <rdieter@fedoraproject.org> - 4.8.2-1
- 4.8.2

* Wed Mar 07 2012 Rex Dieter <rdieter@fedoraproject.org> 4.8.1-2
- s/kdebase-runtime/kde-runtime/

* Mon Mar 05 2012 Jaroslav Reznik <jreznik@redhat.com> - 4.8.1-1
- 4.8.1

* Sun Jan 22 2012 Rex Dieter <rdieter@fedoraproject.org> - 4.8.0-1
- 4.8.0

* Wed Jan 04 2012 Radek Novacek <rnovacek@redhat.com> - 4.7.97-1
- 4.7.97

* Wed Dec 21 2011 Radek Novacek <rnovacek@redhat.com> - 4.7.95-1
- 4.7.95

* Sun Dec 04 2011 Rex Dieter <rdieter@fedoraproject.org> - 4.7.90-1
- 4.7.90

* Fri Nov 25 2011 Jaroslav Reznik <jreznik@redhat.com> 4.7.80-1
- 4.7.80 (beta 1)

* Tue Nov 15 2011 Rex Dieter <rdieter@fedoraproject.org> 4.7.3-2
- okular-part subpkg
- BR: libjpeg-devel

* Sat Oct 29 2011 Rex Dieter <rdieter@fedoraproject.org> 4.7.3-1
- 4.7.3
- more pkgconfig-style deps

* Wed Oct 05 2011 Rex Dieter <rdieter@fedoraproject.org> 4.7.2-1
- 4.7.2

* Tue Sep 06 2011 Than Ngo <than@redhat.com> - 4.7.1-1
- 4.7.1

* Tue Jul 26 2011 Jaroslav Reznik <jreznik@redhat.com> 4.7.0-1
- 4.7.0

* Mon Jul 18 2011 Rex Dieter <rdieter@fedoraproject.org> 4.6.95-4
- %%postun: +update-desktop-database

* Mon Jul 18 2011 Rex Dieter <rdieter@fedoraproject.org> 4.6.95-3
- BR: pkgconfig(qca2)

* Fri Jul 15 2011 Rex Dieter <rdieter@fedoraproject.org> 4.6.95-2
- bump release

* Mon Jul 11 2011 Rex Dieter <rdieter@fedoraproject.org> 4.6.95-1
- 4.6.95
- fix URL

* Wed Jul 06 2011 Rex Dieter <rdieter@fedoraproject.org> 4.6.90-3
- fix Source URL
- Conflicts: kdegraphics < 7:4.6.90-10

* Tue Jul 05 2011 Rex Dieter <rdieter@fedoraproject.org>  4.6.90-2
- first try

