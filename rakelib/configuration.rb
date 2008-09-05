# configuration for Rubinius

RBX_DTRACE          = ENV['DTRACE']
RBX_RUBY_ENGINE     = 'rbx'
RBX_PREFIX          = ENV['PREFIX'] || '/usr/local'
RBX_RUBY_VERSION    = '1.8.6'
RBX_RUBY_PATCHLEVEL = '111'
RBX_RUBY_RELDATE    = '08/04/2008'
RBX_LIBVER          = '0.9'
RBX_VERSION         = "#{RBX_LIBVER}.0"
RBX_HOST            = `./rakelib/config.guess`.chomp
RBX_BUILDREV        = `git rev-list --all | head -n1`.chomp
RBX_CC              = ENV['CC'] || 'gcc'
RBX_BINPATH         = "#{RBX_PREFIX}/bin"
RBX_LIBPATH         = "#{RBX_PREFIX}/lib"
RBX_CODE_PATH       = "#{RBX_PREFIX}/lib/rubinius/#{RBX_LIBVER}"
RBX_RBA_PATH        = "#{RBX_PREFIX}/lib/rubinius/#{RBX_LIBVER}/runtime"
RBX_EXT_PATH        = "#{RBX_PREFIX}/lib/rubinius/#{RBX_LIBVER}/#{RBX_HOST}"

case RBX_HOST
when /darwin9/ then
  RBX_DARWIN         = 1
  RBX_DISABLE_KQUEUE = 1
when /darwin/ then
  RBX_DARWIN         = 1
  RBX_DISABLE_KQUEUE = 1
when /freebsd/ then
  RBX_DARWIN         = 0
  RBX_DISABLE_KQUEUE = 1
else
  RBX_DARWIN         = 0
  RBX_DISABLE_KQUEUE = 0
end

