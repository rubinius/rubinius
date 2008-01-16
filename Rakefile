# NOTE! When updating this file, also update INSTALL, if necessary.

$VERBOSE = true
$verbose = Rake.application.options.trace
$dlext = Config::CONFIG["DLEXT"]
$redcloth_available = nil
$compiler = nil

require 'tsort'
require 'rakelib/struct_generator'
require 'rakelib/const_generator'

begin
  require 'rubygems'
rescue LoadError
  # Don't show RedCloth warning if gems aren't available
  $redcloth_available = false
end

task :default => :build

def make(args = nil)
  if RUBY_PLATFORM =~ /bsd/
    gmake = 'gmake'
  else
    gmake = 'make'
  end
  "#{ENV['MAKE'] || gmake} #{args}"
end

class Hash
  include TSort

  # This keeps things consistent across all platforms
  def tsort_each_node(&block)
    keys.sort.each(&block)
  end

  def tsort_each_child(node, &block)
    fetch(node).each(&block)
  end
end

def newer?(file, cmp)
  File.exists?(cmp) and File.mtime(cmp) >= File.mtime(file)
end

def source_name(compiled)
  File.basename(compiled, '.*') + '.rb'
end

def compiled_name(source, dir)
  File.join(dir, File.basename(source, '.*') + '.rbc')
end

# Some files have load order dependencies. To specify a load order
# dependency, include a comment in the file that has the dependency.
# For example, assume files a.rb and b.rb, where a.rb requires that
# b.rb is loaded first. In a.rb, include a comment
#   # depends on: b.rb
#
# The 'depends on:' declaration takes a space separated list of file.
# When the '.load_order.txt' file is created, a topological sort
# (see name caveat in TSort) of the dependencies is performed
# so files that are depended on are loaded first.
#
# If there is a 'depends on:' declarations for a non-existent file,
# or if there are cyclic dependencies, this method will not create
# the '.load_order.txt' file.

def create_load_order(files, output=".load_order.txt")
  d = Hash.new { |h,k| h[k] = [] }

  # assume all the files are in the same directory
  dir = File.dirname(files.first)
  found = false
  files.each do |fname|
    name = source_name(fname)
    # Force every entry to be in the hash
    d[name]
    File.open(File.join(dir, name), "r") do |f|
      f.each do |line|
        if m = /#\s*depends on:\s*(.*)/.match(line)
          found = true
          m[1].split.each { |dep| d[name] << dep }
        end
      end
    end
  end

  puts "Generating #{output}..."

  File.open(output, "w") do |f|
    begin
      if found
        list = d.tsort
      else
        list = files.sort
      end

      f.puts list.collect { |n| compiled_name(n, dir) }.join("\n")
    rescue IndexError => e
      puts "Unable to generate '.load_order.txt'"
      puts "Most likely, a file includes a 'depends on:' declaration for a non-existent file"
      raise e
    rescue TSort::Cyclic => e
      puts "Unable to generate '.load_order.txt' due to a cyclic dependency\n  (#{e.message})"
      raise e
    end
  end
end

def compile(name, output=nil, check_mtime=false)
  if output
    dir = File.dirname(output)

    unless File.exists?(dir)
      FileUtils.mkdir_p dir
    end

    if check_mtime and File.exists?(output) and File.mtime(output) > File.mtime(name)
      return
    end
  end
  
  inc = "-Iruntime/stable/compiler.rba -rcompiler/init"

  if ENV['GDB']
    sh "shotgun/rubinius --gdb #{inc} compile #{name} #{output}", :verbose => $verbose
  else
    sh "shotgun/rubinius #{inc} compile #{name} #{output}", :verbose => $verbose
  end
end

def compile_dir(dir)
  (Dir["#{dir}/*.rb"] + Dir["#{dir}/**/*.rb"]).each do |file|
    compile file, "#{file}c", true
  end
end

task :stable_compiler do
  if ENV['USE_CURRENT']
    puts "Use current versions, not stable."
  else
    ENV['RBX_BOOTSTRAP'] = "runtime/stable/bootstrap.rba"
    ENV['RBX_CORE'] = "runtime/stable/core.rba"
    ENV['RBX_LOADER'] = "runtime/stable/loader.rbc"
    ENV['RBX_PLATFORM'] = "runtime/stable/platform.rba"
  end
