require "rbconfig"

$verbose = Rake.application.options.trace || ARGV.delete("-v")

def env(name, default = "")
  (ENV[name] || default).dup
end

# Common settings. These can be augmented or overridden
# by the particular extension Rakefile.
#
DEFAULT = Config::CONFIG

# Don't like the globals? Too bad, they are simple and the
# duration of this process is supposed to be short.

$CC       = env "CC", "gcc"
$CXX      = env "CXX", "g++"
$LDSHARED = env "LDSHARED", $CXX
$YACC     = env "YACC", "bison"

$CFLAGS   = env "CFLAGS"
$CXXFLAGS = env "CXXFLAGS"

$ELIBSDIR = env "ELIBSDIR", File.expand_path("../../vm/external_libs", __FILE__)
$LIBS     = env "LIBS"
$LDDIRS   = env "LDDIRS"
$LDFLAGS  = env "LDFLAGS"

$DLEXT    = env "DLEXT", DEFAULT["DLEXT"]
$LIBEXT   = env "LIBEXT", DEFAULT["LIBEXT"]

$BITS        = 1.size == 8 ? 64 : 32

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

def add_flag(*flags)
  flags.each do |f|
    add_cflag f
    add_cxxflag f
  end
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
  add_include_dir File.expand_path("../../vm/capi/include", __FILE__)
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
  $LDSHARED = "#{$CXX} -dynamic -bundle -undefined suppress -flat_namespace -lstdc++"

when /aix/
  $LDSHARED = "#{$CXX} -shared -Wl,-G -Wl,-brtl"

else
  # on Unix we need a g++ link, not gcc.
  $LDSHARED = "#{$CXX} -shared -lstdc++"
  add_flag "-fPIC"
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
    File.expand_path("../../vm/capi/include/*.h", __FILE__),
    *extra
  ].existing
end

def headers(*extra)
  @headers ||= FileList["*.{h,hpp}", *extra].existing
end

def sources(*extra)
  @sources ||= FileList["*.{c,cpp}", *extra].uniq
end

def objects(dir=nil)
  @objects ||= dir ? sources.pathmap("#{dir}/%X.o") : sources.ext(".o")
end

def dependency_file
  ".depends.mf"
end

def graph_dependencies(sources, directories=[])
  directories = Array(directories)
  directories.concat [".", File.expand_path("../../vm/capi/include", __FILE__)]

  grapher = DependencyGrapher.new sources, directories
  grapher.process

  File.open dependency_file, "w" do |file|
    grapher.print_dependencies file
  end
end

# Helper methods for invoking and reporting on commands
#
def report_command(notice)
  puts notice unless $verbose
end

# Quiet the eff up already. Rakes barfing sh is maddening
#
def qsh(cmd)
  cmd << " > /dev/null" unless $verbose
  puts cmd if $verbose
  unless result = rake_system(cmd)
    fail "Command failed with status (#{$?.exitstatus}): [#{cmd}]"
  end
  result
end

class String
  # Wraps a string in escaped quotes if it contains whitespace.
  def quote
    /\s/ =~ self ? "\"#{self}\"" : "#{self}"
  end

  # Generates a string used as cpp macro name.
  def tr_cpp
    strip.upcase.tr_s("^A-Z0-9_", "_")
  end
end

class Array
  # Wraps all strings in escaped quotes if they contain whitespace.
  def quote
    map {|s| s.quote}
  end
end

