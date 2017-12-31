require File.expand_path('../../library/ffi/generators', __FILE__)

deps = ["Rakefile",
        "rakelib/platform.rake"
] + Dir['lib/ffi/*rb']

file "#{BUILD_CONFIG[:builddir]}#{BUILD_CONFIG[:codedbdir]}/platform.conf" => deps do |task|
  puts "GEN codedb/platform.conf"

  File.open task.name, "wb" do |f|
    Rubinius::FFI::Generators::Structures.new 'addrinfo' do |s|
      if BUILD_CONFIG[:windows]
        s.include "ws2tcpip.h"
      else
        s.include 'sys/socket.h'
        s.include 'netdb.h'
      end
      s.name 'struct addrinfo'
      s.field :ai_flags, :int
      s.field :ai_family, :int
      s.field :ai_socktype, :int
      s.field :ai_protocol, :int
      s.field :ai_addrlen, :int
      s.field :ai_addr, :pointer
      s.field :ai_canonname, :string
      s.field :ai_next, :pointer
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'ifaddrs' do |s|
      s.include 'sys/types.h'
      s.include 'ifaddrs.h'

      s.name 'struct ifaddrs'

      s.field :ifa_next, :pointer
      s.field :ifa_name, :string
      s.field :ifa_flags, :int
      s.field :ifa_addr, :pointer
      s.field :ifa_netmask, :pointer
      s.field :ifa_broadaddr, :pointer
      s.field :ifa_dstaddr, :pointer
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'sockaddr' do |s|
      s.include 'sys/socket.h'

      s.name 'struct sockaddr'

      s.field :sa_data, :char_array
      s.field :sa_family, :sa_family_t
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'dirent' do |s|
      s.include "sys/types.h"
      s.include "dirent.h"
      s.name 'struct dirent'
      s.field :d_ino, :ino_t
      s.field :d_reclen, :ushort
      s.field :d_name, :char_array
    end.write_config(f)

    struct = Rubinius::FFI::Generators::Structures.new 'timeval' do |s|
      s.include "sys/time.h"
      s.name 'struct timeval'
      s.field :tv_sec, :time_t
      s.field :tv_usec, :suseconds_t
    end

    struct.write_config(f)
    struct.write_class(f)

    Rubinius::FFI::Generators::Structures.new 'sockaddr_in' do |s|
      if BUILD_CONFIG[:windows]
        s.include "ws2tcpip.h"
      else
        s.include "netinet/in.h"
        s.include "sys/socket.h"
      end
      s.include "fcntl.h"
      s.include "sys/stat.h"
      s.name 'struct sockaddr_in'
      s.field :sin_family, :sa_family_t
      s.field :sin_port, :ushort
      s.field :sin_addr
      s.field :sin_zero, :char_array
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'sockaddr_in6' do |s|
      if BUILD_CONFIG[:windows]
        s.include "ws2tcpip.h"
      else
        s.include "netinet/in.h"
        s.include "sys/socket.h"
      end
      s.include "fcntl.h"
      s.include "sys/stat.h"
      s.name 'struct sockaddr_in6'
      s.field :sin6_family, :sa_family_t
      s.field :sin6_port, :ushort
      s.field :sin6_flowinfo
      s.field :sin6_addr, :char_array
      s.field :sin6_scope_id
    end.write_config(f)

    unless BUILD_CONFIG[:windows]
      sockaddr_un = Rubinius::FFI::Generators::Structures.new 'sockaddr_un' do |s|
        s.include "sys/un.h"
        s.name 'struct sockaddr_un'
        s.field :sun_family, :sa_family_t
        s.field :sun_path, :char_array
      end

      sockaddr_un.write_config f if sockaddr_un.found?
    end

    Rubinius::FFI::Generators::Structures.new 'hostent' do |s|
      s.include 'netdb.h'
      s.name 'struct hostent'

      s.field :h_name, :string
      s.field :h_aliases, :pointer
      s.field :h_addrtype, :int
      s.field :h_length, :int
      s.field :h_addr_list, :pointer
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'linger' do |s|
      if BUILD_CONFIG[:windows]
        s.include "winsock2.h"
      else
        s.include "sys/socket.h"
      end
      s.name "struct linger"
        s.field :l_onoff
        s.field :l_linger
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'iovec' do |s|
      s.include 'sys/socket.h'
      s.name 'struct iovec'

      s.field :iov_base, :pointer
      s.field :iov_len, :size_t
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'msghdr' do |s|
      s.include 'sys/socket.h'
      s.name 'struct msghdr'

      s.field :msg_name, :pointer
      s.field :msg_namelen, :int
      s.field :msg_iov, :pointer
      s.field :msg_iovlen, :size_t
      s.field :msg_control, :pointer
      s.field :msg_controllen, :size_t
      s.field :msg_flags, :int
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'servent' do |s|
      if BUILD_CONFIG[:windows]
        s.include "winsock2.h"
      else
        s.include "netdb.h"
      end
      s.name 'struct servent'
      s.field :s_name, :pointer
      s.field :s_aliases, :pointer
      s.field :s_port, :int
      s.field :s_proto, :pointer
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'stat' do |s|
      unless BUILD_CONFIG[:windows]
        s.include "sys/types.h"
        s.include "unistd.h"
        s.include "time.h"
      end
      s.include "sys/stat.h"
      s.name 'struct stat'
      s.field :st_dev, :dev_t
      s.field :st_ino, :ino_t
      s.field :st_mode, :mode_t
      if BUILD_CONFIG[:windows]
        s.field :st_nlink, :short
        s.field :st_uid, :short
        s.field :st_gid, :short
      else
        s.field :st_nlink, :nlink_t
        s.field :st_uid, :uid_t
        s.field :st_gid, :gid_t
      end
      s.field :st_rdev, :dev_t
      s.field :st_size, :off_t
      unless BUILD_CONFIG[:windows]
        s.field :st_blksize
        s.field :st_blocks
      end
      s.field :st_atime, :time_t
      s.field :st_mtime, :time_t
      s.field :st_ctime, :time_t
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'pwd' do |s|
      s.include "sys/types.h"
      s.include "pwd.h"

      s.name "struct passwd"
      s.field :pw_name, :string
      s.field :pw_passwd, :string
      s.field :pw_uid, :uid_t
      s.field :pw_gid, :gid_t
      s.field :pw_gecos, :string
      s.field :pw_dir, :string
      s.field :pw_shell, :string
    end.write_config(f)

    Rubinius::FFI::Generators::Structures.new 'grp' do |s|
      s.include "sys/types.h"
      s.include "grp.h"

      s.name "struct group"
      s.field :gr_name, :string
      s.field :gr_passwd, :string
      s.field :gr_gid, :gid_t
      s.field :gr_mem, :pointer
    end.write_config(f)

    unless BUILD_CONFIG[:windows]
      Rubinius::FFI::Generators::Structures.new 'rlimit' do |s|
        s.include "sys/types.h"
        s.include "sys/time.h"
        s.include "sys/resource.h"
        s.name 'struct rlimit'
        s.field :rlim_cur, :rlim_t
        s.field :rlim_max, :rlim_t
      end.write_config(f)
    end

    # FIXME these constants don't have standard names.  LOCK_SH == Linux,
    # O_SHLOCK on Bsd/Darwin, etc.  Binary doesn't exist at all in many non-Unix
    # variants.  This should come out of something like config.h

    # fixme_constants = %w{
    #   LOCK_SH
    #   LOCK_EX
    #   LOCK_NB
    #   LOCK_UN
    #   BINARY
    # }

    Rubinius::FFI::Generators::Constants.new 'rbx.platform.file' do |cg|
      cg.include 'stdio.h'
      cg.include 'fcntl.h'
      cg.include 'sys/stat.h'

      file_constants = %w[
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
        O_CLOEXEC
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
        S_ISVTX
      ]

      file_constants.each { |c| cg.const c }
    end.write_constants(f)

    Rubinius::FFI::Generators::Constants.new 'rbx.platform.io' do |cg|
      cg.include 'stdio.h'

      io_constants = %w[
        SEEK_SET
        SEEK_CUR
        SEEK_END
      ]

      io_constants.each { |c| cg.const c }
    end.write_constants(f)
    
    Rubinius::FFI::Generators::Constants.new 'rbx.platform.select' do |cg|
      cg.include 'sys/select.h'
      
      select_constants = %w[
        FD_SETSIZE
      ]
      
      select_constants.each { |c| cg.const c }
    end.write_constants(f)

    # Not available on all platforms. Try to load these constants anyway.
    Rubinius::FFI::Generators::Constants.new 'rbx.platform.advise' do |cg|
      cg.include 'fcntl.h'

      advise_constants = %w[
        POSIX_FADV_NORMAL
        POSIX_FADV_SEQUENTIAL
        POSIX_FADV_RANDOM
        POSIX_FADV_WILLNEED
        POSIX_FADV_DONTNEED
        POSIX_FADV_NOREUSE
      ]

      advise_constants.each { |c| cg.const c }
    end.write_constants(f)

    Rubinius::FFI::Generators::Constants.new 'rbx.platform.fcntl' do |cg|
      cg.include 'fcntl.h'

      fcntl_constants = %w[
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
      ]

      fcntl_constants.each { |c| cg.const c }
    end.write_constants(f)

    Rubinius::FFI::Generators::Constants.new 'rbx.platform.syslog' do |cg|
      cg.include 'syslog.h'

      syslog_constants = %w[
        LOG_EMERG
        LOG_ALERT
        LOG_ERR
        LOG_CRIT
        LOG_WARNING
        LOG_NOTICE
        LOG_INFO
        LOG_DEBUG
        LOG_PID
        LOG_CONS
        LOG_ODELAY
        LOG_NDELAY
        LOG_NOWAIT
        LOG_PERROR
        LOG_AUTH
        LOG_AUTHPRIV
        LOG_CONSOLE
        LOG_CRON
        LOG_DAEMON
        LOG_FTP
        LOG_KERN
        LOG_LPR
        LOG_MAIL
        LOG_NEWS
        LOG_NTP
        LOG_SECURITY
        LOG_SYSLOG
        LOG_USER
        LOG_UUCP
        LOG_LOCAL0
        LOG_LOCAL1
        LOG_LOCAL2
        LOG_LOCAL3
        LOG_LOCAL4
        LOG_LOCAL5
        LOG_LOCAL6
        LOG_LOCAL7
      ]

      syslog_constants.each { |c| cg.const c }
    end.write_constants(f)

    Rubinius::FFI::Generators::Constants.new 'rbx.platform.socket' do |cg|
      cg.include 'sys/types.h'

      if BUILD_CONFIG[:windows]
        cg.include "winsock2.h"
      else
        cg.include 'sys/socket.h'
        cg.include 'netdb.h'
        cg.include 'netinet/in_systm.h'
        cg.include 'netinet/tcp.h'
        cg.include 'netinet/udp.h'
        cg.include 'netinet/in.h'
        cg.include 'net/if.h'
      end

      socket_constants = %w[
        AF_APPLETALK
        AF_AX25
        AF_INET
        AF_INET6
        AF_IPX
        AF_ISDN
        AF_LOCAL
        AF_MAX
        AF_PACKET
        AF_ROUTE
        AF_SNA
        AF_UNIX
        AF_UNSPEC
        AI_ADDRCONFIG
        AI_ALL
        AI_CANONNAME
        AI_NUMERICHOST
        AI_NUMERICSERV
        AI_PASSIVE
        AI_V4MAPPED
        EAI_ADDRFAMILY
        EAI_AGAIN
        EAI_BADFLAGS
        EAI_FAIL
        EAI_FAMILY
        EAI_MEMORY
        EAI_NODATA
        EAI_NONAME
        EAI_OVERFLOW
        EAI_SERVICE
        EAI_SOCKTYPE
        EAI_SYSTEM
        FNM_CASEFOLD
        FNM_DOTMATCH
        FNM_EXTGLOB
        FNM_NOESCAPE
        FNM_PATHNAME
        FNM_SYSCASE
        IFF_ALLMULTI
        IFF_AUTOMEDIA
        IFF_BROADCAST
        IFF_DEBUG
        IFF_DYNAMIC
        IFF_LOOPBACK
        IFF_MASTER
        IFF_MULTICAST
        IFF_NOARP
        IFF_NOTRAILERS
        IFF_POINTOPOINT
        IFF_PORTSEL
        IFF_PROMISC
        IFF_RUNNING
        IFF_SLAVE
        IFF_UP
        IF_NAMESIZE
        INADDR_ALLHOSTS_GROUP
        INADDR_ANY
        INADDR_BROADCAST
        INADDR_LOOPBACK
        INADDR_MAX_LOCAL_GROUP
        INADDR_NONE
        INADDR_UNSPEC_GROUP
        INET6_ADDRSTRLEN
        INET_ADDRSTRLEN
        IPPORT_RESERVED
        IPPORT_USERRESERVED
        IPPROTO_AH
        IPPROTO_DSTOPTS
        IPPROTO_EGP
        IPPROTO_ESP
        IPPROTO_FRAGMENT
        IPPROTO_HOPOPTS
        IPPROTO_ICMP
        IPPROTO_ICMPV6
        IPPROTO_IDP
        IPPROTO_IGMP
        IPPROTO_IP
        IPPROTO_IPV6
        IPPROTO_NONE
        IPPROTO_PUP
        IPPROTO_RAW
        IPPROTO_ROUTING
        IPPROTO_TCP
        IPPROTO_TP
        IPPROTO_UDP
        IPV6_CHECKSUM
        IPV6_DONTFRAG
        IPV6_DSTOPTS
        IPV6_HOPLIMIT
        IPV6_HOPOPTS
        IPV6_JOIN_GROUP
        IPV6_LEAVE_GROUP
        IPV6_MULTICAST_HOPS
        IPV6_MULTICAST_IF
        IPV6_MULTICAST_LOOP
        IPV6_NEXTHOP
        IPV6_PATHMTU
        IPV6_PKTINFO
        IPV6_RECVDSTOPTS
        IPV6_RECVHOPLIMIT
        IPV6_RECVHOPOPTS
        IPV6_RECVPATHMTU
        IPV6_RECVPKTINFO
        IPV6_RECVRTHDR
        IPV6_RECVTCLASS
        IPV6_RTHDR
        IPV6_RTHDRDSTOPTS
        IPV6_RTHDR_TYPE_0
        IPV6_TCLASS
        IPV6_UNICAST_HOPS
        IPV6_V6ONLY
        IP_ADD_MEMBERSHIP
        IP_ADD_SOURCE_MEMBERSHIP
        IP_BLOCK_SOURCE
        IP_DEFAULT_MULTICAST_LOOP
        IP_DEFAULT_MULTICAST_TTL
        IP_DROP_MEMBERSHIP
        IP_DROP_SOURCE_MEMBERSHIP
        IP_FREEBIND
        IP_HDRINCL
        IP_IPSEC_POLICY
        IP_MAX_MEMBERSHIPS
        IP_MINTTL
        IP_MSFILTER
        IP_MTU
        IP_MTU_DISCOVER
        IP_MULTICAST_IF
        IP_MULTICAST_LOOP
        IP_MULTICAST_TTL
        IP_OPTIONS
        IP_PASSSEC
        IP_PKTINFO
        IP_PKTOPTIONS
        IP_PMTUDISC_DO
        IP_PMTUDISC_DONT
        IP_PMTUDISC_WANT
        IP_RECVERR
        IP_RECVOPTS
        IP_RECVRETOPTS
        IP_RECVTOS
        IP_RECVTTL
        IP_RETOPTS
        IP_ROUTER_ALERT
        IP_TOS
        IP_TRANSPARENT
        IP_TTL
        IP_UNBLOCK_SOURCE
        IP_XFRM_POLICY
        LOCK_EX
        LOCK_NB
        LOCK_SH
        LOCK_UN
        MCAST_BLOCK_SOURCE
        MCAST_EXCLUDE
        MCAST_INCLUDE
        MCAST_JOIN_GROUP
        MCAST_JOIN_SOURCE_GROUP
        MCAST_LEAVE_GROUP
        MCAST_LEAVE_SOURCE_GROUP
        MCAST_MSFILTER
        MCAST_UNBLOCK_SOURCE
        MSG_CONFIRM
        MSG_CTRUNC
        MSG_DONTROUTE
        MSG_DONTWAIT
        MSG_EOR
        MSG_ERRQUEUE
        MSG_FASTOPEN
        MSG_FIN
        MSG_MORE
        MSG_NOSIGNAL
        MSG_OOB
        MSG_PEEK
        MSG_PROXY
        MSG_RST
        MSG_SYN
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
        PF_INET
        PF_INET6
        PF_IPX
        PF_ISDN
        PF_KEY
        PF_LOCAL
        PF_MAX
        PF_PACKET
        PF_ROUTE
        PF_SNA
        PF_UNIX
        PF_UNSPEC
        SCM_CREDENTIALS
        SCM_RIGHTS
        SCM_TIMESTAMP
        SCM_TIMESTAMPING
        SCM_TIMESTAMPNS
        SCM_WIFI_STATUS
        SEEK_CUR
        SEEK_DATA
        SEEK_END
        SEEK_HOLE
        SEEK_SET
        SHUT_RD
        SHUT_RDWR
        SHUT_WR
        SOCK_DGRAM
        SOCK_PACKET
        SOCK_RAW
        SOCK_RDM
        SOCK_SEQPACKET
        SOCK_STREAM
        SOL_IP
        SOL_SOCKET
        SOL_TCP
        SOL_UDP
        SO_ACCEPTCONN
        SO_ATTACH_FILTER
        SO_BINDTODEVICE
        SO_BPF_EXTENSIONS
        SO_BROADCAST
        SO_BUSY_POLL
        SO_DEBUG
        SO_DETACH_FILTER
        SO_DOMAIN
        SO_DONTROUTE
        SO_ERROR
        SO_GET_FILTER
        SO_KEEPALIVE
        SO_LINGER
        SO_LOCK_FILTER
        SO_MARK
        SO_MAX_PACING_RATE
        SO_NOFCS
        SO_NO_CHECK
        SO_OOBINLINE
        SO_PASSCRED
        SO_PASSSEC
        SO_PEEK_OFF
        SO_PEERCRED
        SO_PEERNAME
        SO_PEERSEC
        SO_PRIORITY
        SO_PROTOCOL
        SO_RCVBUF
        SO_RCVBUFFORCE
        SO_RCVLOWAT
        SO_RCVTIMEO
        SO_REUSEADDR
        SO_REUSEPORT
        SO_RXQ_OVFL
        SO_SECURITY_AUTHENTICATION
        SO_SECURITY_ENCRYPTION_NETWORK
        SO_SECURITY_ENCRYPTION_TRANSPORT
        SO_SELECT_ERR_QUEUE
        SO_SNDBUF
        SO_SNDBUFFORCE
        SO_SNDLOWAT
        SO_SNDTIMEO
        SO_TIMESTAMP
        SO_TIMESTAMPING
        SO_TIMESTAMPNS
        SO_TYPE
        SO_WIFI_STATUS
        TCP_CONGESTION
        TCP_COOKIE_TRANSACTIONS
        TCP_CORK
        TCP_DEFER_ACCEPT
        TCP_FASTOPEN
        TCP_INFO
        TCP_KEEPCNT
        TCP_KEEPIDLE
        TCP_KEEPINTVL
        TCP_LINGER2
        TCP_MAXSEG
        TCP_MD5SIG
        TCP_NODELAY
        TCP_QUEUE_SEQ
        TCP_QUICKACK
        TCP_REPAIR
        TCP_REPAIR_OPTIONS
        TCP_REPAIR_QUEUE
        TCP_SYNCNT
        TCP_THIN_DUPACK
        TCP_THIN_LINEAR_TIMEOUTS
        TCP_TIMESTAMP
        TCP_USER_TIMEOUT
        TCP_WINDOW_CLAMP
        UDP_CORK
        SOMAXCONN
      ]

      socket_constants.each { |c| cg.const c }
    end.write_constants(f)

    unless BUILD_CONFIG[:windows]
      Rubinius::FFI::Generators::Constants.new 'rbx.platform.process' do |cg|
        cg.include 'sys/wait.h'
        cg.include 'sys/resource.h'
        cg.include 'stdlib.h'

        process_constants = %w{
          EXIT_SUCCESS
          EXIT_FAILURE

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
          RLIMIT_RTPRIO
          RLIMIT_RTTIME
          RLIMIT_SIGPENDING
          RLIMIT_MSGQUEUE
          RLIMIT_NICE
        }

        process_constants.each { |c| cg.const c }

        long_process_constants = %w[
          RLIM_INFINITY
          RLIM_SAVED_MAX
          RLIM_SAVED_CUR
        ]

        long_process_constants.each { |c|
          cg.const c, "%llu", "(unsigned long long)"
        }
      end.write_constants(f)
    end

    # The constants come from MRI's signal.c. This means that some of them might
    # be missing.

    Rubinius::FFI::Generators::Constants.new 'rbx.platform.signal' do |cg|
      cg.include 'signal.h'
      unless BUILD_CONFIG[:windows] or RUBY_PLATFORM.match(/linux-musl$/)
        cg.include 'sys/signal.h'
      end

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

      signal_constants.each { |c| cg.const c }
    end.write_constants(f)

    Rubinius::FFI::Generators::Constants.new 'rbx.platform.zlib' do |cg|
      cg.include_dir 'build/libraries/zlib' if BUILD_CONFIG[:vendor_zlib]
      cg.include 'zlib.h'

      zlib_constants = %w[ZLIB_VERSION]

      zlib_constants.each { |c| cg.const c, "%s", "(char *)" }
    end.write_constants(f)

    unless BUILD_CONFIG[:windows]
      Rubinius::FFI::Generators::Constants.new 'rbx.platform.dlopen' do |cg|
        cg.include 'dlfcn.h'

        dlopen_constants = %w[
          RTLD_LAZY
          RTLD_NOW
          RTLD_LOCAL
          RTLD_GLOBAL
        ]

        dlopen_constants.each { |c| cg.const c }
      end.write_constants(f)
    end

    f.puts Rubinius::FFI::Generators::Types.new.generate
  end
end