end

task :stable_shell => :stable_compiler do
  sh "shotgun/rubinius --gdb"
end

rule ".rbc" => %w[.rb] do |t|
  compile t.source, t.name
end

class CodeGroup

  def initialize(files, compile_dir, rba_name, load_order=true)
    if files.is_a?(FileList)
      @files = files
    else
      @files = FileList[files]
    end

    @output = nil
    @compile_dir = compile_dir
    @build_dir = File.join 'runtime', rba_name
    @rba_name = "#{rba_name}.rba"

    if load_order
      @load_order = File.join @compile_dir, '.load_order.txt'
    else
      @load_order = nil
    end

    @output = []

    make_tasks
  end

  attr_reader :output

  def clean
    sh "find #{@compile_dir} -name '*.rbc' -delete"
  end

  def compile_task
    @files.each do |source|
      runtime = File.join(@compile_dir, source.ext("rbc"))

      @output << runtime

      deps = [source].compact

      file runtime => deps do |t|
        compile t.prerequisites.first, t.name
      end
    end
  end

  def load_order_task
    return unless @load_order

    file @load_order => @files do
      create_load_order(@files, @load_order)
    end
    task "build:load_order" => @files do
      create_load_order(@files, @load_order)
    end

    @output << @load_order
  end

  def make_tasks
    Dir.mkdir @compile_dir unless File.exists? @compile_dir

    compile_task
    load_order_task
    rba_task

    @output
  end

  def rba_task
    file File.join('runtime', 'stable', @rba_name) => @output do
      files = @output.map do |path|
        path.sub File.join(@build_dir, ''), ''
      end

      Dir.chdir @build_dir do
        zip_name = File.join '..', 'stable', @rba_name
        rm_f zip_name, :verbose => $verbose
        sh "zip #{zip_name} #{files.join ' '}", :verbose => $verbose
      end
    end
  end

end

files = FileList['kernel/core/*.rb']

unless files.include?("kernel/core/dir.rb")
  files.add("kernel/core/dir.rb")
end

# make the rebuild less painful.
# this line should be removed in a week or so.
files.exclude("kernel/core/dir_entry.rb")

Core      = CodeGroup.new(files, 'runtime/core', 'core')

Bootstrap = CodeGroup.new 'kernel/bootstrap/*.rb', 'runtime/bootstrap',
                          'bootstrap'
PlatformFiles  = CodeGroup.new 'kernel/platform/*.rb', 'runtime/platform', 'platform'

file 'runtime/loader.rbc' => 'kernel/loader.rb' do
  compile 'kernel/loader.rb', 'runtime/loader.rbc'
end

file 'runtime/stable/loader.rbc' => 'runtime/loader.rbc' do
  cp 'runtime/loader.rbc', 'runtime/stable', :verbose => $verbose
end

file 'runtime/stable/compiler.rba' => 'build:compiler' do
  sh "cd lib; zip -r ../runtime/stable/compiler.rba compiler -x \\*.rb"
end

Rake::StructGeneratorTask.new do |t|
  t.dest = "lib/etc.rb"
end

Rake::StructGeneratorTask.new do |t|
  t.dest = 'lib/zlib.rb'
end

AllPreCompiled = Core.output + Bootstrap.output + PlatformFiles.output
AllPreCompiled << "runtime/loader.rbc"

# spec tasks
desc "Run all 'known good' specs (task alias for spec:ci)"
task :spec => 'spec:ci'

