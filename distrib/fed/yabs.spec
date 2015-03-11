Name:           yabs
Version:        master
Release:        1%{?dist}
Summary:        Yet another build system

License:        BSD
URL:            https://github.com/0X1A/yabs
Source0:        https://github.com/0X1A/%{name}/archive/%{version}.tar.gz

BuildRequires:  git libyaml-devel libarchive-devel libssh2-devel
Requires:       libyaml libarchive libssh2

%description


%prep
%setup -q


%build
make PREFIX=/usr CXXFLAGS="-pipe -std=c++11 -march=x86-64 -mtune=generic -O2 -pipe --param=ssp-buffer-size=4 -Wall" LIBS="-lssh2 -lyaml -larchive" LFLAGS="-Wl,-O1,--sort-common,--as-needed,-z,relro -Wl,-O1" %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make PREFIX=/usr DEST=%{buildroot} install

%files
%{_bindir}/*
%{_mandir}/man1/*
%doc



%changelog
* Wed Mar 11 2015 Alberto Corona <alberto@0x1a.us>
- Initial write
