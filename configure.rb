#!/usr/bin/env ruby

require './rakelib/configure'
require './rakelib/release'
require './rakelib/build_signature'
require 'rbconfig'
require 'tempfile'
require 'fileutils'
require 'stringio'
require 'date'
require 'digest/md5'
require 'digest/sha2'
require 'net/http'

module Rubinius
  BUILD_CONFIG = {}
end

root = File.expand_path File.dirname(__FILE__)

require File.join(root, "core", "options")

class Configure

  # Default settings only. All code that may depend on user-selected options
  # must run after options are processed.
  def initialize(root)
    @log = Logger.new "configure.log"

    @command_line = ARGV.dup
    @log.log "Command line: #{@command_line.join(" ").inspect}"

    @features = {}
    @defines = []
    @config = File.join(root, "build/config/config.rb")

    # Platform settings
    @host = `sh -c ./build/bin/config.guess`.chomp
    @cpu = nil
    @vendor = nil
    @os = nil
    @windows = nil
    @darwin = nil
    @bsd = nil
    @linux = nil
    @little_endian = false
    @sizeof = {}
    @gc_stack_check = false
    @log_concurrent_update = false
    @raise_concurrent_update = false

    # Build tools
    @cc = nil
    @cxx = nil
    @make = nil
    @rake = nil
    @tar = nil
    @bzip = nil
    @perl = nil
    @gem = nil

    # LLVM settings
    @llvm_path              = nil
    @llvm_system_name       = get_system_name
    @llvm_configure         = nil
    @llvm_version           = nil
    @llvm_api_version       = nil
    @llvm_shared            = false
    @llvm_shared_objs       = nil
    @llvm_cxxflags          = ""
    @llvm_ldflags           = ""

    # System settings
    @libc         = nil
    @x86_64       = false
    @aarch64      = false
    @dtrace       = false
    @dtrace_const = false
    @have_lchmod  = false
    @have_lchown  = false
    @have_mkfifo  = false
    @debug_build  = false
    @include_dirs = []
    @lib_dirs     = []

    # File system paths
    @sourcedir    = root
    @prefixdir    = nil
    @bindir       = nil
    @appdir       = nil
    @libdir       = nil
    @encdir       = nil
    @runtimedir   = nil
    @codedbdir    = nil
    @codetoolsdir = nil
    @stdlibdir    = nil
    @coredir      = nil
    @sitedir      = nil
    @archdir      = nil
    @vendordir    = nil
    @mandir       = nil
    @gemsdir      = nil
    @includedir   = nil

    @build_libdir = nil
    @builddir     = nil
    @scriptdir    = nil
    @capi_includedir  = "#{@sourcedir}/machine/include/capi"

    @bootstrap_gems_dir = nil

    @vm_release_h = File.join(root, "/machine/release.h")

    @preserve_prefix = false

    @program_name = "rbx"
    @bin_links = ["rbx", "ruby", "rake", "gem", "irb", "rdoc", "ri", "erb"]
    @use_bin_links = true

    # List of all gems to pre-install.
    @gems_list = File.join(root, "gems_list.txt")
    @gem_files = File.readlines(@gems_list).map { |x| x.chomp }
    @gem_names = @gem_files.map { |x| /(.*)-\d+\.\d+(\.\d+)?\.gem$/.match(x)[1] }

    @installed_gems = [
      "bundler-1.16.1.gem",
      "minitest-5.11.1.gem",
      "racc-1.4.14.gem",
      "rake-12.3.0.gem",
      "rdoc-5.1.0.gem",
      "rb-readline-0.5.5.gem",
      "test-unit-3.2.7.gem",
     ]

    # Default cache directory, can be overwritten by the user.
    @gems_cache = File.expand_path "../build/libraries/cache", __FILE__

    # Vendored library settings
    @build_libdir = File.join(root, "/build/libraries")

    # Ruby compatibility version
    @ruby_version = "10.0"
    @ruby_libversion = @ruby_version.split(/\./)[0..1].join.to_i

    @build_bin = "#{@sourcedir}/build/bin"

    # Configure settings
    @release_build = !in_git?
  end

  # Set up system commands to run in cmd.exe on Windows. Either Windows
  # or MRI on Windows has issues with subprocesses where the invocation
  # of the subprocess will return before the subprocess has finished.
  # This manifests in configure when uncompressing LLVM source returns
  # but attempting to move the directory fails sporadically with an access
  # exception. Adding the, essentially no-op, 'sleep 0' resolves this.
  def msys_system(cmd)
    old_system %[cmd.exe /C "#{cmd} && sleep 0"]
  end

  def msys_backquote(cmd)
    old_backquote %[cmd.exe /C "#{cmd}"]
  end

  def expand(path)
    File.expand_path(path)
  end

  def expand_install_dir(dir)
    dir = expand dir
    if !@preserve_prefix and File.directory?(dir) and dir !~ /(rubinius|rbx).*\/?$/
      original = dir
      dir += "/rubinius/#{@libversion}"
      @log.write "The directory #{original} already exists, installing to #{dir}"
    end
    dir
  end

  def set_host
    /([^-]+)-([^-]+)-(.*)/ =~ @host
    @cpu, @vendor, @os = $1, $2, $3

    # TODO: For better cross-compiling support, it may be necessary to
    # use the feature facility to check for a define in the compiler.
    @windows = (@host =~ /mingw|mswin/) != nil
    @darwin  = (@host =~ /darwin/) != nil
    @bsd     = (@host =~ /bsd/) != nil
    @linux   = (@host =~ /linux/) != nil
  end

  def set_system_commands
    # Set up system commands to run in cmd.exe on Windows.
    if @windows
      alias :old_system    :system
      alias :old_backquote :`
      alias :system        :msys_system
      alias :`             :msys_backquote
    end
  end

  def set_filesystem_paths
    @prefixdir = @prefixdir ? expand_install_dir(@prefixdir) : @sourcedir

    if @appdir
      dir = expand_install_dir @appdir

      @libdir     = dir + "/library"
      @runtimedir = dir + "/runtime"
      @codedbdir  = dir + "/codedb"
      @coredir    = dir + "/core"
      @sitedir    = dir + "/site"
      @archdir    = dir + "/site/#{@cpu}-#{@os}"
      @encdir     = dir + "/site/#{@cpu}-#{@os}/encoding/converter"
      @vendordir  = dir + "/vendor"
    end

    @bindir       = @prefixdir + "/bin" unless @bindir
    @libdir       = @prefixdir + "/library" unless @libdir
    @runtimedir   = @prefixdir + "/runtime" unless @runtimedir
    @codedbdir    = @prefixdir + "/codedb" unless @codedbdir
    @coredir      = @prefixdir + "/core" unless @coredir
    @sitedir      = @prefixdir + "/site" unless @sitedir
    @archdir      = @prefixdir + "/site/#{@cpu}-#{@os}" unless @archdir
    @encdir       = @prefixdir + "/site/#{@cpu}-#{@os}/encoding/converter" unless @encdir
    @vendordir    = @prefixdir + "/vendor" unless @vendordir
    @mandir       = @prefixdir + "/man" unless @mandir
    @gemsdir      = @prefixdir + "/gems" unless @gemsdir
    @includedir   = @prefixdir + "/machine/include/capi" unless @includedir


    dirs = [@bindir, @libdir, @runtimedir, @codedbdir, @coredir, @sitedir,
            @archdir, @vendordir, @mandir, @gemsdir, @includedir, @encdir]

    parts = dirs.map { |d| d.split "/" }

    i = 0
    total = parts[0].size
    prefix = []

    while i < total
      part = parts[0][i]
      break unless parts.all? { |p| p[i] == part }
      prefix << part
      i += 1
    end

    @prefixdir = prefix.join "/"
    size = @prefixdir.size

    dirs.each { |d| d.replace d[size..-1] }

    @scriptdir = "#{@sourcedir}/build/scripts"

    @builddir = "#{@sourcedir}/build/rubinius" unless @builddir

    stat = File.stat @builddir if File.exist? @builddir

    if stat and stat.owned? and not @builddir.empty? and @builddir != "/"
      FileUtils.rm_r @builddir
    end

    if @preserve_prefix
      @builddir = File.expand_path "#{@builddir}/#{@prefixdir}"
    end

    FileUtils.mkdir_p @builddir

    @bootstrap_gems_dir ||= "#{@sourcedir}/build/libraries/gems"
    @codetoolsdir = "#{@sourcedir}/build/codetools"
    @stdlibdir = "#{@sourcedir}/build/stdlib"
  end

  def add_opt_dir(dir)
    @include_dirs << "#{dir}/include"
    @lib_dirs     << "#{dir}/lib" << "#{dir}/lib64"
  end

  def options
    @options = Rubinius::Options.new "Usage: configure [options]", 30
    o = @options
    o.left_align

    o.doc " Configure settings"

    o.on "--log-file", "NAME", "Write log to file NAME" do |name|
      old_log = @log.path
      @log = Logger.new name, false
      @log.replace old_log
    end

    o.on "--make", "NAME", "Use NAME as 'make' during build" do |name|
      @make = name
    end

    o.on "--rake", "NAME", "Use NAME as 'rake' during build" do |name|
      @rake = name
    end

    o.on "--tar", "NAME", "Use NAME as 'tar'" do |name|
      @tar = name
    end

    o.on "--bzip", "NAME", "Use NAME as 'bzip'" do |name|
      @bzip = name
    end

    o.on "--perl", "NAME", "Use NAME as 'perl' during build" do |name|
      @perl = name
    end

    o.on "--gem", "NAME", "Use NAME as 'gem' during build" do |name|
      @gem = name
    end

    o.on "--debug-build", "Disable C++ optimizations and retain debugging symbols" do
      @debug_build = true
    end

    o.on "--sanitize", "SANITIZER", "Enable the Clang sanitizer: 'memory', 'address', 'undefined'" do |sanitizer|
      if ["address", "memory", "undefined"].include?(sanitizer)
        @debug_build = true
        (@system_cxxflags ||= "") << " -fsanitize=#{sanitizer}  -fno-omit-frame-pointer -fno-optimize-sibling-calls "
        @system_cxxflags << " -fsanitize-address-use-after-scope " if sanitizer == "address"
        @system_cxxflags << " -fsanitize-memory-track-origins " if sanitizer == "memory"

        (@system_ldflags ||= "") << " -g -fsanitize=#{sanitizer} "
      end
    end

    o.on "--release-build", "Build from local files instead of accessing the network" do
      @release_build = true
    end

    o.on "--no-release-build", "Build from the network instead of local files" do
      @release_build = false
    end

    o.on "--gc-stack-check", "Emit stack trace for all threads on pause for GC" do
      @gc_stack_check = true
    end

    o.on "--log-concurrent-update", "Log when T2 updates an object created by T1" do
      @log_concurrent_update = true
    end

    o.on "--raise-concurrent-update", "Raise a RuntimeError when T2 updates an object created by T1" do
      @raise_concurrent_update = true
    end

    o.doc "\n Compiler settings"

    o.on "--cc", "COMPILER", "Compiler to use for C code (eg clang)" do |cc|
      @cc = cc
    end

    o.on "--cxx", "COMPILER", "Compiler to use for C++ code (eg clang++)" do |cxx|
      @cxx = cxx
    end

    o.doc "\n LLVM settings"

    o.on "--system-name", "NAME", "Name of OS (eg fedora-8, ubuntu-10.04)" do |name|
      @llvm_system_name = name
    end

    o.on "--llvm-path", "PATH", "File system path to the directory containing LLVM" do |dir|
      @llvm_path = dir.dup
    end

    o.on "--llvm-config", "PROGRAM", "File system path to the llvm-config program" do |program|
      @llvm_configure = program
    end

    o.on "--llvm-shared", "Link to shared LLVM library" do
      @llvm_shared = true
    end

    o.doc "\n System settings"

    o.on "--with-include-dir", "DIR", "Add DIR to the default include search paths" do |dir|
      dir.split(File::PATH_SEPARATOR).each do |d|
        @include_dirs << d
      end
    end

    o.on "--with-lib-dir", "DIR", "Add DIR to the default library search paths" do |dir|
      dir.split(File::PATH_SEPARATOR).each do |d|
        @lib_dirs << d
      end
    end

    o.on "--with-opt-dir", "DIR", "Add DIR/include and DIR/lib to include and library search paths" do |dir|
      dir.split(File::PATH_SEPARATOR).each do |d|
        add_opt_dir(d)
      end
    end

    o.on "--libc", "NAME", "Use NAME as the libc for FFI" do |name|
      @libc = name
    end

    o.on "--host", "HOST", "Override guessed platform with HOST specification" do |host|
      @log.write "------------------------------------------------------"
      @log.write "\nChanging the platform specification can cause Rubinius"
      @log.write "to malfunction. The current platform specification is:"
      @log.write "\n#{@host}"
      @log.write "\n------------------------------------------------------"

      @host = host
    end

    o.doc "\n Program names"

    o.on "--program-name", "NAME", "Build Rubinius executable as NAME" do |name|
      @program_name = name
    end

    o.on "--bin-link", "NAME", "Create NAME as binary symlink to program name" do |name|
      @bin_links << name
    end

    o.on "--no-bin-links", "Do not create any symlinks to program name" do
      @use_bin_links = false
    end

    o.doc "\n File system paths for installing Rubinius"

    o.on "-P", "--prefix", "PATH", "Install Rubinius in subdirectories of PATH" do |dir|
      warn_prefix dir
      @prefixdir = dir.dup
    end

    o.on "-B", "--bindir", "PATH", "Install Rubinius executable in PATH" do |dir|
      @bindir = expand dir
    end

    o.on "-I", "--includedir", "PATH", "Install Rubinius C-API include files in PATH" do |dir|
      @includedir = expand dir
    end

    o.on "-A", "--appdir", "PATH", "Install Ruby runtime and libraries in PATH" do |dir|
      @appdir = dir.dup
    end

    o.on "-L", "--libdir", "PATH", "Install Rubinius shared library in PATH" do |dir|
      @libdir = dir.dup
    end

    o.on "-M", "--mandir", "PATH", "Install man pages in PATH" do |dir|
      @mandir = expand dir
    end

    o.on "-G", "--gemsdir", "PATH", "Install gems in PATH" do |dir|
      @gemsdir = expand dir
    end

    o.on "--gems-cache", "PATH", "Cache Gems in PATH during compilation" do |dir|
      @gems_cache = expand dir
    end

    o.on "--sitedir", "PATH", "Install site-specific Ruby code in PATH" do |dir|
      @sitedir = expand dir
    end

    o.on "--archdir", "PATH", "Install arch-specific native extensions in PATH" do |dir|
      @archdir = expand dir
    end

    o.on "--vendordir", "PATH", "Install vendor-specific Ruby code in PATH" do |dir|
      @vendordir = expand dir
    end

    o.on "--preserve-prefix", "Use the configure prefix for staging Rubinius to install" do
      @preserve_prefix = true
    end

    o.on "--stagingdir", "PATH", "Use PATH to build and prepare all files for install" do |dir|
      @builddir = expand dir
    end

    o.doc "\n Optional features"

    feature "execinfo", true
    feature "vendor-zlib", false
    feature "vendor-libsodium", true
    feature "alloc-tracking", false
    feature "dtrace", false
    feature "rpath", false

    o.doc "\n Help!"

    o.on "--show", "Print the current configuration and exit" do
      print_debug
      exit 0
    end

    o.on "-V", "--verbose", "Print additional info" do
      @verbose = true
    end

    o.help

    o.doc ""

  end

  def feature(name, default_value=true)
    @features[name] = ConfigurationToggle.new default_value

    @options.on "--with-#{name}", "Enable #{name}" do
      @features[name].configured = true
    end

    @options.on "--without-#{name}", "Disable #{name}" do
      @features[name].configured = false
    end
  end

  def parse(ary)
    @options.parse ary
  end

  def md5_checksum(md5_path, full_path)
    return Digest::MD5.file(full_path).hexdigest == File.read(md5_path).strip.split(" ").first
  end

  def download(url, full_path, follows=0)
    begin
      dir = File.dirname full_path
      Dir.mkdir dir unless File.exist? dir

      uri = url.kind_of?(URI) ? url : URI(url)

      if ENV['http_proxy']
        protocol, userinfo, p_host, p_port  = URI::split(ENV['http_proxy'])
        p_user, p_pass = userinfo.split(/:/) if userinfo
        http = Net::HTTP.new(uri.host, uri.port, p_host, p_port, p_user, p_pass)
      else
        http = Net::HTTP.new(uri.host, uri.port)
      end
      http.use_ssl = true if uri.scheme == 'https'
      request = Net::HTTP::Get.new(uri.request_uri)

      http.request(request) do |res|
        case res
        when Net::HTTPNotFound
          @log.write "      #{url} not found."
          return false
        when Net::HTTPMovedPermanently,
             Net::HTTPFound,
             Net::HTTPSeeOther,
             Net::HTTPTemporaryRedirect
          if follows > 3
            @log.write "      ERROR: too many redirects: #{url}"
            return false
          end

          return download URI.parse(res['Location']), full_path, follows + 1
        when Net::HTTPClientError
          @log.write "      ERROR: #{res.inspect}"
          return false
        end

        size = 0
        total = res.header['Content-Length'].to_i

        @log.write "    Downloading #{File.basename(full_path)}..."
        File.open full_path, "wb" do |f|
          res.read_body do |chunk|
            f << chunk
            size += chunk.size
            print "\r      [ %d%% (%d of %d) ]" % [(size * 100) / total, size, total]
          end
        end
        @log.write ": done!"
      end
    rescue Interrupt
      File.unlink full_path if File.exist?(full_path)
      raise
    rescue StandardError => e
      File.unlink full_path if File.exist?(full_path)
      @log.write " ERROR: #{e.message}"
      return false
    end

    return true
  end

  def setup_llvm
    @log.print "  Checking for 'llvm-config': "

    config = @llvm_configure
    if !config
      which = ENV['PATH'].split(":").find do |path|
        File.exist? File.join(path, "llvm-config")
      end
      if which
        config = File.join(which, "llvm-config")
      elsif @darwin
        if macports?
          config = macports_llvm_config
        else
          out = brew "--prefix llvm"
          config = "#{out}/bin/llvm-config" if $?.success?
        end
      end
    end

    if config
      config_cmd = llvm_config_cmd config
      begin
        version = `#{config_cmd} --version`.strip

        # Ruby 1.8 returns an empty string
        failed = true if version.empty?
      rescue Errno::ENOENT
        # Ruby 1.9 raises this error
        failed = true
      end

      unless failed
        parts = version.sub(/svn$/, "").split(".").map { |i| i.to_i }
        api_version = ("%d%02d" % parts[0..1]).to_i
        if api_version < 306
          @log.write "only LLVM 3.6+ is supported"
        else
          @log.write "found! (version #{version} - api: #{api_version})"
          @llvm = :config
          @llvm_configure = config_cmd
          @llvm_version = version
          @llvm_api_version = api_version

          check_llvm_flags

          if @llvm_shared
            setup_llvm_shared
          end

          return true
        end
      else
        @log.write "executing #{config_cmd.inspect} failed"
      end
    else
      @log.write "not found"
    end

    failure "ABORT: unable to set up LLVM"
  end

  def setup_llvm_shared
    @log.print "  Checking for LLVM shared libs: "

    src = <<-EOP