namespace :spec do
  namespace :setup do
    # Setup for 'Subtend' specs. No need to call this yourself.
    task :subtend do
      Dir["spec/subtend/**/Rakefile"].each do |rakefile|
        sh "rake -f #{rakefile}"
      end
    end
  end

  desc "Run continuous integration examples"
  task :ci do
    target = ENV['SPEC_TARGET'] || 'rbx'
    system %(shotgun/rubinius -e 'puts "rbx build: \#{Rubinius::BUILDREV}"') if target == 'rbx'
    sh "bin/ci -t #{target}"
  end

  spec_targets = %w(compiler core language library parser rubinius)
  # Build a spec:<task_name> for each group of Rubinius specs
  spec_targets.each do |group|
    desc "Run #{group} examples"
    task group do
      sh "bin/mspec spec/#{group}"
    end
  end

  desc "Run subtend (Rubinius C API) examples"
  task :subtend => "spec:setup:subtend" do
    sh "bin/mspec spec/rubinius/subtend"
  end

  # Specdiffs to make it easier to see what your changes have affected :)
  desc 'Run specs and produce a diff against current base'
  task :diff => 'diff:run'

  namespace :diff do
    desc 'Run specs and produce a diff against current base'
    task :run do
      system 'bin/mspec -f ci -o spec/reports/specdiff.txt spec'
      system 'diff -u spec/reports/base.txt spec/reports/specdiff.txt'
      system 'rm spec/reports/specdiff.txt'
    end

    desc 'Replace the base spec file with a new one'
    task :replace do
      system 'bin/mspec -f ci -o spec/reports/base.txt spec'
    end
  end

  task :r2r do
    puts ARGV.inspect
  end
end

desc "Build everything that needs to be built"
task :build => 'build:all'

def install_files(files, destination)
  files.sort.each do |path|
    next if File.directory? path

    file = path.sub %r%^(runtime|lib)/%, ''
    dest_file = File.join destination, file
    dest_dir = File.dirname dest_file
    mkdir_p dest_dir unless File.directory? dest_dir

    install path, dest_file, :mode => 0644, :verbose => true
  end
end

desc "Install rubinius as rbx"
task :install => :config_env do
  sh "cd shotgun; #{make "install"}"

  mkdir_p ENV['RBAPATH'], :verbose => true
  mkdir_p ENV['CODEPATH'], :verbose => true

  rba_files = Rake::FileList.new('runtime/platform.conf',
                                 'runtime/**/*.rb{a,c}',
                                 'runtime/**/.load_order.txt')

  install_files rba_files, ENV['RBAPATH']

  lib_files = Rake::FileList.new 'lib/**/*'

  install_files lib_files, ENV['CODEPATH']

  mkdir_p File.join(ENV['CODEPATH'], 'bin'), :verbose => true

  Rake::FileList.new("#{ENV['CODEPATH']}/**/*.rb").sort.each do |rb_file|
    sh File.join(ENV['BINPATH'], 'rbx'), 'compile', rb_file, :verbose => true
  end
end


desc "Uninstall rubinius and libraries. Helps with build problems."
task :uninstall => :config_env do
  rm Dir[File.join(ENV['BINPATH'], 'rbx*')]
  rm_r Dir[File.join(ENV['LIBPATH'], '*rubinius*')]
end

task :config_env => 'shotgun/config.mk' do
  File.foreach 'shotgun/config.mk' do |line|
    next unless line =~ /(.*?)=(.*)/
    ENV[$1] = $2
  end
end

task :compiledir => :stable_compiler do
  dir = ENV['DIR']
  raise "Use DIR= to set which directory" if !dir or dir.empty?
  compile_dir(dir)
end

desc "Recompile all ruby system files"
task :rebuild => %w[clean:rbc clean:extensions clean:shotgun build:all]

task :clean => %w[clean:rbc clean:extensions clean:shotgun]

desc "Remove all ruby system files"
task :distclean => %w[pristine clean:rbc clean:extensions clean:shotgun clean:external]

desc "Remove all stray compiled Ruby files"
task :pristine do
  FileList['**/*.rbc'].each do |fn|
    next if /^runtime/.match(fn)
    next if %r!fixtures/require!.match(fn)
    next if %r!lib/compiler!.match(fn)
    FileUtils.rm fn rescue nil
  end
end

namespace :clean do

  desc "Remove all compile system ruby files (runtime/)"
  task :rbc do
    AllPreCompiled.each do |f|
      rm_f f, :verbose => $verbose
    end

    (Dir["lib/compiler/*.rbc"] + Dir["lib/compiler/**/*.rbc"]).each do |f|
      rm_f f, :verbose => $verbose
    end

    rm_f "runtime/platform.conf"
  end

  desc "Cleans all compiled extension files (lib/ext)"
  task :extensions do
    Dir["lib/ext/**/*#{$dlext}"].each do |f|
      rm_f f, :verbose => $verbose
    end
  end

  desc "Cleans up VM building site"
  task :shotgun do
    sh make('clean')
  end

  desc "Cleans up VM and external libs"
  task :external do
    sh "cd shotgun; #{make('distclean')}"
  end
