# configure.rake - handles all configuration and generate needed build files

require 'rbconfig'
require 'rakelib/configuration'

file 'lib/rbconfig.rb' => 'rakelib/configure.rake' do
  write_rbconfig
end

def write_rbconfig
  File.open 'lib/rbconfig.rb', 'w' do |f|
    f.puts '#--'
    f.puts '# This file was generated from rakelib/configure.rake'
    f.puts '#++'
    f.puts
    f.puts 'module Config'
    f.puts '  unless defined? RUBY_ENGINE and RUBY_ENGINE == "rbx" then'
    f.puts '    raise "Looks like you loaded the Rubinius rbconfig, but this is not Rubinius."'
    f.puts '  end'
    f.puts
    f.puts '  prefix = File.dirname(File.dirname(__FILE__))'
    f.puts
    f.puts '  CONFIG = {}'
    f.puts
    f.puts '  CONFIG["prefix"]             = prefix'
    f.puts "  CONFIG[\"install_prefix\"]     = '#{RBX_PREFIX}'"
    f.puts '  CONFIG["DLEXT"]              = Rubinius::LIBSUFFIX[1..-1]'
    f.puts '  CONFIG["EXEEXT"]             = ""'
    f.puts '  CONFIG["ruby_install_name"]  = RUBY_ENGINE.dup'
    f.puts '  CONFIG["RUBY_INSTALL_NAME"]  = RUBY_ENGINE.dup'

    f.puts '  CONFIG["exec_prefix"]        = "$(prefix)"'
    f.puts '  if File.exists?(File.join(prefix, "bin", "rbx"))'
    f.puts '    CONFIG["bindir"]             = "$(exec_prefix)/bin"'
    f.puts '  else'
    f.puts "    CONFIG[\"bindir\"]           = '#{RBX_BINPATH}'"
    f.puts '  end'
    f.puts '  CONFIG["sbindir"]            = "$(exec_prefix)/sbin"'
    f.puts '  CONFIG["libexecdir"]         = "$(exec_prefix)/libexec"'
    f.puts '  CONFIG["datarootdir"]        = "$(prefix)/share"'
    f.puts '  CONFIG["datadir"]            = "$(datarootdir)"'
    f.puts '  CONFIG["sysconfdir"]         = "$(prefix)/etc"'
    f.puts '  CONFIG["sharedstatedir"]     = "$(prefix)/com"'
    f.puts '  CONFIG["localstatedir"]      = "$(prefix)/var"'
    f.puts '  CONFIG["includedir"]         = "$(prefix)/include"'
    f.puts '  CONFIG["oldincludedir"]      = "/usr/include"'
    f.puts '  CONFIG["docdir"]             = "$(datarootdir)/doc/$(PACKAGE)"'
    f.puts '  CONFIG["infodir"]            = "$(datarootdir)/info"'
    f.puts '  CONFIG["htmldir"]            = "$(docdir)"'
    f.puts '  CONFIG["dvidir"]             = "$(docdir)"'
    f.puts '  CONFIG["pdfdir"]             = "$(docdir)"'
    f.puts '  CONFIG["psdir"]              = "$(docdir)"'
    f.puts '  CONFIG["libdir"]             = "$(exec_prefix)/lib"'
    f.puts '  CONFIG["localedir"]          = "$(datarootdir)/locale"'
    f.puts '  CONFIG["mandir"]             = "$(datarootdir)/man"'

    f.puts '  major, minor, teeny = RUBY_VERSION.split(".")'
    f.puts '  CONFIG["MAJOR"]              = "#{major}"'
    f.puts '  CONFIG["MINOR"]              = "#{minor}"'
    f.puts '  CONFIG["TEENY"]              = "#{teeny}"'
    f.puts '  CONFIG["ruby_version"]       = "$(MAJOR).$(MINOR)"'
    f.puts '  CONFIG["RUBY_SO_NAME"]       = "rubinius-#{Rubinius::RBX_VERSION}"'

    f.puts '  CONFIG["rubyhdrdir"]         = "#{Rubinius::HDR_PATH}"'
    f.puts '  CONFIG["sitedir"]            = "#{Rubinius::SITEDIR}"'
    f.puts '  CONFIG["sitelibdir"]         = "#{Rubinius::SITELIBDIR}"'
    f.puts '  CONFIG["arch"]               = "#{Rubinius::ARCH}"'
    f.puts '  CONFIG["sitearch"]           = "#{Rubinius::SITEARCH}"'
    f.puts '  CONFIG["rubylibdir"]         = "#{Rubinius::RUBYLIBDIR}"'
    f.puts '  CONFIG["archdir"]            = "#{Rubinius::ARCHDIR}"'
    f.puts '  CONFIG["sitearchdir"]        = "#{Rubinius::SITEARCHDIR}"'
    f.puts '  CONFIG["topdir"]             = File.dirname(__FILE__)'

    f.puts '  # some of these only relevant to cross-compiling'
    f.puts '  cpu                          = Rubinius::CONFIG_CPU'
    f.puts '  vendor                       = Rubinius::CONFIG_VENDOR'
    f.puts '  os                           = Rubinius::CONFIG_OS'
    f.puts '  CONFIG["build"]              = "#{cpu}-#{vendor}-#{os}"'
    f.puts '  CONFIG["build_cpu"]          = "#{cpu}"'
    f.puts '  CONFIG["build_vendor"]       = "#{vendor}"'
    f.puts '  CONFIG["build_os"]           = "#{os}"'
    f.puts '  CONFIG["host"]               = "#{cpu}-#{vendor}-#{os}"'
    f.puts '  CONFIG["host_cpu"]           = "#{cpu}"'
    f.puts '  CONFIG["host_vendor"]        = "#{vendor}"'
    f.puts '  CONFIG["host_os"]            = "#{os}"'
    f.puts '  CONFIG["target"]             = "#{cpu}-#{vendor}-#{os}"'
    f.puts '  CONFIG["target_cpu"]         = "#{cpu}"'
    f.puts '  CONFIG["target_vendor"]      = "#{vendor}"'
    f.puts '  CONFIG["target_os"]          = "#{os}"'
    f.puts '  CONFIG["build_alias"]        = ""'
    f.puts '  CONFIG["host_alias"]         = ""'
    f.puts '  CONFIG["target_alias"]       = ""'

    # TODO: we need to be able to discover these, but for now, UNIXy defaults
    f.puts '  # command line utilities'
    f.puts '  CONFIG["SHELL"]              = "/bin/sh"'
    f.puts '  CONFIG["ECHO_C"]             = ""'
    f.puts '  CONFIG["ECHO_N"]             = "-n"'
    f.puts '  CONFIG["ECHO_T"]             = ""'
    f.puts '  CONFIG["GREP"]               = "/usr/bin/grep"'
    f.puts '  CONFIG["EGREP"]              = "/usr/bin/grep -E"'
    f.puts '  CONFIG["RM"]                 = "rm -f"'
    f.puts '  CONFIG["CP"]                 = "cp"'
    f.puts '  CONFIG["NROFF"]              = "/usr/bin/nroff"'
    f.puts '  CONFIG["MAKEDIRS"]           = "mkdir -p"'

    f.puts '  # compile tools'
    f.puts '  CONFIG["CC"]                 = "gcc"'
    f.puts '  CONFIG["CPP"]                = "gcc -E"'
    f.puts '  CONFIG["YACC"]               = "bison -y"'
    f.puts '  CONFIG["RANLIB"]             = "ranlib"'
    f.puts '  CONFIG["AR"]                 = "ar"'
    f.puts '  CONFIG["AS"]                 = "as"'
    f.puts '  CONFIG["WINDRES"]            = ""'
    f.puts '  CONFIG["DLLWRAP"]            = ""'
    f.puts '  CONFIG["OBJDUMP"]            = ""'
    f.puts '  CONFIG["LN_S"]               = "ln -s"'
    f.puts '  CONFIG["NM"]                 = ""'
    f.puts '  CONFIG["INSTALL"]            = "/usr/bin/install -c"'
    f.puts '  CONFIG["INSTALL_PROGRAM"]    = "$(INSTALL)"'
    f.puts '  CONFIG["INSTALL_SCRIPT"]     = "$(INSTALL)"'
    f.puts '  CONFIG["INSTALL_DATA"]       = "$(INSTALL) -m 644"'
    f.puts '  CONFIG["STRIP"]              = "strip -A -n"'
    f.puts '  CONFIG["MANTYPE"]            = "doc"'
    f.puts '  CONFIG["MAKEFILES"]          = "Makefile"'

    # TODO: fill in these values
    f.puts '  # compile tools flags'
    f.puts '  CONFIG["CFLAGS"]             = "-Wall -ggdb3 -O2"'
    f.puts '  CONFIG["LDFLAGS"]            = ""'
    f.puts '  CONFIG["CPPFLAGS"]           = ""'
    f.puts '  CONFIG["OBJEXT"]             = "o"'
    f.puts '  CONFIG["GNU_LD"]             = ""'
    f.puts '  CONFIG["CPPOUTFILE"]         = ""'
    f.puts '  CONFIG["OUTFLAG"]            = "-o "'
    f.puts '  CONFIG["YFLAGS"]             = ""'
    f.puts '  CONFIG["ASFLAGS"]            = ""'
    f.puts '  CONFIG["DLDFLAGS"]           = ""'
    f.puts '  CONFIG["ARCH_FLAG"]          = ""'
    f.puts '  CONFIG["STATIC"]             = ""'
    f.puts '  CONFIG["CCDLFLAGS"]          = ""'
    f.puts '  CONFIG["XCFLAGS"]            = ""'
    f.puts '  CONFIG["XLDFLAGS"]           = ""'
    f.puts '  CONFIG["LIBRUBY_DLDFLAGS"]   = ""'
    f.puts '  CONFIG["rubyw_install_name"] = ""'
    f.puts '  CONFIG["RUBYW_INSTALL_NAME"] = ""'
    f.puts '  CONFIG["SOLIBS"]             = ""'
    f.puts '  CONFIG["DLDLIBS"]            = ""'
    f.puts '  CONFIG["ENABLE_SHARED"]      = ""'
    f.puts '  CONFIG["MAINLIBS"]           = ""'
    f.puts '  CONFIG["COMMON_LIBS"]        = ""'
    f.puts '  CONFIG["COMMON_MACROS"]      = ""'
    f.puts '  CONFIG["COMMON_HEADERS"]     = ""'
    f.puts '  CONFIG["EXPORT_PREFIX"]      = ""'
    f.puts '  CONFIG["EXTOUT"]             = ".ext"'
    f.puts '  CONFIG["ARCHFILE"]           = ""'
    f.puts '  CONFIG["RDOCTARGET"]         = ""'
    f.puts '  CONFIG["LIBRUBY_A"]          = ""'
    f.puts '  CONFIG["LIBRUBY_SO"]         = "lib$(RUBY_SO_NAME).$(DLEXT)"'
    f.puts '  CONFIG["LIBRUBY_ALIASES"]    = "lib$(RUBY_SO_NAME).$(DLEXT)"'
    f.puts '  CONFIG["LIBRUBY"]            = "$(LIBRUBY_SO)"'
    f.puts '  CONFIG["LIBRUBYARG"]         = "$(LIBRUBYARG_STATIC)"'
    f.puts '  CONFIG["LIBRUBYARG_STATIC"]  = ""'
    f.puts '  CONFIG["LIBRUBYARG_SHARED"]  = "-l$(RUBY_SO_NAME)"'
    f.puts '  CONFIG["configure_args"]     = ""'
    f.puts '  CONFIG["ALLOCA"]             = ""'
    f.puts '  CONFIG["LIBEXT"]             = "a"'
    f.puts '  CONFIG["LINK_SO"]            = ""'
    f.puts '  CONFIG["LIBPATHFLAG"]        = " -L%s"'
    f.puts '  CONFIG["RPATHFLAG"]          = ""'
    f.puts '  CONFIG["LIBPATHENV"]         = "DYLD_LIBRARY_PATH"'
    f.puts '  CONFIG["TRY_LINK"]           = ""'
    f.puts '  CONFIG["EXTSTATIC"]          = ""'
    f.puts '  CONFIG["setup"]              = "Setup"'
    f.puts '  CONFIG["PATH_SEPARATOR"]     = ":"'
    f.puts '  CONFIG["PACKAGE_NAME"]       = ""'
    f.puts '  CONFIG["PACKAGE_TARNAME"]    = ""'
    f.puts '  CONFIG["PACKAGE_VERSION"]    = ""'
    f.puts '  CONFIG["PACKAGE_STRING"]     = ""'
    f.puts '  CONFIG["PACKAGE_BUGREPORT"]  = ""'

    # @todo create a config step that sets the values used to compile
    # Rubinius; read those values for these types of config variables
    if Config::CONFIG["build_os"] =~ /darwin/
      f.puts '  CONFIG["LDSHARED"]          = "cc -dynamic -bundle -undefined suppress -flat_namespace"'
      f.puts '  CONFIG["LIBRUBY_LDSHARED"]  = "cc -dynamic -bundle -undefined suppress -flat_namespace"'
    else
      f.puts '  CONFIG["LDSHARED"]          = "cc -shared"'
      f.puts '  CONFIG["LIBRUBY_LDSHARED"]  = "cc -shared"'
    end
    f.puts
    f.puts <<-EOC
  # Adapted from MRI's' rbconfig.rb
  MAKEFILE_CONFIG = {}
  CONFIG.each { |k,v| MAKEFILE_CONFIG[k] = v.kind_of?(String) ? v.dup : v }

  def Config.expand(val, config = CONFIG)
    return val unless val.kind_of? String

    val.gsub!(/\\$\\$|\\$\\(([^()]+)\\)|\\$\\{([^{}]+)\\}/) do |var|
      if !(v = $1 || $2)
        '$'
      elsif key = config[v = v[/\\A[^:]+(?=(?::(.*?)=(.*))?\\z)/]]
        pat, sub = $1, $2
        config[v] = false
        Config.expand(key, config)
        config[v] = key
        key = key.gsub(/\#{Regexp.quote(pat)}(?=\\s|\\z)/n) {sub} if pat
        key
      else
        var
      end
    end
    val
  end

  CONFIG.each_value do |val|
    Config.expand(val)
  end
EOC
    f.puts "end"
    f.puts
    f.puts "CROSS_COMPILING = nil unless defined? CROSS_COMPILING"
    f.puts "RbConfig = Config"
  end
end

file 'kernel/bootstrap/rubinius_config.rb' =>
     %w[rakelib/configuration.rb rakelib/configure.rake] do |task, args|
  open task.name, 'w' do |io|
    io << <<-EOF
#--
# This file was generated from rakelib/configure.rake
#++

module Rubinius
  BUILDREV        = #{RBX_BUILDREV.inspect}
  BASE_PATH       = #{RBX_BASE_PATH.inspect}
  CODE_PATH       = #{RBX_CODE_PATH.inspect}
  EXT_PATH        = #{RBX_EXT_PATH.inspect}
  RBA_PATH        = #{RBX_RBA_PATH.inspect}
  BIN_PATH        = #{RBX_BIN_PATH.inspect}
  LIB_PATH        = #{RBX_LIB_PATH.inspect}
  HDR_PATH        = #{RBX_HDR_PATH.inspect}
  RBX_VERSION     = #{RBX_VERSION.inspect}
  CONFIG_CPU      = #{RBX_CPU.inspect}
  CONFIG_VENDOR   = #{RBX_VENDOR.inspect}
  CONFIG_OS       = #{RBX_OS.inspect}
  SITEDIR         = #{RBX_SITEDIR.inspect}
  SITELIBDIR      = #{RBX_SITELIBDIR.inspect}
  ARCH            = #{RBX_ARCH.inspect}
  SITEARCH        = #{RBX_SITEARCH.inspect}
  RUBYLIBDIR      = #{RBX_RUBYLIBDIR.inspect}
  ARCHDIR         = #{RBX_ARCHDIR.inspect}
  SITEARCHDIR     = #{RBX_SITEARCHDIR.inspect}
end

    EOF
  end
end

file 'kernel/bootstrap/ruby_config.rb' =>
     %w[rakelib/configuration.rb rakelib/configure.rake] do |task, args|
  open task.name, 'w' do |io|
    io << <<-EOF
#--
# This file was generated from rakelib/configure.rake
#++

PLATFORM          = #{RBX_HOST.inspect}

RUBY_ENGINE       = #{RBX_RUBY_ENGINE.inspect}
RUBY_PATCHLEVEL   = #{RBX_RUBY_PATCHLEVEL.inspect}
RUBY_PLATFORM     = #{RBX_HOST.inspect}
RUBY_RELEASE_DATE = #{RBX_RUBY_RELDATE.inspect}
RUBY_VERSION      = #{RBX_RUBY_VERSION.inspect}

VERSION           = RUBY_VERSION
    EOF
  end
end

namespace :configure do
  task :clean do
    rm_f 'lib/rbconfig.rb'
    rm_f 'kernel/bootstrap/rubinius_config.rb'
    rm_f 'kernel/bootstrap/ruby_config.rb'
  end
end