#include <llvm/IR/LLVMContext.h>
using namespace llvm;
int main() { LLVMContext &Context = getGlobalContext(); }
    EOP

    common_args = "`#{@llvm_configure} --cppflags` #{@llvm_cxxflags} #{@llvm_ldflags}".strip.split(/\s+/)
    shared_configs = {
      "libLLVM-#{@llvm_version}"  => ["-lLLVM-#{@llvm_version}"],
      "#{@llvm_configure} --libs" => `#{@llvm_configure} --libs`.strip.split(/\s+/)
    }

    shared_configs.each do |desc, objs|
      status = check_program(false, *(common_args + objs)) do |f|
        f.puts src
        @log.log src
      end

      if status == 0
        @log.write "found! (using #{desc})"
        @llvm_shared_objs = objs
        return true
      end
    end

    @log.write "not found"
    false
  end

  def check_llvm_flags
    flags = '--ldflags'

    # Starting with LLVM 3.5 the --system-libs option is required in order to
    # link against libraries such as zlib. Prior to 3.5 this was handled by
    # --ldflags.
    flags << ' --system-libs'

    # Generate the actual flags. For whatever reason llvm-config also includes
    # newlines in the output, so lets get rid of those while we're at it.
    @llvm_ldflags = `#{@llvm_configure} #{flags}`.strip.gsub("\n", ' ')
  end

  def env(which)
    ENV[which] || ""
  end

  def default_link_libs
    libs = []
    unless @host =~ /haiku/
      libs << "m"
    end
    libs
  end

  def failure(message=nil)
    @log.error message if message

    STDERR.puts "\nRunning 'configure' failed. Please check configure.log for more details."
    exit 1
  end

  def supported_compiler(name)
    failure <<-EOM
