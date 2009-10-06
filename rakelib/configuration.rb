# configuration for Rubinius

config_path = File.join(File.dirname(__FILE__), "..", "config.rb")
unless File.file?(config_path)
  STDERR.puts "Please run ./configure first"
  exit 1
end

load config_path

RBX_DTRACE          = nil
RBX_RUBY_ENGINE     = 'rbx'
RBX_RUBY_VERSION    = '1.8.7'
RBX_RUBY_PATCHLEVEL = 174
RBX_RUBY_RELDATE    = '2009-10-05'
RBX_LIBVER          = '0.12'
RBX_VERSION         = "#{RBX_LIBVER}.0"
RBX_HOST            = `./rakelib/config.guess`.chomp
RBX_BUILDREV        = `git rev-list --all | head -n1`.chomp
RBX_CC              = Rubinius::BUILD_CONFIG[:compiler]

# There are two ways to build Rubinius: development and install.
# We assume that if prefix is set, we are building in
# install mode, otherwise development mode. For more details,
# see doc/build_system.txt.
if prefix = Rubinius::BUILD_CONFIG[:prefix]
  RBX_PREFIX          = prefix
  RBX_BINPATH         = "#{RBX_PREFIX}/bin"
  RBX_LIBPATH         = "#{RBX_PREFIX}/lib"
  RBX_BASE_PATH       = "#{RBX_PREFIX}/lib/rubinius/#{RBX_LIBVER}"
  RBX_HDR_PATH        = "#{RBX_BASE_PATH}/#{RBX_HOST}"
else
  RBX_PREFIX          = Dir.pwd
  RBX_BASE_PATH       = RBX_PREFIX
  RBX_BINPATH         = "#{RBX_BASE_PATH}/bin"
  RBX_LIBPATH         = "#{RBX_BASE_PATH}/vm"
  RBX_HDR_PATH        = "#{RBX_BASE_PATH}/vm/capi"
end

# RubyGems is already using Rubinius::CODE_PATH so we will
# continue to support it, although BASE_PATH is probably
# a better description.
RBX_CODE_PATH       = RBX_BASE_PATH
RBX_RBA_PATH        = "#{RBX_BASE_PATH}/runtime"
RBX_BIN_PATH        = "#{RBX_BASE_PATH}/bin"
RBX_LIB_PATH        = "#{RBX_BASE_PATH}/lib"
RBX_EXT_PATH        = "#{RBX_BASE_PATH}/lib/ext"

/([^-]+)-([^-]+)-(.*)/ =~ RBX_HOST
RBX_CPU, RBX_VENDOR, RBX_OS = $1, $2, $3

RBX_SITEDIR         = "#{RBX_PREFIX}/lib/rubinius"
RBX_SITELIBDIR      = "#{RBX_SITEDIR}/#{RBX_LIBVER}"
RBX_RUBYLIBDIR      = RBX_SITELIBDIR
RBX_ARCH            = "#{RBX_CPU}-#{RBX_OS}"
RBX_SITEARCH        = RBX_ARCH
RBX_ARCHDIR         = "#{RBX_RUBYLIBDIR}/#{RBX_ARCH}"
RBX_SITEARCHDIR     = "#{RBX_SITELIBDIR}/#{RBX_SITEARCH}"

case RBX_HOST
when /darwin9/
  RBX_DARWIN         = 1
  RBX_DISABLE_KQUEUE = 1
when /darwin/
  RBX_DARWIN         = 1
  RBX_DISABLE_KQUEUE = 1
when /freebsd/
  RBX_DARWIN         = 0
  RBX_DISABLE_KQUEUE = 1
else
  RBX_DARWIN         = 0
  RBX_DISABLE_KQUEUE = 0
end

