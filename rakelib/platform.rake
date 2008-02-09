file 'runtime/platform.conf' => %w[Rakefile rakelib/platform.rake rakelib/struct_generator.rb] do |task|
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
  
  stat = StructGenerator.new
  stat.include "sys/types.h"
  stat.include "sys/stat.h"
  stat.name 'struct stat'
  stat.field :st_dev
  stat.field :st_ino
  stat.field :st_mode, :ushort
  stat.field :st_nlink
  stat.field :st_uid
  stat.field :st_gid
  stat.field :st_rdev
  stat.field :st_size
  stat.field :st_blksize
  stat.field :st_blocks
  stat.field :st_atime
  stat.field :st_mtime
  stat.field :st_ctime
  stat.calculate
  
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
    S_IFMT
    S_IFIFO
    S_IFCHR
    S_IFDIR
    S_IFBLK
    S_IFREG
    S_IFLNK
    S_IFSOCK
    S_IFWHT
    S_ISUID
    S_ISGID
  }

  io_constants = %w{
    SEEK_SET
    SEEK_CUR
    SEEK_END
  }

  fcntl_constants = %w{
    F_DUPFD
    F_GETFD
    F_GETLK
    F_SETFD
    F_GETFL
    F_SETFL
    F_SETLK
    F_SETLKW
    FD_CLOEXEC
    F_RDLCK
    F_UNLCK
    F_WRLCK
    O_CREAT
    O_EXCL
    O_NOCTTY
    O_TRUNC
    O_APPEND
    O_NONBLOCK
    O_NDELAY
    O_RDONLY
    O_RDWR
    O_WRONLY
    O_ACCMODE
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
  fcntl_constants.each {|c| cg.const c }
  socket_constants.each { |c| cg.const c }
  process_constants.each { |c| cg.const c }
  long_process_constants.each { |c| cg.const(c, "%llu") }
  signal_constants.each { |c| cg.const c }
  fcntl_constants.each { |c| cg.const c }

  cg.calculate

  puts "Generating #{task.name}..."

  File.open task.name, "w" do |f|
    f.puts addrinfo.generate_config('addrinfo')
    f.puts "rbx.platform.dir.d_name = #{dirent_d_name.offset}"
    f.puts timeval.generate_config('timeval')
    f.puts sockaddr_in.generate_config('sockaddr_in')
    f.puts sockaddr_un.generate_config('sockaddr_un') if sockaddr_un.found?
    f.puts servent.generate_config('servent')
    f.puts stat.generate_config('stat')

    file_constants.each do | name |
      const = cg.constants[name]
      f.puts "rbx.platform.file.#{name} = #{const.converted_value}"
    end

    io_constants.each do |name|
      const = cg.constants[name]
      f.puts "rbx.platform.io.#{name} = #{const.converted_value}"
    end

    fcntl_constants.each do |name|
      const = cg.constants[name]
      next if const.converted_value.nil?
      f.puts "rbx.platform.fcntl.#{name} = #{const.converted_value}"
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
  CONFIG['wordsize'] = Rubinius::WORDSIZE

end

RbConfig = Config
EOF

  File.open 'lib/rbconfig.rb', 'w' do |fp|
    fp.write rbconfig
  end
end

