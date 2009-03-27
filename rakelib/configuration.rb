# configuration for Rubinius

RBX_DTRACE          = ENV['DTRACE']
RBX_RUBY_ENGINE     = 'rbx'
RBX_RUBY_VERSION    = '1.8.6'
RBX_RUBY_PATCHLEVEL = 322
RBX_RUBY_RELDATE    = '12/31/2009'
RBX_LIBVER          = '0.11'
RBX_VERSION         = "#{RBX_LIBVER}.0-dev"
RBX_HOST            = `./rakelib/config.guess`.chomp
RBX_BUILDREV        = `git rev-list --all | head -n1`.chomp
RBX_CC              = ENV['CC'] || 'gcc'

# There are two ways to build Rubinius: development and install.
# We assume that if ENV['RBX_PREFIX'] is set, we are building in
# install mode, otherwise development mode. For more details,
# see doc/build_system.txt.
install = ! Rake.application.top_level_tasks.grep(/install/).empty?
if install || ENV['RBX_PREFIX']
  RBX_PREFIX          = ENV['RBX_PREFIX'] || "/usr/local"
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

