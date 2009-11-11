require "rbconfig"

$verbose = Rake.application.options.trace || ARGV.delete("-v")

# Common settings. These can be augmented or overridden
# by the particular extension Rakefile.
#
DEFAULT = Config::CONFIG

# Don't like the globals? Too bad, they are simple and the
# duration of this process is supposed to be short.

$CC          = ENV["CC"]         || "gcc"
$CXX         = ENV["CXX"]        || "g++"
$LDSHARED    = ENV["LDSHARED"]   || $CXX
$YACC        = ENV["YACC"]       || "bison"

$CFLAGS      = ENV["CFLAGS"]     || ""
$CXXFLAGS    = ENV["CXXFLAGS"]   || ""

$ELIBSDIR    = ENV["ELIBSDIR"]   || File.expand_path("../../vm/external_libs", __FILE__)
$LIBS        = ENV["LIBS"]       || ""
$LDDIRS      = ENV["LDDIRS"]     || ""
$LDFLAGS     = ENV["LDFLAGS"]    || ""

$DLEXT       = ENV["DLEXT"]      || DEFAULT["DLEXT"]
$LIBEXT      = ENV["LIBEXT"]     || DEFAULT["LIBEXT"]

# Helper methods for manipulating constants
#
def add_define(*defines)
  defines.each do |d|
    define = "-D#{d}"
    add_cflag define
    add_cxxflag define
  end
end

def add_include_dir(*dirs)
  dirs.each do |f|
    incl = "-I#{f}"
    add_cflag incl
    add_cxxflag incl
  end
end

def add_cflag(*flags)
  flags.each { |f| $CFLAGS << " #{f}" }
end

def add_cxxflag(*flags)
  flags.each { |f| $CXXFLAGS << " #{f}" }
end

def add_ldflag(*flags)
  flags.each { |f| $LDFLAGS << " #{f}" }
end

def add_link_dir(*dirs)
  dirs.each { |d| $LDDIRS << " -L#{d}" }
end

def add_lib(*libs)
  libs.each { |l| $LIBS << " lib#{l}.#{$LIBEXT}" }
end

def add_shared_lib(*libs)
  libs.each { |l| $LIBS << " -l#{l}" }
end

def add_external_lib(*libs)
  libs.each { |l| $LIBS << " #{$ELIBSDIR}/lib#{l}/lib#{l}.#{$LIBEXT}" }
end

def add_mri_capi
  add_cflag DEFAULT["DEFS"]
  add_cflag DEFAULT["CFLAGS"]

  add_cxxflag DEFAULT["DEFS"]
  add_cxxflag DEFAULT["CFLAGS"]

  $LIBS << " #{DEFAULT["LIBS"]}"
  $LIBS << " #{DEFAULT["DLDLIBS"]}"

  add_ldflag DEFAULT["LDSHARED"].split[1..-1].join(' ')
  add_ldflag DEFAULT["LDFLAGS"]
  rubyhdrdir = DEFAULT["rubyhdrdir"]
  if RUBY_VERSION =~ /\A1\.9\./
    arch_hdrdir = "#{rubyhdrdir}/#{DEFAULT['arch']}"
    add_include_dir rubyhdrdir
    add_include_dir arch_hdrdir
    add_link_dir DEFAULT["archdir"]
  else
    add_include_dir DEFAULT["archdir"]
  end
end

def add_rbx_capi
  add_include_dir File.expand_path("../../vm/capi", __FILE__)
end

# Setup some initial computed values
#
add_include_dir "."
add_link_dir "."

# Setup platform-specific values
#
# (Adapted from EventMachine. Thank you EventMachine and tmm1 !)
#
case RUBY_PLATFORM
when /mswin32/, /mingw32/, /bccwin32/
  # TODO: discovery helpers
  #check_heads(%w[windows.h winsock.h], true)
  #check_libs(%w[kernel32 rpcrt4 gdi32], true)

  if GNU_CHAIN
    $LDSHARED = "#{$CXX} -shared -lstdc++"
  else
    add_define "-EHs", "-GR"
  end

when /solaris/
  add_define "OS_SOLARIS8"

  if $CC == "cc" and `cc -flags 2>&1` =~ /Sun/ # detect SUNWspro compiler
    # SUN CHAIN
    add_define "CC_SUNWspro", "-KPIC"
    $CXX = $CC
    $LDSHARED = "#{$CXX} -G -KPIC -lCstd"
  else
    # GNU CHAIN
    # on Unix we need a g++ link, not gcc.
    $LDSHARED = "#{$CXX} -shared"
  end

when /openbsd/
  # OpenBSD branch contributed by Guillaume Sellier.

  # on Unix we need a g++ link, not gcc. On OpenBSD, linking against
  # libstdc++ have to be explicitly done for shared libs
  $LDSHARED = "#{$CXX} -shared -lstdc++ -fPIC"

when /darwin/
  # on Unix we need a g++ link, not gcc.
  # Ff line contributed by Daniel Harple.
  $LDSHARED = "#{$CXX} -dynamic -bundle -undefined suppress -flat_namespace"

when /aix/
  $LDSHARED = "#{$CXX} -shared -Wl,-G -Wl,-brtl"

else
  # on Unix we need a g++ link, not gcc.
  $LDSHARED = "#{$CXX} -shared"
end

# To quiet MRI's warnings about ivars being uninitialized.
# Doesn't need to be a method, but it's nicely encapsulated.
def init
  @common_headers = nil
  @headers = nil
  @sources = nil
  @objects = nil
  @noise_maker = nil
end

init

# Helper methods to collect the project files according to category
#
def common_headers(*extra)
  @common_headers ||= FileList[
    File.expand_path("../../vm/capi/*.h", __FILE__),
    *extra
  ].existing
end

def headers(*extra)
  @headers ||= FileList["*.{h,hpp}", *extra].existing
end

def sources(*extra)
  @sources ||= FileList["*.{c,cpp}", *extra].uniq
end

def objects(*extra)
  @objects ||= sources.ext(".o")
end

# Helper methods for invoking and reporting on commands
#
def report_command(notice)
  puts notice unless $verbose
end

# Quiet the eff up already. Rakes barfing sh is maddening
#
def qsh(cmd)
  cmd << " > /dev/null 2>&1" unless $verbose
  puts cmd if $verbose
  unless result = rake_system(cmd)
    fail "Command failed with status (#{$?.exitstatus}): [#{cmd}]"
  end
  result
end

# Rules for building all files
#
rule ".cpp" => ".y" do |t|
  report_command "YACC #{t.source}"
  qsh "#{$YACC} -o #{t.name} #{t.source}"
end

rule ".o" => ".c" do |t|
  report_command "CC #{t.source}"
  qsh "#{$CC} -c -o #{t.name} #{$CFLAGS} #{t.source}"
end

rule ".o" => ".cpp" do |t|
  report_command "CXX #{t.source}"
  qsh "#{$CXX} -c -o #{t.name} #{$CXXFLAGS} #{t.source}"
end

rule ".#{$DLEXT}" do |t|
  report_command "LDSHARED #{t.name}"
  qsh "#{$LDSHARED} #{objects} #{$LDFLAGS} #{$LDDIRS} #{$LIBS} -o #{t.name}"
end