Unable to find #{name} compiler. Support for compilers other than #{name}
compiler was deprecated 1 Jun 2016 and has been removed. If your platform does
not support #{name} compiler, please email contact@rubinius.com
    EOM
  end

  def default_cc
    return 'clang' if `clang --version > /dev/null 2>&1` && $?.success?
    supported_compiler "clang C"
  end

  def default_cxx
    return 'clang++' if `clang++ --version > /dev/null 2>&1` && $?.success?
    supported_compiler "clang++ C++"
  end

  def check_tools
    @cc ||= ENV['CC'] || default_cc
    @cxx ||= ENV['CXX'] || default_cxx

    check_tool_version @cc, '-dumpversion', [4, 1]
    check_tool_version @cxx, '-dumpversion', [4, 1]

    supported_compiler "clang C" unless @cc =~ /clang|ccc-analyzer/
    supported_compiler "clang++ C++" unless @cxx =~ /clang\+\+|c\+\+\-analyzer/

    if File.exist? @build_bin
      if !File.directory? @build_bin
        fail "#{@build_bin} already exists and is not a directory"
      end
    else
      FileUtils.mkdir_p @build_bin
    end

    if @cc != "cc"
      cc = "#{@build_bin}/cc"
      File.symlink `which #{@cc}`.chomp, cc unless File.exist? cc
    end

    if @cxx != "c++"
      cxx = "#{@build_bin}/c++"
      File.symlink `which #{@cxx}`.chomp, cxx unless File.exist? cxx
    end

    @make ||= ENV['MAKE'] || 'make'
    @rake ||= ENV['RAKE'] || 'rake'
    @tar ||= ENV['TAR'] || (@windows ? 'bsdtar' : 'tar')
    @bzip ||= ENV['BZIP'] || 'bzip2'
    @perl ||= ENV['PERL'] || 'perl'
    @gem ||= ENV['GEM'] || 'gem'

    @gcc_major = `#{@cc} -dumpversion`.strip.split(".")[0,2].join(".")
    if @host == "i686-pc-linux-gnu" || @host == "x86_64-unknown-linux-gnu"
      @llvm_generic_prebuilt  = "llvm-#{@llvm_version}-#{@host}-#{@gcc_major}.tar.bz2"
    else
      @llvm_generic_prebuilt  = "llvm-#{@llvm_version}-#{@host}.tar.bz2"
    end

    @system_cflags    ||= ""
    (@system_cxxflags ||= "") << "-std=c++14 "
    @system_cppflags  ||= ""
    @system_incflags  ||= ""
    @system_ldflags   ||= ""

    @user_cflags =   ENV['CFLAGS']
    @user_cxxflags = ENV['CXXFLAGS']
    @user_cppflags = ENV['CPPFLAGS']
    @user_incflags = ENV['INCFLAGS']
    @user_ldflags =  ENV['LDFLAGS']

    setup_platform
  end

  def setup_platform
    @ldsharedxx = "#{@cxx} -shared"
    @ldshared   = "#{@cc} -shared"

    @include_dirs.each do |d|
      @system_incflags << "-I#{d} "
    end
    @lib_dirs.each do |d|
      @system_ldflags << "-L#{d} "
    end

    case RUBY_PLATFORM
    when /mswin/i, /mingw/i, /bccwin32/i
      # TODO: discovery helpers
      #check_heads(%w[windows.h winsock.h], true)
      #check_libs(%w[kernel32 rpcrt4 gdi32], true)

      unless RUBY_PLATFORM =~ /mingw/
        @system_cflags << "-EHs -GR"
      end
      @system_ldflags << "-lws2_32"
      @features["rpath"].configured = false
    when /solaris/i
      # GNU CHAIN only supported
      @ldsharedxx = "#{@cxx} -shared -G -fPIC -lstdc++"
      @ldshared   = "#{@cc} -shared -G -fPIC"
      @system_cflags << "-fPIC -Wno-strict-aliasing"
      @system_ldflags << "-lsocket -lnsl -fPIC"
      @features["rpath"].configured = false
      @make = "gmake"
    when /freebsd/i
      @ldsharedxx = "#{@cxx} -shared -fPIC"
      @ldshared   = "#{@cc} -shared -fPIC"
      @system_cflags << "-fPIC"
      @system_ldflags << "-lcrypt -pthread -rdynamic"
      @make = "gmake"
    when /openbsd/i
      # OpenBSD branch contributed by Guillaume Sellier.

      # on Unix we need a g++ link, not gcc. On OpenBSD, linking against
      # libstdc++ have to be explicitly done for shared libs
      @ldsharedxx = "#{@cxx} -shared -lstdc++ -fPIC"
      @ldshared   = "#{@cc} -shared -fPIC"
      @system_cflags << "-fPIC"
      @system_ldflags << "-pthread -rdynamic -Wl,--export-dynamic"
      @make = "gmake"
    when /netbsd/i
      @ldsharedxx = "#{@cxx} -shared -lstdc++ -fPIC"
      @ldshared   = "#{@cc} -shared -fPIC"
      @system_cflags << "-fPIC"
      @system_ldflags << "-lcrypt -pthread -rdynamic -Wl,--export-dynamic"
      @make = "gmake"
    when /darwin/i
      # on Unix we need a g++ link, not gcc.
      # Ff line contributed by Daniel Harple.
      @ldsharedxx = "#{@cxx} -bundle -undefined suppress -flat_namespace"
      @ldshared   = "#{@cc} -bundle -undefined suppress -flat_namespace"
      @system_cflags << "-fPIC -D_DARWIN_USE_64_BIT_INODE"
      @features["rpath"].configured = false
    when /haiku/i
      @system_cflags << "-fPIC"
      @system_ldflags << "-ldl -lnetwork"
      @features["rpath"].configured = false
    when /aix/i
      @ldsharedxx = "#{@cxx} -shared -Wl,-G -Wl,-brtl"
      @ldshared   = "#{@cc} -shared -Wl,-G -Wl,-brtl"
      @features["rpath"].configured = false
    when /linux/i
      @system_cflags << "-fPIC"
      @system_ldflags << "-Wl,--export-dynamic -lrt -lcrypt -ldl -lpthread"
    else
      # on Unix we need a g++ link, not gcc.
      @system_cflags << "-fPIC"
      @system_ldflags << "-ldl -lpthread"
    end

    if @features["rpath"].value
      @lib_dirs.each do |d|
        @system_ldflags << " -Wl,-rpath=#{d}"
      end
    end
  end

  def check_program(run=true, *arguments)
    begin
      basename = "rbx-configure-test"
      source   = basename + ".cpp"
      File.open source, "wb" do |f|
        yield f
      end

      File.open source, "rb" do |f|
        @log.log f.read
      end

      libs = default_link_libs.map { |l| "-l#{l}" }.join(" ")
      args = arguments.join(" ")

      cmd = "#{@cxx} #{@user_cppflags} #{@user_cflags} #{@user_cxxflags} #{@user_incflags} #{@user_ldflags} -o #{basename} #{source} #{@system_cppflags} #{@system_cflags} #{@system_cxxflags} #{@system_incflags} #{@system_ldflags} #{libs} #{args} >>#{@log.path} 2>&1"
      @log.log cmd
      system cmd
      return $?.exitstatus unless run

      unless $?.exitstatus == 0
        @log.log msg = "Compiling configure test program failed."
        raise RuntimeError, msg
      end

      system expand("./#{basename}")
      return $?.exitstatus
    rescue => e
      @log.log "Error in check_program: #{e.class} #{e.message}\n  #{e.backtrace.join("\n  ")}"
      raise e
    ensure
      FileUtils.rm_r(Dir["#{basename}*"])
    end
  end

  def write_have_defines(f)
    f.puts
    @defines.each { |d| f.puts "#define #{d.ljust(20)} 1" }
  end

  def write_have_sizeof_defines(f)
    f.puts
    @sizeof.keys.sort.each { |k| f.puts "#define HAVE_#{k}".ljust(30) + "1" }
  end

  def write_sizeof_defines(f)
    f.puts
    @sizeof.keys.sort.each { |k| f.puts "#define SIZEOF_#{k}".ljust(30) + @sizeof[k].to_s }
  end

  def sizeof_typename(type)
    if type =~ /(\*+)$/
      name = "#{type[0...-$1.size]}#{"p" * $1.size}"
    else
      name = type
    end
    name.gsub(/\W/, "_").upcase
  end

  def sizeof(type)
    @sizeof[sizeof_typename(type)] or failure("Unknown type: '#{type}'.")
  end

  def assert_sizeof
    @log.print "Checking sizeof(intptr_t) == sizeof(int64_t): "

    status = check_program do |f|
      src = <<-EOP