end

namespace :build do

  task :all => %w[
    build:shotgun
    build:platform
    build:rbc
    compiler
    lib/etc.rb
    lib/rbconfig.rb
    extensions
  ]

  # This nobody rule lets use use all the shotgun files as
  # prereqs. This rule is run for all those prereqs and just
  # (obviously) does nothing, but it makes rake happy.
  rule '^shotgun/.+'

  c_source = FileList[
    "shotgun/config.h",
    "shotgun/lib/*.[chy]",
    "shotgun/lib/*.rb",
    "shotgun/lib/subtend/*.[chS]",
    "shotgun/main.c",
  ].exclude(/auto/, /instruction_names/, /node_types/)

  file "shotgun/rubinius.bin" => c_source do
    sh make('vm')
  end

  file 'shotgun/mkconfig.sh' => 'configure'
  file 'shotgun/config.mk' => %w[shotgun/config.h shotgun/mkconfig.sh shotgun/vars.mk]
  file 'shotgun/config.h' => %w[shotgun/mkconfig.sh shotgun/vars.mk] do
    sh "./configure"
    raise 'Failed to configure Rubinius' unless $?.success?
  end

  desc "Compiles shotgun (the C-code VM)"
  task :shotgun => %w[configure shotgun/rubinius.bin]

  task :setup_rbc => :stable_compiler

  task :rbc => ([:setup_rbc] + AllPreCompiled)
  
  task :compiler => :stable_compiler do
    compile_dir "lib/compiler"
  end

  desc "Rebuild runtime/stable/*.  If you don't know why you're running this, don't."
  task :stable => %w[
    build:all
    runtime/stable/bootstrap.rba
    runtime/stable/core.rba
    runtime/stable/compiler.rba
    runtime/stable/loader.rbc
    runtime/stable/platform.rba
  ]

  file 'lib/rbconfig.rb' => %w[config_env Rakefile] do
    rbconfig = <<-EOF
#--
# This file was generated by the rubinius Rakefile.
#++

module Config

  CONFIG = {}

  CONFIG['DLEXT'] = Rubinius::LIBSUFFIX.dup
  CONFIG['EXEEXT'] = ""
  CONFIG['RUBY_SO_NAME'] = "rubinius-#\{Rubinius::RBX_VERSION}"
  CONFIG['arch'] = RUBY_PLATFORM.dup
  CONFIG['bindir'] = "#{ENV['BINPATH']}"
  CONFIG['datadir'] = "#{File.join ENV['PREFIX'], 'share'}"
  CONFIG['libdir'] = "#{ENV['LIBPATH']}"
  CONFIG['ruby_install_name'] = "#{ENV['ENGINE']}"
  CONFIG['ruby_version'] = Rubinius::RUBY_VERSION.dup
  CONFIG['sitedir'] = "#{File.join ENV['LIBPATH'], 'rubinius'}"
  CONFIG['sitelibdir'] = "#{ENV['CODEPATH']}"

end

RbConfig = Config
    EOF

    File.open 'lib/rbconfig.rb', 'w' do |fp|
      fp.write rbconfig
    end
  end

  desc "Rebuild the .load_order.txt files"
  task "load_order" do
    # Note: Steps to rebuild load_order were defined above
  end

  namespace :vm do
    task "clean" do
      sh "cd shotgun/lib; make clean"
    end

    task "dev" do
      sh "cd shotgun/lib; make DEV=1"
    end
  end

  task :platform => 'runtime/platform.conf'
end

