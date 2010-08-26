require 'rubinius/build_config'

module RbConfig
  unless defined? RUBY_ENGINE and RUBY_ENGINE == "rbx" then
    raise "Looks like you loaded the Rubinius rbconfig, but this is not Rubinius."
  end

  prefix = File.dirname(File.dirname(__FILE__))

  CONFIG = {}

  CONFIG["prefix"]             = prefix
  CONFIG["install_prefix"]     = ''
  CONFIG["DLEXT"]              = Rubinius::LIBSUFFIX[1..-1]
  CONFIG["EXEEXT"]             = ""
  CONFIG["ruby_install_name"]  = RUBY_ENGINE.dup
  CONFIG["RUBY_INSTALL_NAME"]  = RUBY_ENGINE.dup
  CONFIG["exec_prefix"]        = "$(prefix)"
  CONFIG["bindir"]             = Rubinius::BUILD_CONFIG[:bindir]
  CONFIG["sbindir"]            = "$(exec_prefix)/sbin"
  CONFIG["libexecdir"]         = "$(exec_prefix)/libexec"
  CONFIG["datarootdir"]        = "$(prefix)/share"
  CONFIG["datadir"]            = "$(datarootdir)"
  CONFIG["sysconfdir"]         = "$(prefix)/etc"
  CONFIG["sharedstatedir"]     = "$(prefix)/com"
  CONFIG["localstatedir"]      = "$(prefix)/var"
  CONFIG["includedir"]         = "$(prefix)/include"
  CONFIG["oldincludedir"]      = "/usr/include"
  CONFIG["docdir"]             = "$(datarootdir)/doc/$(PACKAGE)"
  CONFIG["infodir"]            = "$(datarootdir)/info"
  CONFIG["htmldir"]            = "$(docdir)"
  CONFIG["dvidir"]             = "$(docdir)"
  CONFIG["pdfdir"]             = "$(docdir)"
  CONFIG["psdir"]              = "$(docdir)"
  CONFIG["libdir"]             = "$(exec_prefix)/lib"
  CONFIG["localedir"]          = "$(datarootdir)/locale"
  CONFIG["mandir"]             = "$(datarootdir)/man"
  major, minor, teeny = RUBY_VERSION.split(".")
  CONFIG["MAJOR"]              = major
  CONFIG["MINOR"]              = minor
  CONFIG["TEENY"]              = teeny
  CONFIG["ruby_version"]       = "$(MAJOR).$(MINOR)"
  CONFIG["RUBY_SO_NAME"]       = "rubinius-#{Rubinius::VERSION}"
  CONFIG["rubyhdrdir"]         = "#{Rubinius::HDR_PATH}"
  CONFIG["LIBS"]               = ""

  sitedir                      = Rubinius::SITE_PATH
  sitelibdir                   = sitedir
  arch                         = "#{Rubinius::CPU}-#{Rubinius::OS}"

  CONFIG["sitedir"]            = sitedir
  CONFIG["sitelibdir"]         = sitelibdir
  CONFIG["arch"]               = arch
  CONFIG["sitearch"]           = arch
  CONFIG["rubylibdir"]         = sitelibdir
  CONFIG["archdir"]            = "#{sitelibdir}/#{arch}"
  CONFIG["sitearchdir"]        = "#{sitelibdir}/#{arch}"
  CONFIG["topdir"]             = File.dirname(__FILE__)
  # some of these only relevant to cross-compiling
  cpu                          = Rubinius::CPU
  vendor                       = Rubinius::VENDOR
  os                           = Rubinius::OS
  CONFIG["build"]              = "#{cpu}-#{vendor}-#{os}"
  CONFIG["build_cpu"]          = "#{cpu}"
  CONFIG["build_vendor"]       = "#{vendor}"
  CONFIG["build_os"]           = "#{os}"
  CONFIG["host"]               = "#{cpu}-#{vendor}-#{os}"
  CONFIG["host_cpu"]           = "#{cpu}"
  CONFIG["host_vendor"]        = "#{vendor}"
  CONFIG["host_os"]            = "#{os}"
  CONFIG["target"]             = "#{cpu}-#{vendor}-#{os}"
  CONFIG["target_cpu"]         = "#{cpu}"
  CONFIG["target_vendor"]      = "#{vendor}"
  CONFIG["target_os"]          = "#{os}"
  CONFIG["build_alias"]        = ""
  CONFIG["host_alias"]         = ""
  CONFIG["target_alias"]       = ""
  # command line utilities
  CONFIG["SHELL"]              = "/bin/sh"
  CONFIG["ECHO_C"]             = ""
  CONFIG["ECHO_N"]             = "-n"
  CONFIG["ECHO_T"]             = ""
  CONFIG["GREP"]               = "/usr/bin/grep"
  CONFIG["EGREP"]              = "/usr/bin/grep -E"
  CONFIG["RM"]                 = "rm -f"
  CONFIG["CP"]                 = "cp"
  CONFIG["NROFF"]              = "/usr/bin/nroff"
  CONFIG["MAKEDIRS"]           = "mkdir -p"
  # compile tools
  CONFIG["CC"]                 = "gcc"
  CONFIG["CPP"]                = "gcc -E"
  CONFIG["YACC"]               = "bison -y"
  CONFIG["RANLIB"]             = "ranlib"
  CONFIG["AR"]                 = "ar"
  CONFIG["AS"]                 = "as"
  CONFIG["WINDRES"]            = ""
  CONFIG["DLLWRAP"]            = ""
  CONFIG["OBJDUMP"]            = ""
  CONFIG["LN_S"]               = "ln -s"
  CONFIG["NM"]                 = ""
  CONFIG["INSTALL"]            = "install -c"
  CONFIG["INSTALL_PROGRAM"]    = "$(INSTALL)"
  CONFIG["INSTALL_SCRIPT"]     = "$(INSTALL)"
  CONFIG["INSTALL_DATA"]       = "$(INSTALL) -m 644"
  CONFIG["STRIP"]              = "strip -A -n"
  CONFIG["MANTYPE"]            = "doc"
  CONFIG["MAKEFILES"]          = "Makefile"
  # compile tools flags
  # since we hardcode using gcc, and this flag is only
  # used by mkmf to compile extensions, be sure PIC is in
  # there
  CONFIG["CFLAGS"]             = "-ggdb3 -O2 -fPIC"
  CONFIG["LDFLAGS"]            = ""
  CONFIG["CPPFLAGS"]           = ""
  CONFIG["OBJEXT"]             = "o"
  CONFIG["GNU_LD"]             = ""
  CONFIG["CPPOUTFILE"]         = ""
  CONFIG["OUTFLAG"]            = "-o "
  CONFIG["YFLAGS"]             = ""
  CONFIG["ASFLAGS"]            = ""
  CONFIG["DLDFLAGS"]           = ""
  CONFIG["ARCH_FLAG"]          = ""
  CONFIG["STATIC"]             = ""
  CONFIG["CCDLFLAGS"]          = ""
  CONFIG["XCFLAGS"]            = ""
  CONFIG["XLDFLAGS"]           = ""
  CONFIG["LIBRUBY_DLDFLAGS"]   = ""
  CONFIG["rubyw_install_name"] = ""
  CONFIG["RUBYW_INSTALL_NAME"] = ""
  CONFIG["SOLIBS"]             = ""
  CONFIG["DLDLIBS"]            = ""
  CONFIG["ENABLE_SHARED"]      = ""
  CONFIG["MAINLIBS"]           = ""
  CONFIG["COMMON_LIBS"]        = ""
  CONFIG["COMMON_MACROS"]      = ""
  CONFIG["COMMON_HEADERS"]     = ""
  CONFIG["EXPORT_PREFIX"]      = ""
  CONFIG["EXTOUT"]             = ".ext"
  CONFIG["ARCHFILE"]           = ""
  CONFIG["RDOCTARGET"]         = ""
  CONFIG["LIBRUBY_A"]          = ""
  CONFIG["LIBRUBY_SO"]         = "lib$(RUBY_SO_NAME).$(DLEXT)"
  CONFIG["LIBRUBY_ALIASES"]    = "lib$(RUBY_SO_NAME).$(DLEXT)"
  CONFIG["LIBRUBY"]            = "$(LIBRUBY_SO)"
  CONFIG["LIBRUBYARG"]         = "$(LIBRUBYARG_STATIC)"
  CONFIG["LIBRUBYARG_STATIC"]  = ""
  CONFIG["LIBRUBYARG_SHARED"]  = ""
  CONFIG["configure_args"]     = ""
  CONFIG["ALLOCA"]             = ""
  CONFIG["LIBEXT"]             = "a"
  CONFIG["LINK_SO"]            = ""
  CONFIG["LIBPATHFLAG"]        = " -L%s"
  CONFIG["RPATHFLAG"]          = ""
  CONFIG["LIBPATHENV"]         = "DYLD_LIBRARY_PATH"
  CONFIG["TRY_LINK"]           = ""
  CONFIG["EXTSTATIC"]          = ""
  CONFIG["setup"]              = "Setup"
  CONFIG["PATH_SEPARATOR"]     = ":"
  CONFIG["PACKAGE_NAME"]       = ""
  CONFIG["PACKAGE_TARNAME"]    = ""
  CONFIG["PACKAGE_VERSION"]    = ""
  CONFIG["PACKAGE_STRING"]     = ""
  CONFIG["PACKAGE_BUGREPORT"]  = ""
  CONFIG["LDSHARED"]           = Rubinius::LDSHARED
  CONFIG["LIBRUBY_LDSHARED"]   = Rubinius::LDSHARED

  # Adapted from MRI's' rbconfig.rb
  MAKEFILE_CONFIG = {}
  CONFIG.each { |k,v| MAKEFILE_CONFIG[k] = v.kind_of?(String) ? v.dup : v }

  def RbConfig.expand(val, config = CONFIG)
    return val unless val.kind_of? String

    val.gsub!(/\$\$|\$\(([^()]+)\)|\$\{([^{}]+)\}/) do |var|
      if !(v = $1 || $2)
        '$'
      elsif key = config[v = v[/\A[^:]+(?=(?::(.*?)=(.*))?\z)/]]
        pat, sub = $1, $2
        config[v] = false
        RbConfig.expand(key, config)
        config[v] = key
        key = key.gsub(/#{Regexp.quote(pat)}(?=\s|\z)/n) {sub} if pat
        key
      else
        var
      end
    end
    val
  end

  CONFIG.each_value do |val|
    RbConfig.expand(val)
  end
end

CROSS_COMPILING = nil unless defined? CROSS_COMPILING
Config = RbConfig
