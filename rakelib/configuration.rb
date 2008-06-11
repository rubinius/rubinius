# configuration for Rubinius

DTRACE              = ENV['DTRACE']
ENGINE              = "rbx"
PREFIX              = ENV['PREFIX'] || "/usr/local"
RBX_RUBY_VERSION    = "1.8.6"
RBX_RUBY_PATCHLEVEL = "111"
LIBVER              = "0.8"
RBX_VERSION         = "#{LIBVER}.0"
HOST                = `./shotgun/config.guess`.chomp
BUILDREV            = `git rev-list --all | head -n1`.chomp
CC                  = ENV['CC'] || 'gcc'
BINPATH             = "#{PREFIX}/bin"
LIBPATH             = "#{PREFIX}/lib"
CODEPATH            = "#{PREFIX}/lib/rubinius/#{LIBVER}"
RBAPATH             = "#{PREFIX}/lib/rubinius/#{LIBVER}/runtime"
EXTPATH             = "#{PREFIX}/lib/rubinius/#{LIBVER}/#{HOST}"

case HOST
when /darwin9/ then
  DARWIN         = 1
  DISABLE_KQUEUE = 1
when /darwin/ then
  DARWIN         = 1
  DISABLE_KQUEUE = 1
else
  DARWIN         = 0
  DISABLE_KQUEUE = (HOST =~ /freebsd/ ? 1 : 0)
end