file 'runtime/platform.conf' => %w[Rakefile rakelib/struct_generator.rb] do |task|
  addrinfo = StructGenerator.new
  addrinfo.include 'sys/socket.h'
  addrinfo.include 'netdb.h'
  addrinfo.name 'struct addrinfo'
  addrinfo.field :ai_flags, :int
  addrinfo.field :ai_family, :int
  addrinfo.field :ai_socktype, :int
  addrinfo.field :ai_protocol, :int
  addrinfo.field :ai_addrlen, :int
  addrinfo.field :ai_addr, :pointer
  addrinfo.field :ai_canonname, :string
  addrinfo.field :ai_next, :pointer
  addrinfo.calculate

  dirent = StructGenerator.new
  dirent.include "dirent.h"
  dirent.name 'struct dirent'
  dirent_d_name = dirent.field :d_name
  dirent.calculate

  timeval = StructGenerator.new
  timeval.include "sys/time.h"
  timeval.name 'struct timeval'
  timeval.field :tv_sec
  timeval.field :tv_usec
  timeval.calculate

  sockaddr_in = StructGenerator.new
  sockaddr_in.include "netinet/in.h"
  sockaddr_in.include "fcntl.h"
  sockaddr_in.include "sys/socket.h"
  sockaddr_in.include "sys/stat.h"
  sockaddr_in.name 'struct sockaddr_in'
  sockaddr_in.field :sin_family
  sockaddr_in.field :sin_port
  sockaddr_in.field :sin_addr
  sockaddr_in.field :sin_zero
  sockaddr_in.calculate

  sockaddr_un = StructGenerator.new
  sockaddr_un.include "sys/un.h"
  sockaddr_un.name 'struct sockaddr_un'
  sockaddr_un.field :sun_family
  sockaddr_un.field :sun_path
  sockaddr_un.calculate

  servent = StructGenerator.new
  servent.include "netdb.h"
  servent.name 'struct servent'
  servent.field :s_name, :pointer
  servent.field :s_aliases, :pointer
  servent.field :s_port, :int
  servent.field :s_proto, :pointer
  servent.calculate

  # FIXME these constants don't have standard names.
  # LOCK_SH == Linux, O_SHLOCK on Bsd/Darwin, etc.
  # Binary doesn't exist at all in many non-Unix variants.
  # This should come out of something like config.h
  fixme_constants = %w{
    LOCK_SH
    LOCK_EX
    LOCK_NB
    LOCK_UN
    BINARY  
  }

  file_constants = %w{
    O_RDONLY
    O_WRONLY
    O_RDWR
    O_CREAT
    O_EXCL
    O_NOCTTY
    O_TRUNC
    O_APPEND
    O_NONBLOCK
    O_SYNC
    S_IRUSR
    S_IWUSR
    S_IXUSR
    S_IRGRP
    S_IWGRP
    S_IXGRP
    S_IROTH
    S_IWOTH
    S_IXOTH
  }

  io_constants = %w{
    SEEK_SET
    SEEK_CUR
    SEEK_END
  }

  socket_constants = %w[
    AF_APPLETALK
    AF_ATM
    AF_AX25
    AF_CCITT
    AF_CHAOS
    AF_CNT
    AF_COIP
    AF_DATAKIT
    AF_DEC
    AF_DLI
    AF_E164
    AF_ECMA
    AF_HYLINK
    AF_IMPLINK
    AF_INET
    AF_INET6
    AF_IPX
    AF_ISDN
    AF_ISO
    AF_LAT
    AF_LINK
    AF_LOCAL
    AF_MAX
    AF_NATM
    AF_NDRV
    AF_NETBIOS
    AF_NETGRAPH
    AF_NS
    AF_OSI
    AF_PPP
    AF_PUP
    AF_ROUTE
    AF_SIP
    AF_SNA
    AF_SYSTEM
    AF_UNIX
    AF_UNSPEC

    AI_ADDRCONFIG
    AI_ALL
    AI_CANONNAME
    AI_DEFAULT
    AI_MASK
    AI_NUMERICHOST
    AI_PASSIVE
    AI_V4MAPPED
    AI_V4MAPPED_CFG

    EAI_ADDRFAMILY
    EAI_AGAIN
    EAI_BADFLAGS
    EAI_BADHINTS
    EAI_FAIL
    EAI_FAMILY
    EAI_MAX
    EAI_MEMORY
    EAI_NODATA
    EAI_NONAME
    EAI_PROTOCOL
    EAI_SERVICE
    EAI_SOCKTYPE
    EAI_SYSTEM

    INADDR_ALLHOSTS_GROUP
    INADDR_ANY
    INADDR_BROADCAST
    INADDR_LOOPBACK
    INADDR_MAX_LOCAL_GROUP
    INADDR_NONE
    INADDR_UNSPEC_GROUP

    IPPORT_RESERVED
    IPPORT_USERRESERVED

    IPPROTO_BIP
    IPPROTO_EGP
    IPPROTO_EON
    IPPROTO_GGP
    IPPROTO_HELLO
    IPPROTO_ICMP
    IPPROTO_IDP
    IPPROTO_IGMP
    IPPROTO_IP
    IPPROTO_MAX
    IPPROTO_ND
    IPPROTO_PUP
    IPPROTO_RAW
    IPPROTO_TCP
    IPPROTO_TP
    IPPROTO_UDP
    IPPROTO_XTP

    IPX_TYPE

    IP_ADD_MEMBERSHIP
    IP_DEFAULT_MULTICAST_LOOP
    IP_DEFAULT_MULTICAST_TTL
    IP_DROP_MEMBERSHIP
    IP_HDRINCL
    IP_MAX_MEMBERSHIPS
    IP_MULTICAST_IF
    IP_MULTICAST_LOOP
    IP_MULTICAST_TTL
    IP_OPTIONS
    IP_RECVDSTADDR
    IP_RECVOPTS
    IP_RECVRETOPTS
    IP_RETOPTS
    IP_TOS
    IP_TTL

    MSG_COMPAT
    MSG_CTRUNC
    MSG_DONTROUTE
    MSG_DONTWAIT
    MSG_EOF
    MSG_EOR
    MSG_FLUSH
    MSG_HAVEMORE
    MSG_HOLD
    MSG_OOB
    MSG_PEEK
    MSG_RCVMORE
    MSG_SEND
    MSG_TRUNC
    MSG_WAITALL

    NI_DGRAM
    NI_MAXHOST
    NI_MAXSERV
    NI_NAMEREQD
    NI_NOFQDN
    NI_NUMERICHOST
    NI_NUMERICSERV

    PF_APPLETALK
    PF_AX25
    PF_CCITT
    PF_CHAOS
    PF_CNT
    PF_COIP
    PF_DATAKIT
    PF_DLI
    PF_ECMA
    PF_HYLINK
    PF_IMPLINK
    PF_INET
    PF_INET6
    PF_IPX
    PF_ISDN
    PF_ISO
    PF_KEY
    PF_LAT
    PF_LINK
    PF_LOCAL
    PF_MAX
    PF_NATM
    PF_NDRV
    PF_NETBIOS
    PF_NETGRAPH
    PF_NS
    PF_OSI
    PF_PIP
    PF_PPP
    PF_PUP
    PF_ROUTE
    PF_RTIP
    PF_SIP
    PF_SNA
    PF_SYSTEM
    PF_UNIX
    PF_UNSPEC
    PF_XTP

    SHUT_RD
    SHUT_RDWR
    SHUT_WR

    SOCK_DGRAM
    SOCK_PACKET
    SOCK_RAW
    SOCK_RDM
    SOCK_SEQPACKET
    SOCK_STREAM

    SOL_ATALK
    SOL_AX25
    SOL_IP
    SOL_IPX
    SOL_SOCKET
    SOL_TCP
    SOL_UDP

    SOPRI_BACKGROUND
    SOPRI_INTERACTIVE
    SOPRI_NORMAL

    SO_ACCEPTCONN
    SO_ACCEPTFILTER
    SO_ATTACH_FILTER
    SO_BINDTODEVICE
    SO_BROADCAST
    SO_DEBUG
    SO_DETACH_FILTER
    SO_DONTROUTE
    SO_DONTTRUNC
    SO_ERROR
    SO_KEEPALIVE
    SO_LINGER
    SO_NKE
    SO_NOSIGPIPE
    SO_NO_CHECK
    SO_NREAD
    SO_OOBINLINE
    SO_PASSCRED
    SO_PEERCRED
    SO_PEERNAME
    SO_PRIORITY
    SO_RCVBUF
    SO_RCVLOWAT
    SO_RCVTIMEO
    SO_REUSEADDR
    SO_REUSEPORT
    SO_SECURITY_AUTHENTICATION
    SO_SECURITY_ENCRYPTION_NETWORK
    SO_SECURITY_ENCRYPTION_TRANSPORT
    SO_SNDBUF
    SO_SNDLOWAT
    SO_SNDTIMEO
    SO_TIMESTAMP
    SO_TYPE
    SO_USELOOPBACK
    SO_WANTMORE
    SO_WANTOOBFLAG

    TCP_MAXSEG
    TCP_NODELAY
  ]

  process_constants = %w{
    WNOHANG
    WUNTRACED
    PRIO_PROCESS
    PRIO_PGRP
    PRIO_USER
    RLIMIT_CPU
    RLIMIT_FSIZE
    RLIMIT_DATA
    RLIMIT_STACK
    RLIMIT_CORE
    RLIMIT_RSS
    RLIMIT_NPROC
    RLIMIT_NOFILE
    RLIMIT_MEMLOCK
    RLIMIT_AS
    RLIMIT_SBSIZE
  }

  long_process_constants = %w{
    RLIM_INFINITY
    RLIM_SAVED_MAX
    RLIM_SAVED_CUR
  }

  # The constants come from MRI's signal.c. This means that some of them might
  # be missing.
  signal_constants = %w{
    SIGHUP
    SIGINT
    SIGQUIT
    SIGILL
    SIGTRAP
    SIGIOT
    SIGABRT
    SIGEMT
    SIGFPE
    SIGKILL
    SIGBUS
    SIGSEGV
    SIGSYS
    SIGPIPE
    SIGALRM
    SIGTERM
    SIGURG
    SIGSTOP
    SIGTSTP
    SIGCONT
    SIGCHLD
    SIGCLD
    SIGCHLD
    SIGTTIN
    SIGTTOU
    SIGIO
    SIGXCPU
    SIGXFSZ
    SIGVTALRM
    SIGPROF
    SIGWINCH
    SIGUSR1
    SIGUSR2
    SIGLOST
    SIGMSG
    SIGPWR
    SIGPOLL
    SIGDANGER
    SIGMIGRATE
    SIGPRE
    SIGGRANT
    SIGRETRACT
    SIGSOUND
    SIGINFO
  }
  
  cg = ConstGenerator.new
  cg.include "stdio.h"
  cg.include "fcntl.h"
  cg.include "sys/types.h"
  cg.include "sys/socket.h"
  cg.include "netdb.h"
  cg.include "sys/stat.h"
  cg.include "sys/resource.h"
  cg.include "netinet/tcp.h"
  cg.include "signal.h"

  file_constants.each { |c| cg.const c }
  io_constants.each { |c| cg.const c }
  socket_constants.each { |c| cg.const c }
  process_constants.each { |c| cg.const c }
  long_process_constants.each { |c| cg.const(c, "%llu") }
  signal_constants.each { |c| cg.const c }

  cg.calculate
  
  puts "Generating #{task.name}..."

  File.open task.name, "w" do |f|
    f.puts addrinfo.generate_config('addrinfo')
    f.puts "rbx.platform.dir.d_name = #{dirent_d_name.offset}"
    f.puts timeval.generate_config('timeval')
    f.puts sockaddr_in.generate_config('sockaddr_in')
    f.puts sockaddr_un.generate_config('sockaddr_un') if sockaddr_un.found?
    f.puts servent.generate_config('servent')

    file_constants.each do | name |
      const = cg.constants[name]
      f.puts "rbx.platform.file.#{name} = #{const.converted_value}"
    end

    io_constants.each do |name|
      const = cg.constants[name]
      f.puts "rbx.platform.io.#{name} = #{const.converted_value}"
    end

    socket_constants.each do |name|
      const = cg.constants[name]
      next if const.converted_value.nil?
      f.puts "rbx.platform.socket.#{name} = #{const.converted_value}"
    end

    (process_constants + long_process_constants).each do |name|
      const = cg.constants[name]
      next if const.converted_value.nil?
      f.puts "rbx.platform.process.#{name} = #{const.converted_value}"
    end

    signal_constants.each do |name|
      const = cg.constants[name]
      next if const.converted_value.nil?
      f.puts "rbx.platform.signal.#{name} = #{const.converted_value}"
    end
  end
  