#include <stdint.h>

int main(int argc, char* argv[]) {
  return sizeof(intptr_t) == sizeof(int64_t);
}
      EOP
      f.puts src
      @log.log src
    end

    if status == 1
      @log.write "yes"
    else
      @log.write "no"
      failure "\nRubinius requires that sizeof(intptr_t) == sizeof(int64_t)"
    end
  end

  def detect_sizeof(type, includes=[])
    @log.print "Checking sizeof(#{type}): "

    size = check_program do |f|
      src = includes.map { |include| "#include <#{include}>\n" }.join
      src += <<-EOP
#include <stddef.h>
#include <stdint.h>

int main() { return sizeof(#{type}); }
      EOP
      f.puts src
      @log.log src
    end

    @sizeof[sizeof_typename(type)] = size

    @log.write "#{size} bytes"
  end

  def detect_endian
    @log.print "Checking platform endianness: "

    status = check_program do |f|
      src = "int main() { int one = 1; return (*((char*)&one)) == 1 ? 0 : 1; }"
      f.puts src
      @log.log src
    end

    @little_endian = (status == 0)
    @log.write @little_endian ? "little endian" : "big endian"
  end

  def detect_tr1
    @log.print "Checking for tr1: "

    status = check_program(false) do |f|
      src = <<-EOP
#include <tr1/unordered_map>

typedef std::tr1::unordered_map<int, void*> X;

int main() { X x; return 0; }
      EOP
      f.puts src
      @log.log src
    end

    @tr1 = (status == 0)
    @log.write @tr1 ? "found" : "not found"
  end

  def detect_tr1_hash
    @log.print "Checking for tr1/hash definition: "

    status = check_program(false) do |f|
      src = <<-EOP
#include <stdint.h>
#include <tr1/unordered_map>

typedef std::tr1::unordered_map<uint64_t, void*> X;

int main() { X x; return 0; }
      EOP
      f.puts src
      @log.log src
    end

    @tr1_hash = (status == 0)
    @log.write @tr1_hash ? "found" : "not found"
  end

  def detect_x86
    print "Checking for x86_64: "

    status = check_program do |f|
      src = <<-EOP
int main() {
#if defined(__x86_64) || defined(__x86_64__)
return 1;
#else
return 0;
#endif
}
      EOP

      f.puts src
      @log.log src
    end
    @x86_64 = (status == 1)

    puts @x86_64 ? "yes" : "no"
  end

  def detect_aarch64
    print "Checking for aarch64: "

    status = check_program do |f|
      src = <<-EOP
int main() {
#if defined(__ARM_ARCH_ISA_A64) 
return 1;
#else
return 0;
#endif
}
      EOP

      f.puts src
      @log.log src
    end
    @aarch64 = (status == 1)

    puts @aarch64 ? "yes" : "no"
  end

  def detect_curses
    @log.print "Checking curses library: "

    src = <<-EOP
#include <curses.h>
#include <term.h>

int main() { return tgetnum(""); }
    EOP

    ["-lcurses", "-lncurses", "-ltermcap"].each do |lib|
      status = check_program(false, lib) do |f|
        f.puts src
        @log.log src
      end

      if status == 0
        @curses = lib
        break
      end
    end

    if @curses
      @log.write(@curses)
    end
  end

  def detect_build_dirs
    ["/usr/local", "/opt/local", "/usr/pkg"].each do |dir|
      add_opt_dir(dir)
    end

    @include_dirs = @include_dirs.select {|p| File.directory? p }
    @lib_dirs = @lib_dirs.select {|p| File.directory? p }
  end

  def has_struct_member(struct, member, includes = [])
    compile_check "struct #{struct} has member #{member}" do |src|
      includes.each do |i|
        src.puts "#include <#{i}>"
      end

      src.puts "int main() { struct #{struct} st; st.#{member}; }"
    end
  end

  def has_global(name, includes=[])
    compile_check "global '#{name}'" do |src|
      includes.each do |i|
        src.puts "#include <#{i}>"
      end
      src.puts "int main() { #{name}; }"
    end
  end

  def has_header(name)
    compile_check "header '#{name}'" do |src|
      # Some headers have an implicit dependency on stdio.h. For example,
      # readline/readline.h requires it but doesn't actually include it for
      # you. Because there could be an infinite amount of headers that require
      # stdio.h we'll just always include it.
      src.puts "#include <stdio.h>"

      src.puts "#include <#{name}>"
      src.puts "int main() {return 0;}"
    end
  end

  def has_function(name, includes=[], defines = [])
    compile_check "function '#{name}'", defines do |src|
      includes.each do |i|
        src.puts "#include <#{i}>"
      end
      src.puts "int main() { void* ptr = (void *) &#{name}; }"
    end
  end

  def has_library(name, function, libraries, includes=[])
    @log.print "Checking for library: #{name}: "

    args = libraries.map { |l| "-l#{l}" }

    status = check_program(true, *args) do |src|
      includes.each do |i|
        src.puts "#include <#{i}>"
      end
      src.puts "int main() { void* ptr = (void*)(&#{function}); return 0; }"
    end

    success = status == 0
    @log.write(success ? "found!" : "not found!")
    success
  end

  def has_dtrace
    @log.print "Checking for dtrace: "

    begin
      basename = "rbx-configure-dtrace-test"
      source   = basename + ".d"
      output   = basename + ".h"

      File.open source, "wb" do |f|
        f.write "provider conftest{ probe m__entry(const char*); };"
      end

      cmd = "dtrace -h -o #{output} -s #{source}"
      @log.log cmd
      system cmd

      @dtrace = $?.exitstatus == 0
      @dtrace_const = !!File.read(output).index("const") if @dtrace

      @log.write(@dtrace ? "yes" : "no")

      @dtrace
    ensure
      File.delete(*Dir["#{basename}*"])
    end
  end

  def compile_check(logpart, defines = [], &block)
    @log.print "Checking for #{logpart}: "

    source = StringIO.new
    yield source
    file = Tempfile.new("rbx-test")

    source.rewind
    string = source.read

    file.puts string
    file.close

    @log.log string

    cmd = "#{@cxx} -S -o - -x c++ #{defines.join(" ")} #{@user_cppflags} #{@user_incflags} #{@user_cxxflags} #{@user_cflags} #{@user_ldflags} #{@system_cppflags} #{@system_incflags} #{@system_cxxflags} #{@system_cflags} #{@system_ldflags} #{file.path} >>#{@log.path} 2>&1"
    @log.log cmd
    system cmd

    status = ($?.exitstatus == 0)
    file.unlink

    @log.write(status ? "found!" : "not found")
    status
  end

  def enable_features
    if @features["vendor-zlib"].value
      # Our vendored zlib uses long as the crc_table type
      # If we update vendored zlib in the future, we have to
      # review this and make sure we update it properly to
      # match the newer version which like will have uint32_t
      # as the type.
      @include_dirs << "#{@build_libdir}/zlib"
      @lib_dirs     << "#{@build_libdir}/zlib"
    end

    if @features["vendor-libsodium"].value
      @include_dirs << "#{@build_libdir}/libsodium/src/libsodium/include"
      @lib_dirs     << "#{@build_libdir}/libsodium/src/libsodium/.libs/"
    end
  end

  def detect_features
    # Default on *BSD is no execinfo
    if @bsd and @features["execinfo"].configured.nil?
      @features["execinfo"].configured = false
    end

    if @features["execinfo"].value and has_function("backtrace", ["execinfo.h"])
      @defines << "HAS_EXECINFO"
    end

    if @features["alloc-tracking"].value
      @defines << "RBX_ALLOC_TRACKING"
    end

    if @features["dtrace"].value and has_dtrace
      @defines << "HAVE_DTRACE"
    end

    # Default on Windows is vendor-zlib
    if @windows and @features["vendor-zlib"].configured.nil?
      @features["vendor-zlib"].configured = true
    end

    @defines << "HAVE_SPT_REUSEARGV" if @linux || @darwin || @bsd
  end

  def detect_functions
    if has_function("clock_gettime", ["time.h"])
      @defines << "HAVE_CLOCK_GETTIME"
    end

    if has_function("nl_langinfo", ["langinfo.h"])
      @defines << "HAVE_NL_LANGINFO"
    end

    if has_function("setproctitle", ["sys/types.h", "unistd.h"])
      @defines << "HAVE_SETPROCTITLE"
    end

    if has_function("posix_fadvise", ["fcntl.h"])
      @defines << "HAVE_POSIX_FADVISE"
    end

    if has_function("strnlen", ["string.h"])
      @defines << "HAVE_STRNLEN"
    end

    if has_function("kqueue", ["sys/types.h", "sys/event.h", "sys/time.h"])
      @defines << "HAVE_KQUEUE"
    end

    if has_function("timerfd_create", ["sys/timerfd.h"])
      @defines << "HAVE_TIMERFD"
    end

    if has_function("inotify_init", ["sys/inotify.h"])
      @defines << "HAVE_INOTIFY"
    end

    if has_function("gettid", ["unistd.d", "sys/types.h"])
      @defines << "HAVE_GETTID"
    end

    if has_struct_member("stat", "st_atim", ["sys/stat.h"])
      @defines << "HAVE_STRUCT_STAT_ST_ATIM"
    end

    if has_struct_member("stat", "st_atimespec", ["sys/stat.h"])
      @defines << "HAVE_STRUCT_STAT_ST_ATIMESPEC"
    end

    if has_struct_member("stat", "st_atimensec", ["sys/stat.h"])
      @defines << "HAVE_STRUCT_STAT_ST_ATIMENSEC"
    end

    if has_struct_member("stat", "st_mtim", ["sys/stat.h"])
      @defines << "HAVE_STRUCT_STAT_ST_MTIM"
    end

    if has_struct_member("stat", "st_mtimespec", ["sys/stat.h"])
      @defines << "HAVE_STRUCT_STAT_ST_MTIMESPEC"
    end

    if has_struct_member("stat", "st_mtimensec", ["sys/stat.h"])
      @defines << "HAVE_STRUCT_STAT_ST_MTIMENSEC"
    end

    if has_struct_member("stat", "st_ctim", ["sys/stat.h"])
      @defines << "HAVE_STRUCT_STAT_ST_CTIM"
    end

    if has_struct_member("stat", "st_ctimespec", ["sys/stat.h"])
      @defines << "HAVE_STRUCT_STAT_ST_CTIMESPEC"
    end

    if has_struct_member("stat", "st_ctimensec", ["sys/stat.h"])
      @defines << "HAVE_STRUCT_STAT_ST_CTIMENSEC"
    end

    if has_struct_member("stat", "st_birthtimespec", ["sys/stat.h"])
      @defines << "HAVE_ST_BIRTHTIME"
    end

    # glibc has useless lchmod() so we don't try to use lchmod() on linux
    if !@linux and has_function("lchmod", ["sys/stat.h", "unistd.h"])
      @have_lchmod = true
    end

    if has_function("lchown", ["sys/stat.h", "unistd.h"])
      @have_lchown = true
    end

    if has_function("mkfifo", ["sys/stat.h", "sys/types.h"])
      @have_mkfifo = true
    end
  end

  def detect_structures
    if has_struct_member("tm", "tm_gmtoff", ["time.h"])
      @defines << "HAVE_TM_GMTOFF"
    end

    if has_struct_member("tm", "tm_zone", ["time.h"])
      @defines << "HAVE_TM_ZONE"
    end
  end

  def detect_globals
    if has_global("timezone", ["time.h"])
      @defines << "HAVE_TIMEZONE"
    end

    if has_global("tzname", ["time.h"])
      @defines << "HAVE_TZNAME"
    end

    if has_global("daylight", ["time.h"])
      @defines << "HAVE_DAYLIGHT"
    end
  end

  def detect_headers
    unless @features["vendor-zlib"].value
      unless has_header("zlib.h")
        failure "zlib.h is required"
      end
    end

    unless @features["vendor-libsodium"].value
      unless has_header("sodium.h")
        failure "sodium.h is required"
      end
    end

    unless has_header("openssl/ssl.h")
      failure "openssl/ssl.h is required"
    end

    if has_header("alloca.h")
      @defines << "HAVE_ALLOCA_H"
    end

    if has_header("string.h")
      @defines << "HAVE_STRING_H"
    end

    if has_header("sys/time.h")
      @defines << "HAVE_SYS_TIME_H"
    end

    if has_header("sys/times.h")
      @defines << "HAVE_SYS_TIMES_H"
    end

    if has_header("sys/types.h")
      @defines << "HAVE_SYS_TYPES_H"
    end

    if has_header("unistd.h")
      @defines << "HAVE_UNISTD_H"
    end

    if has_header("stdarg.h")
      @defines << "HAVE_STDARG_H"
    end

    if has_header("sys/pstat.h")
      @defines << "HAVE_SYS_PSTAT_H"
    end

    if has_header("valgrind/valgrind.h")
      @defines << "HAVE_VALGRIND_H"
    end
  end

  def strerror_r_returns_char_pointer
    status = check_program(false) do |src|
      src.puts "#include <string.h>"
      src.puts "int main() { char buf[1024]; static_cast<char*>(strerror_r(42, buf, 1024)); }"
    end
    status == 0
  end

  def detect_strerror
    @log.print "Checking if function 'strerror_r' returns char*: "
    if strerror_r_returns_char_pointer
      @defines << "STRERROR_R_CHAR_P"
      @log.write "yes"
    else
      @log.write "no"
    end
  end

  def detect_atomic
    @log.print "Checking -latomic: "

    saved_ldflags = @system_ldflags.dup
    @system_ldflags << " -latomic "

    begin
      status = check_program() do |src|
        src.puts <<-EOP
#include <stdio.h>
#include <atomic>

int main(int argc, char* argv[]) {
  std::atomic<int> i;
  printf("%d", (int)i);
  return 0;
}
        EOP
      end
    rescue
      status = nil
    end

    if status == 0
      @log.write "yes"
    else
      @log.write "no"
      @system_ldflags = saved_ldflags
    end
  end

  def warn_prefix(dir)
    delimiter = "-------------------------%s-----------------------"

    if File.file? dir
      @log.write delimiter % " ERROR "
      @log.write "The specified prefix '#{dir}' is a regular file."
      @log.write "Remove the file or specify a different prefix."
      @log.write delimiter % "-------"
      exit 1
    elsif File.directory? dir
      @log.write delimiter % " WARNING "
      @log.write "The specified prefix '#{dir}' already exists."
      @log.write "Installing Rubinius into an existing directory may"
      @log.write "overwrite existing unrelated files or cause conflicts"
      @log.write "between different versions of Rubinius files."
      @log.write delimiter % "---------"
      sleep 2
    end
  end

  def process
    set_system_commands

    enable_features
    detect_build_dirs

    setup_llvm

    @log.write ""

    assert_sizeof

    detect_sizeof("short")
    detect_sizeof("int")
    detect_sizeof("void*")
    detect_sizeof("intptr_t")
    detect_sizeof("uintptr_t")
    detect_sizeof("size_t")
    detect_sizeof("long")
    detect_sizeof("long long")
    detect_sizeof("float")
    detect_sizeof("double")
    detect_sizeof("off_t", ["unistd.h"])
    detect_sizeof("time_t", ["time.h"])

    detect_libc_name

    detect_endian
    detect_tr1
    detect_tr1_hash
    detect_x86
    detect_aarch64
    detect_features
    detect_functions
    detect_structures
    detect_globals
    detect_headers
    detect_curses
    detect_strerror
    detect_atomic
 end

  # Checks whether the given config file is a Perl script by checking its first
  # line for a Perl hashbang.
  def llvm_config_cmd(config)
    begin
      File.open(config, "r") do |f|
        first_line = f.readline
        if first_line =~ /^#! ?\/usr(\/local)?\/bin\/(env )?perl/
          "#{@perl} #{config}"
        else
          config
        end
      end
    rescue Errno::ENOENT, ArgumentError
      # The file doesn't exist (ENOENT) or it's a binary file (ArgumentError).
      config
    end
  end

  def get_system_name
    return unless @os =~ /linux/
    return unless File.exist? "/etc/issue"

    data = IO.readlines("/etc/issue").first
    data =~ /([^ ]+)[^\d\.]*([\d\.]*)/

    name = $1.downcase
    version = $2

    if name == "debian" and File.exist? "/etc/debian_version"
      version = IO.read("/etc/debian_version").split.first.gsub(/\W/, "-")
    end

    return "#{name}-#{version}"
  end

  def check_tool_version(tool_name, opts, version, regexp=/(?=\d)(\d+).(\d+).?(\d+)?/)
    @log.print "Checking #{tool_name}:"

    output = `#{tool_name} #{opts}`

    if $?.exitstatus == 0
      v = output.scan(regexp)[0].map { |x| x.to_i }
      unless (v <=> version) >= 0
        failure " Expected #{tool_name} version >= #{version.join('.')}, found #{v.join('.')}"
      end
      @log.write " found"
    else
      failure "#{tool_name} not found."
    end
  end

  def detect_libc_name
    return if @libc

    @log.print "Checking for libc version: "

    case
    when @windows
      @libc = "msvcrt.dll"
    when @darwin
      @libc = "libc.dylib"
    else
      begin
        exe = ENV["SHELL"] || "/bin/sh"
        ldd_output = `ldd #{exe}`

        @libc = ldd_output[/libc\.so\.[0-9]+/]
      rescue
        # Don't abort if the command is not found
      end

      unless $?.success? and @libc
        failure "libc not found. Use the --libc configure option."
      end
    end

    @log.write "#{@libc} found!"
  end

  def write_configure_files
    @log.write "\nWriting configuration files..."

    @bin_links.delete @program_name

    config_settings = {
      :config_file        => @config,
      :command_line       => @command_line,
      :build_make         => @make,
      :build_rake         => @rake,
      :build_perl         => @perl,
      :llvm_path          => @llvm_path,
      :llvm_system_name   => @llvm_system_name,
      :llvm_configure     => @llvm_configure,
      :llvm_version       => @llvm_version,
      :llvm_api_version   => @llvm_api_version,
      :llvm_shared        => @llvm_shared,
      :llvm_shared_objs   => @llvm_shared_objs,
      :llvm_cxxflags      => @llvm_cxxflags,
      :llvm_ldflags       => @llvm_ldflags,
      :cc                 => @cc,
      :cxx                => @cxx,
      :make               => @make,
      :rake               => @rake,
      :tar                => @tar,
      :bzip               => @bzip,
      :perl               => @perl,
      :gem                => @gem,
      :ldshared           => @ldshared,
      :ldsharedxx         => @ldsharedxx,
      :gcc_major          => @gcc_major,
      :user_cflags        => "#{@user_cflags}",
      :user_cxxflags      => "#{@user_cxxflags}",
      :user_cppflags      => "#{@user_cppflags}",
      :user_incflags      => "#{@user_incflags}",
      :user_ldflags       => "#{@user_ldflags}",
      :system_cflags      => "#{@system_cflags}",
      :system_cxxflags    => "#{@system_cxxflags}",
      :system_cppflags    => "#{@system_cppflags}",
      :system_incflags    => "#{@system_incflags}",
      :system_ldflags     => "#{@system_ldflags}",
      :include_dirs       => @include_dirs,
      :lib_dirs           => @lib_dirs,
      :defines            => @defines,
      :curses             => @curses,
      :host               => @host,
      :cpu                => @cpu,
      :vendor             => @vendor,
      :os                 => @os,
      :little_endian      => @little_endian,
      :sizeof_short       => sizeof("short"),
      :sizeof_int         => sizeof("int"),
      :sizeof_void_ptr    => sizeof("void*"),
      :sizeof_intptr_t    => sizeof("intptr_t"),
      :sizeof_uintptr_t   => sizeof("uintptr_t"),
      :sizeof_size_t      => sizeof("size_t"),
      :sizeof_long        => sizeof("long"),
      :sizeof_long_long   => sizeof("long long"),
      :sizeof_float       => sizeof("float"),
      :sizeof_double      => sizeof("double"),
      :sizeof_off_t       => sizeof("off_t"),
      :sizeof_time_t      => sizeof("time_t"),
      :x86_64             => @x86_64,
      :aarch64            => @aarch64,
      :dtrace             => @dtrace,
      :dtrace_const       => @dtrace_const,
      :debug_build        => @debug_build,
      :sourcedir          => @sourcedir,
      :builddir           => @builddir,
      :scriptdir          => @scriptdir,
      :bootstrap_gems_dir => @bootstrap_gems_dir,
      :capi_includedir    => @capi_includedir,
      :build_libdir       => @build_libdir,
      :build_exe          => "#{@builddir}#{@bindir}/#{@program_name}",
      :build_bin          => @build_bin,
      :prefixdir          => @prefixdir,
      :bindir             => @bindir,
      :libdir             => @libdir,
      :encdir             => @encdir,
      :runtimedir         => @runtimedir,
      :codedbdir          => @codedbdir,
      :codetoolsdir       => @codetoolsdir,
      :stdlibdir          => @stdlibdir,
      :coredir            => @coredir,
      :sitedir            => @sitedir,
      :archdir            => @archdir,
      :vendordir          => @vendordir,
      :includedir         => @includedir,
      :mandir             => @mandir,
      :gemsdir            => @gemsdir,
      :gems_cache         => @gems_cache,
      :gems_list          => @gems_list,
      :gem_files          => @gem_files,
      :installed_gems     => @installed_gems,
      :program_name       => @program_name,
      :bin_links          => @bin_links,
      :use_bin_links      => @use_bin_links,
      :rpath              => @features["rpath"].value,
      :windows            => @windows,
      :darwin             => @darwin,
      :bsd                => @bsd,
      :linux              => @linux,
      :vendor_zlib        => @features["vendor-zlib"].value,
      :vendor_libsodium   => @features["vendor-libsodium"].value,
      :vm_release_h       => @vm_release_h,
      :ruby_version       => @ruby_version,
      :ruby_libversion    => @ruby_libversion,
    }

    write_config_rb @config, config_settings

    FileUtils.cp @config, "#{@sourcedir}/core/build_config.rb"

    # Write the config file used to build the C++ VM.
    Dir.mkdir "machine/gen" unless File.directory? "machine/gen"

    vm_paths_h = "machine/paths.h"
    File.open vm_paths_h, "wb" do |f|
      f.puts <<-EOF
#ifndef RBX_PATHS_H
#define RBX_PATHS_H

#define RBX_PREFIX_PATH      "#{@prefixdir}"
#define RBX_BIN_PATH         "#{@bindir}"
#define RBX_GEMS_PATH        "#{@gemsdir}"
#define RBX_RUNTIME_PATH     "#{@runtimedir}"
#define RBX_CODEDB_PATH      "#{@codedbdir}"
#define RBX_KERNEL_PATH      "#{@coredir}"
#define RBX_CORE_PATH        "#{@coredir}"
#define RBX_LIB_PATH         "#{@libdir}"
#define RBX_ENC_PATH         "#{@encdir}"
#define RBX_HDR_PATH         "#{@includedir}"
#define RBX_SITE_PATH        "#{@sitedir}"
#define RBX_ARCH_PATH        "#{@archdir}"
#define RBX_VENDOR_PATH      "#{@vendordir}"

#endif
      EOF
    end

    vm_config_h = "machine/config.h"
    File.open vm_config_h, "wb" do |f|
      f.puts <<-EOC
#ifndef RBX_CONFIG
#define RBX_CONFIG

#define RBX_PROGRAM_NAME     "#{@program_name}"
#define RBX_HOST             "#{@host}"
#define RBX_CPU              "#{@cpu}"
#define RBX_VENDOR           "#{@vendor}"
#define RBX_OS               "#{@os}"
#define RBX_RUBY_LIB_VERSION #{@ruby_libversion}
#define RBX_LDSHARED         "#{@ldshared}"
#define RBX_LDSHAREDXX       "#{@ldsharedxx}"
#define RBX_SIZEOF_LONG      #{sizeof("long")}
#define RBX_LLVM_API_VER     #{@llvm_api_version}
#define RBX_LIBC             "#{@libc}"
#define RBX_HAVE_LCHMOD      #{@have_lchmod}
#define RBX_HAVE_LCHOWN      #{@have_lchown}
#define RBX_HAVE_MKFIFO      #{@have_mkfifo}
#define RBX_DEBUG_BUILD      #{@debug_build.inspect}
      EOC

      if @llvm_version
        f.puts "#define RBX_LLVM_VERSION     #{@llvm_version.inspect}"
      end

      if @little_endian
        f.puts "#define RBX_LITTLE_ENDIAN    1"
      end

      if @tr1
        f.puts "#define RBX_HAVE_TR1         1"
      end

      if @tr1_hash
        f.puts "#define RBX_HAVE_TR1_HASH    1"
      end

      if @gc_stack_check
        f.puts "#define RBX_GC_STACK_CHECK   1"
      end

      if @log_concurrent_update
        f.puts "#define RBX_LOG_CONCURRENT_UPDATE 1"
      end

      if @raise_concurrent_update
        f.puts "#define RBX_RAISE_CONCURRENT_UPDATE 1"
      end

      [:windows, :darwin, :bsd, :linux].each do |platform|
        if instance_variable_get(:"@#{platform}")
          f.puts "#define RBX_#{platform.to_s.upcase}           1"
        end
      end

      f.puts "#define RBX_DTRACE_CONST     #{@dtrace_const ? "const" : ""}"

      write_have_defines f

      f.puts <<-EOC

#include "detection.hpp"

#define RBX_STRERROR_BUFSIZE 256

// strerror_r has different signatures on GNU and XSI.
// - The GNU version returns a pointer to a string, which may be the one passed
//   to the function as 'buf', or some immutable static string, in which case
//   'buf' is unused.
// - The XSI version always stores the error message in 'buf' and returns 0 on
//   success.
// This macro makes sure that the error message is returned either way.
#ifdef STRERROR_R_CHAR_P
#define RBX_STRERROR(errno, buf, size) strerror_r(errno, buf, size)
#else
#define RBX_STRERROR(errno, buf, size) (strerror_r(errno, buf, size), buf)
#endif

// Enable this define for some minimal GC debugging
// #define RBX_GC_DEBUG

// Enable for GC stress. This only ensures that the interrupts
// for a GC are set. Use RBX_GC_STRESS_YOUNG and / or RBX_GC_STRESS_MATURE
// to run either the young or mature gen on each possibility
// #define RBX_GC_STRESS

// When stress testing is enabled, forces a young collection every time it
// is possible. This can be useful to flush out bugs because of moving objects.
// #define RBX_GC_STRESS_YOUNG

// When stress testing is enabled, forces a mature collection every time it
// is possible. This can be useful to flush out bugs with reachability etc.
// #define RBX_GC_STRESS_MATURE

#endif
      EOC
    end

    # Write the config file for vendor/oniguruma.
    File.open "#{@build_libdir}/oniguruma/config.h", "wb" do |f|
      f.puts <<-EOC
/* This file is generated by the Rubinius build system. Your edits
 * will be lost. See the configure script.
 */
      EOC

      write_have_defines f
      write_have_sizeof_defines f
      write_sizeof_defines f
    end

    # Write release header file.
    write_release @vm_release_h

    # Write the rubinius-specific C-API config headers.
    vm_capi_header_gen = "#{@capi_includedir}/gen"
    FileUtils.mkdir_p vm_capi_header_gen
    FileUtils.cp vm_config_h, "#{vm_capi_header_gen}/rbx_config.h"
    FileUtils.cp @vm_release_h, "#{vm_capi_header_gen}/rbx_release.h"

    # Write the config file used in the C-API.
    capi_config_h = "#{@capi_includedir}/ruby/config.h"
    FileUtils.mkdir_p File.dirname(capi_config_h)
    File.open capi_config_h, "wb" do |f|
      f.puts <<-EOC
/* This file is generated by the build system. Your edits
 * will be lost. See the configure script.
 */

#ifndef NORETURN
#define NORETURN(x) __attribute__ ((noreturn)) x
#endif

#ifndef UNREACHABLE
#define UNREACHABLE __builtin_unreachable()
#endif

      EOC

      write_have_defines f
      write_have_sizeof_defines f
      write_sizeof_defines f

      if @windows
        f.puts "#define RBX_WINDOWS 1"
      end
    end
  end

  def print_debug
    puts "\nUsing the following configuration to build"
    puts "------------------------------------------"
    cat("build/config/config.rb")
    puts "\nSetting the following defines for the VM"
    puts "----------------------------------------"
    cat("machine/config.h")
  end

  def cat(file)
    puts IO.read(relative_file(file))
  end

  def relative_file(name)
    File.expand_path("../#{name}", __FILE__)
  end

  def check_force_clean
    unless verify_build_signature
      @log.write "\nDetected old configuration settings, forcing a clean build"
      system("#{build_ruby} -S #{@rake} clean")
    end
  end

  def fetch_gems
    @log.write "\nFetching gems..."
    failed = false

    Dir.chdir @gems_cache do
      @gem_files.each do |gem|
        next if File.exist? gem

        failed = true unless download "https://rubygems.org/gems/#{gem}", "./#{gem}"
      end
    end

    failure "Unable to download required gems." if failed
  end

  def verify_gems
    @log.write "\nVerifying gems..."

    failed = false

    @gem_files.each do |gem_name|
      unless File.file? "#{@gems_cache}/#{gem_name}"
        @log.write "unable to find gem #{gem_name}"
        failed = true
      end
    end

    failure "Unable to find required gems." if failed
  end

  def clean_gems(dir, gems)
    unpacked = Dir["#{dir}/*"]

    # Remove unpacked gems not specified by these configure settings
    unpacked.each do |dir|
      d = File.basename dir
      unless gems.find { |x| d =~ /^#{x}/ } and
             @gem_files.find { |x| d =~ /^#{x[0..-5]}/ }
        FileUtils.rm_rf dir
      end
    end
  end

  def unpack_gems(source, destination, list)
    FileUtils.mkdir_p destination unless File.directory? destination

    Dir.chdir destination do
      list.each do |name|
        gem_name = @gem_files.find { |x| x =~ /^#{name}/ }
        failure "Unable to find gem to unpack: #{name}" unless gem_name

        next if @installed_gems.include? gem_name

        unless File.directory? gem_name[0..-5]
          system("#{@gem} unpack #{source}/#{gem_name}")

          unless $?.exitstatus == 0
            failure "Unable to unpack bootstrap gems."
          end
        end
      end
    end
  end

  def setup_gems
    @log.write "\nSetting up gems..."

    # Remove unpacked gems not specified by these configure settings
    clean_gems @bootstrap_gems_dir, @gem_names

    # Unpack gems not found for these configure settings
    unpack_gems @gems_cache, @bootstrap_gems_dir, @gem_files
  end

  def setup_codedb
    @log.write "\nSetting up CodeDB..."

    dir = "#{@builddir}#{@codedbdir}"
    codedb_cache = "#{dir}/cache"

    unless File.file? codedb_cache
      url = "https://rubinius-binaries-rubinius-com.s3.amazonaws.com/codedb/"
      cache = "rubinius-codedb-cache"
      cache_bzip = "#{cache}.bz2"
      cache_digest = "#{cache_bzip}.sha512"

      unless File.file? cache_bzip
        download "#{url}#{cache_bzip}", cache_bzip
      end

      unless File.file? cache_digest
        download "#{url}#{cache_digest}", cache_digest
      end

      if Digest::SHA512.file(cache_bzip).hexdigest !=
          File.read(cache_digest).strip.split(" ").first
        failure "CodeDB cache SHA does not match"
      end
    end
  end

  def setup_stdlib
    @log.write "\nSetting up stdlib..."

    stdlib_cache = "rubinius-stdlib-cache"
    cache_bzip = "#{stdlib_cache}.bz2"
    cache_digest = "#{cache_bzip}.sha512"

    unless File.file? cache_bzip
      url = "https://rubinius-binaries-rubinius-com.s3.amazonaws.com/stdlib/"

      unless File.file? cache_bzip
        download "#{url}#{cache_bzip}", cache_bzip
      end

      unless File.file? cache_digest
        download "#{url}#{cache_digest}", cache_digest
      end

      if Digest::SHA512.file(cache_bzip).hexdigest !=
          File.read(cache_digest).strip.split(" ").first
        failure "Stdlib cache SHA does not match"
      end
    end
  end

  def setup_codetools
    @log.write "\nSetting up codetools..."

    codetools_cache = "rubinius-codetools-cache"
    cache_bzip = "#{codetools_cache}.bz2"
    cache_digest = "#{cache_bzip}.sha512"

    unless File.file? cache_bzip
      url = "https://rubinius-binaries-rubinius-com.s3.amazonaws.com/codetools/"

      unless File.file? cache_bzip
        download "#{url}#{cache_bzip}", cache_bzip
      end

      unless File.file? cache_digest
        download "#{url}#{cache_digest}", cache_digest
      end

      if Digest::SHA512.file(cache_bzip).hexdigest !=
          File.read(cache_digest).strip.split(" ").first
        failure "Codetools cache SHA does not match"
      end
    end
  end

  # Create directories that don't have to be created by the end user
  # themselves.
  def create_directories
    FileUtils.mkdir_p @gems_cache
  end

  def run
    unless ENV["RBX_SUPRESS_DEPRECATION"]
      @log.deprecated "\n\n'configure' is deprecated and will be removed in the future.\n" \
        "Use 'build.sh' to configure, build, package, and install Rubinius.\n\n\n"
    end

    options
    set_host
    parse ARGV
    detect_homebrew_openssl_lib
    create_directories
    check_tools
    check_force_clean

    set_filesystem_paths

    process

    unless sizeof("long") == 8
      failure "Support for non-64bit platforms was deprecated 1 Jun 2016 and has now been removed. If non-64bit support is a critical feature for your application, please email contact@rubinius.com"
    end

    if @release_build
      verify_gems
    else
      fetch_gems
    end
    setup_gems
    setup_codedb
    setup_codetools
    setup_stdlib
    write_configure_files
    write_build_signature

    return if @release_config

    print_debug if @verbose

    if @llvm_source_build
      files = prebuilt_files.map { |f| File.basename f, ".tar.bz2" }.join("\n  ")

      @log.write <<-EOM

------------------------------------------------------------------
Unable to find an existing binary build of LLVM for your platform.

Please notify the Rubinius team at the #rubinius channel on
irc.freenode.net and provide the following system information:

  prebuilts:

  #{files}
------------------------------------------------------------------
      EOM
    end

    unless @builddir
      build_msg = <<-EOM
Rubinius (#{release_revision.last[0, 8]}) has been configured.

Run 'rake' to build and test Rubinius.
      EOM
    else
      build_msg = <<-EOM
Rubinius (#{release_revision.last[0, 8]}) has been configured for the following paths:

prefix:     #{@prefixdir}
bin:        #{@prefixdir}#{@bindir}
lib:        #{@prefixdir}#{@libdir}
include:    #{@prefixdir}#{@includedir}
codedb:     #{@prefixdir}#{@codedbdir}
site:       #{@prefixdir}#{@sitedir}
arch:       #{@prefixdir}#{@archdir}
vendor:     #{@prefixdir}#{@vendordir}
man:        #{@prefixdir}#{@mandir}
gems:       #{@prefixdir}#{@gemsdir}
gems cache: #{@gems_cache}

Run 'rake' to build, test and install Rubinius.
      EOM
    end

    links = (@bin_links + [@program_name]).uniq

    @log.write <<-EOM
------------------------------------------------------------------

#{build_msg}
After building, you may add

'#{@prefixdir}#{@bindir}'

to your PATH or run commands directly from that directory.

Available commands are:

  #{links.join(", ")}

------------------------------------------------------------------
    EOM
  end


  # Configuration item that has both a default and a configured value
  class ConfigurationToggle
    attr_reader :default, :configured

    def initialize(default_value)
      @default = !!default_value
      @configured = nil
    end

    def configured=(value)
      @configured = !!value
    end

    def value
      unless @configured.nil?
        @configured
      else
        @default
      end
    end
  end

  # Handles user output and logging while running configure.
  class Logger
    attr_reader :path

    # Creates an instance of Logger writing to +file+.
    def initialize(file, init=true)
      @path = File.expand_path("../#{file}", __FILE__)
      if init
        File.open(@path, "wb") { }
        log "Configuring Rubinius..."
      end
    end

    # Copies the contents of +other+ into this logger's file.
    def replace(other)
      output do |f|
        f.puts File.read(other)
      end
    end

    # Writes +message+ to the logging file but not to the screen.
    def log(message, error=false)
      output do |f|
        stamp = "#{timestamp}#{'*** ERROR' if error}"
        if multiline?(message)
          f.puts "#{stamp} ---"
          f.puts message
          f.puts "---"
        else
          f.puts "#{stamp} #{message}"
        end
      end
    end

    # Writes a normal message to STDOUT and logs to the file.
    def write(message)
      log message
      STDOUT.puts message
    end

    # Writes a normal message to STDOUT with #print and logs to file.
    def print(message)
      log message
      STDOUT.print message
    end

    # Writes an error message to STDERR and logs to the file with
    # error decorations. This should only be used for errors that
    # affect configure itself.
    def error(message)
      log message, true
      STDERR.puts message
    end

    DEPRECATION_HEADER =
      "------------------------------ Deprecation notice ------------------------------"
    DEPRECATION_FOOTER =
      "--------------------------------------------------------------------------------"

    def deprecated(message)
      log DEPRECATION_HEADER, true
      log message, true
      log DEPRECATION_FOOTER, true

      STDERR.puts DEPRECATION_HEADER
      STDERR.puts message
      STDERR.puts DEPRECATION_FOOTER
    end

    # Yields an IO for writing log messages.
    def output
      File.open @path, "a" do |f|
        yield f
      end
    end

    # Returns a formatted times suitable for logging.
    def timestamp
      Time.now.strftime "[%Y-%m-%d %H:%M:%S]"
    end

    # Returns true if the message has more than one line.
    def multiline?(message)
      message.index("\n") != nil
    end
  end

  # Returns true if the *port* command is in the PATH and identifies
  # itself with "MacPorts" when run interactively.
  def macports?
    `echo quit | port 2>&-`.start_with? 'MacPorts'
  end

  # Query MacPorts for the path to the latest installed version of
  # llvm-config that is within the range of supported LLVM versions.
  def macports_llvm_config
    supported_versions = (3.6..3.9)
    installed_ports    = `port installed | egrep -o 'llvm-[^ ]+'`.split
    latest_usable_port = installed_ports.sort.select do |fname|
                           version = fname.match(/-\K.*/)[0].to_f
                           supported_versions.include? version
                         end.last
    avail_binaries     = `port contents #{latest_usable_port} |
                          fgrep llvm-config`.split
    avail_binaries.reject { |fname| fname.include? 'libexec' }.last
  end

  def brew(args)
    `brew #{args}`.chomp
  end

  # Returns true if the *brew* command is in the PATH and identifies
  # itself with "Homebrew" when run interactively with -v argument.
  def homebrew?
    brew("-v 2>&1").start_with? 'Homebrew'
  end

  # Check if latest version of openssl is installed; if so, add to include
  # and libs
  def detect_homebrew_openssl_lib
    if @darwin && homebrew? && (brew("list").split("\n").include? "openssl")
      prefix = brew("--prefix openssl")

      ENV["OPENSSL_DIR"] = prefix unless ENV["OPENSSL_DIR"]
      add_opt_dir prefix
    end
  end

end

STDOUT.sync = true
Configure.new(root).run