module MKMF
  module Logging
    @log = nil
    @logfile = 'mkmf.log'
    @orgerr = $stderr.dup
    @orgout = $stdout.dup
    @postpone = 0
    @quiet = $extmk

    def self::open
      @log ||= File::open(@logfile, 'w')
      @log.sync = true
      $stderr.reopen(@log)
      $stdout.reopen(@log)
      yield
    ensure
      $stderr.reopen(@orgerr)
      $stdout.reopen(@orgout)
    end

    def self::message(*s)
      @log ||= File::open(@logfile, 'w')
      @log.sync = true
      @log.printf(*s)
    end

    def self::logfile file
      @logfile = file
      if @log and not @log.closed?
        @log.flush
        @log.close
        @log = nil
      end
    end

    def self::postpone
      tmplog = "mkmftmp#{@postpone += 1}.log"
      open do
        log, *save = @log, @logfile, @orgout, @orgerr
        @log, @logfile, @orgout, @orgerr = nil, tmplog, log, log
        begin
          log.print(open {yield})
          @log.close
          File::open(tmplog) {|t| FileUtils.copy_stream(t, log)}
        ensure
          @log, @logfile, @orgout, @orgerr = log, *save
          @postpone -= 1
          rm_f tmplog
        end
      end
    end

    class << self
      attr_accessor :quiet
    end
  end

  def message(*s)
    unless Logging.quiet and not $VERBOSE
      printf(*s)
      $stdout.flush
    end
  end

  def checking_for(m, fmt = nil)
    f = caller[0][/in `(.*)'$/, 1] and f << ": " #` for vim
    m = "checking #{/\Acheck/ =~ f ? '' : 'for '}#{m}... "
    message "%s", m
    a = r = nil
    Logging::postpone do
      r = yield
      a = (fmt ? fmt % r : r ? "yes" : "no") << "\n"
      "#{f}#{m}-------------------- #{a}\n"
    end
    message(a)
    Logging::message "--------------------\n\n"
    r
  end

  def checking_message(target, place = nil, opt = nil)
    [["in", place], ["with", opt]].inject("#{target}") do |msg, (pre, noun)|
      if noun
        [[:to_str], [:join, ","], [:to_s]].each do |meth, *args|
          if noun.respond_to?(meth)
            break noun = noun.send(meth, *args)
          end
        end
        msg << " #{pre} #{noun}" unless noun.empty?
      end
    msg
    end
  end

  def cpp_include(header)
    if header
      header = [header] unless header.kind_of? Array
      header.map {|h| "#include <#{h}>\n"}.join
    else
      ""
    end
  end

  MAIN_DOES_NOTHING = "int main() { return 0; }"

  def try_func(func, libs, headers = nil, &b)
    headers = cpp_include(headers)
    try_link(<<"SRC", libs, &b) or
#{headers}
/*top*/
#{MAIN_DOES_NOTHING}
int t() { void ((*volatile p)()); p = (void ((*)()))#{func}; return 0; }
SRC
    try_link(<<"SRC", libs, &b)
#{headers}
/*top*/
#{MAIN_DOES_NOTHING}
int t() { #{func}(); return 0; }
SRC
  end

  class << self
    attr_accessor :libs
  end

  def format(str, *rest)
    str % rest
  end

  def with_config(config, *defaults)
    return *defaults

    config = config.sub(/^--with[-_]/, '')
    val = arg_config("--with-"+config) do
      if arg_config("--without-"+config)
        false
      elsif block_given?
        yield(config, *defaults)
      else
        break *defaults
      end
    end
    case val
    when "yes"
      true
    when "no"
      false
    else
      val
    end
  end

  COMMON_LIBS = []
  LIBARG = "-l%s"

  def append_library(libs, lib) # :no-doc:
    STDERR.puts "Hello" #  [libs, lib]
    format(LIBARG, lib) + " " + libs
  end

  def current_libs
    @libs ||= ""
  end

  def try_link0(src, opt="", &b)
    try_do(src, link_command("", opt), &b)
  end

  def try_link(src, opt="", &b)
    try_link0(src, opt, &b)
  ensure
    rm_f "conftest*", "c0x32*"
  end

  def try_do(src, command, &b)
    unless have_devel?
      raise <<-MSG
The complier failed to generate an executable file.
You have to install development tools first.
      MSG
    end
    src = create_tmpsrc(src, &b)
    xsystem(command)
  ensure
    log_src(src)
  end

  def rm_f(*files)
    FileUtils.rm_f(Dir[files.join("\0")])
  end

  def have_library(lib, func = nil, headers = nil, &b)
    func = "main" if !func or func.empty?
    lib = with_config(lib+'lib', lib)
    checking_for checking_message("#{func}()", LIBARG%lib) do
      if COMMON_LIBS.include?(lib)
        true
      else
        libs = append_library(current_libs, lib)
        if try_func(func, libs, headers, &b)
          @libs = libs
          add_shared_lib lib
          true
        else
          false
        end
      end
    end
  end

  def have_func(func, headers = nil, &b)
    checking_for checking_message("#{func}()", headers) do
      if try_func(func, current_libs, headers, &b)
        add_cflag format("-DHAVE_%s", func.tr_cpp)
        true
      else
        false
      end
    end
  end

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