end

desc "Build extensions from lib/ext"
task :extensions => %w[
  build:shotgun
  build:rbc

  extension:digest_md5
  extension:fcntl
  extension:syck
  extension:zlib
  extension:readline
]

namespace :extension do
  task :digest_md5 => "lib/ext/digest/md5/md5.#{$dlext}"

  file "lib/ext/digest/md5/md5.#{$dlext}" => FileList[
    'lib/ext/digest/md5/build.rb',
    'lib/ext/digest/md5/*.c',
    'lib/ext/digest/md5/*.h',
    'lib/ext/digest/defs.h',
  ] do
    compile 'lib/ext/digest/md5'
  end

  task :digest_sha1 => "lib/ext/digest/sha1/sha1.#{$dlext}"

  file "lib/ext/digest/sha1/sha1.#{$dlext}" => FileList[
    'shotgun/lib/sha1.h',
    'shotgun/lib/sha1.c',
    'lib/ext/digest/sha1/build.rb',
    'lib/ext/digest/sha1/*.c',
    'lib/ext/digest/sha1/*.h',
    'lib/ext/digest/defs.h',
  ] do
    compile 'lib/ext/digest/sha1'
  end


  task :fcntl => "lib/ext/fcntl/fcntl.#{$dlext}"

  file "lib/ext/fcntl/fcntl.#{$dlext}" => FileList[
    'shotgun/lib/subtend/ruby.h',
    'lib/ext/fcntl/build.rb',
    'lib/ext/fcntl/*.c'
  ] do
    compile "lib/ext/fcntl"
  end

  task :syck => "lib/ext/syck/rbxext.#{$dlext}"

  file "lib/ext/syck/rbxext.#{$dlext}" => FileList[
    'shotgun/lib/subtend/ruby.h',
    'lib/ext/syck/build.rb',
    'lib/ext/syck/*.c',
    'lib/ext/syck/*.h',
  ] do
    compile "lib/ext/syck"
  end
  
  task :mongrel => "lib/ext/mongrel/http11.#{$dlext}"

  file "lib/ext/mongrel/http11.#{$dlext}" => FileList[
    'shotgun/lib/subtend/ruby.h',
    'lib/ext/mongrel/build.rb',
    'lib/ext/mongrel/*.c',
    'lib/ext/mongrel/*.h',
  ] do
    compile "lib/ext/mongrel"
  end

  task :zlib => %W[lib/ext/zlib/zlib.#{$dlext} lib/zlib.rb]

  file "lib/ext/zlib/zlib.#{$dlext}" => FileList[
    'shotgun/lib/subtend/ruby.h',
    'lib/ext/zlib/build.rb',
    'lib/ext/zlib/*.c'
  ] do
    compile "lib/ext/zlib"
  end

  task :readline => %W[lib/ext/readline/readline.#{$dlext} lib/readline.rb]

  file "lib/ext/readline/readline.#{$dlext}" => FileList[
    'shotgun/lib/subtend/ruby.h',
    'lib/ext/readline/build.rb',
    'lib/ext/readline/*.c'
  ] do
    compile "lib/ext/readline"
  end
end

desc "Build task for CruiseControl"
task :ccrb => [:build, 'spec:ci']

## Include tasks to build documentation
def redcloth_present?
  if $redcloth_available.nil?
    begin
      require 'redcloth'
      $redcloth_available = true
    rescue Exception
      puts
      puts "WARNING: RedCloth 3.x is required to build the VM html docs"
      puts "Run 'gem install redcloth' to install the latest RedCloth gem"
      puts
      $redcloth_available = false
    end
  end
  $redcloth_available
end

namespace "doc" do
  namespace "vm" do

    desc "Remove all generated HTML files under doc/vm"
    task "clean" do
      Dir.glob('doc/vm/**/*.html').each do |html|
        rm_f html unless html =~ /\/?index.html$/
      end
    end

    desc "Generate HTML in doc/vm from YAML and Textile sources"
    task "html"
    
    begin
      # Define tasks for each opcode html file on the corresponding YAML file
      require 'doc/vm/op_code_info'
      OpCode::Info.op_codes.each do |op|
        html = "doc/vm/op_codes/#{op}.html"
        yaml = "doc/vm/op_codes/#{op}.yaml"
        file html => yaml do
          cd 'doc/vm' do
            ruby "gen_op_code_html.rb #{op}"
          end
        end

        task "html" => html
      end

    rescue LoadError

    end

    # Define tasks for each section html file on the corresponding textile file
    # Note: requires redcloth gem to convert textile markup to html
    Dir.glob('doc/vm/*.textile').each do |f|
      html = f.chomp('.textile') + '.html'
      file html => f do
        if redcloth_present?
          section = File.basename(f)
          cd 'doc/vm' do
            ruby "gen_section_html.rb #{section}"
          end
        end
      end

      task "html" => html
    end
  end
end

