#  Created by Ari Brown on 2008-02-23.
#  For rubinius. All pwnage reserved.
#  
#  Used in pwning teh nubs with FFI instead of C

module Syslog
  class << self
    include Constants

    module Foreign
      # methods
      attach_function "openLog", :open, [:string, :int, :int], :void
      attach_function "closeLog", :close, [], :void
      attach_function "syslog", :write, [:int, :string, :string], :void
      attach_function "setlogmask", :set_mask, [:int], :void
      attach_function "LOG_UPTO", :LOG_UPTO, [:int], :int
      attach_function "LOG_MASK", :LOG_MASK, [:int], :int
    end

    module Constants
      # constants
      syslog_constants = %w{
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
        LOG_NODELAY
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
      }.each do |c|
        const_set(c, Rubinius::RUBY_CONFIG['rbx.platform.syslog.' + c])
      end
    end

    def ident; @ident ||= nil; end
    def options; @options ||= -1; end
    def facility; @facility ||= -1; end
    def opened; @opened ||= false; end
    def mask; @mask ||= -1; end
    attr_reader :mask

    def open(ident=nil, opt=nil, fac=nil)
      raise "Syslog already open" unless @closed
      
      @ident = ident
      @options = opt
      @facility = fac
      
      ident ||= $0
      opt = LOG_PID | LOG_CONS
      fac = LOG_USER
      
      Foreign.open(ident, opt, fac)
      
      @opened = true
      @mask = Foreign.setlogmask(0)
      
      if block_given?
        begin
          yield self
        ensure
          close
        end
      end
      
      self
    end

    def reopen(*args)
      close
      open(*args)
    end

    def opened?
      @opened
    end
    
    def close
      raise "Syslog not opened" unless @opened
      
      Foreign.close
      @ident = nil
      @options = @facility = @mask = -1;
      @opened = false
    end
    
    # [TODO] Implement this with Foreign.write
    # can't quite get how to pass args appropriately
    def log(*args)
      raise NotImplementedError
    end
    
    # handy little shortcuts
    def emerg(*args);  Foreign.write(LOG_EMERG,   *args); end
    def alert(*args);  Foreign.write(LOG_ALERT,   *args); end
    def err(*args);    Foreign.write(LOG_ERR,     *args); end
    def crit(*args);   Foreign.write(LOG_CRIT,    *args); end
    def warning(*args);Foreign.write(LOG_WARNING, *args); end
    def notice(*args); Foreign.write(LOG_NOTICE,  *args); end
    def info(*args);   Foreign.write(LOG_INFO,    *args); end
    def debug(*args);  Foreign.write(LOG_DEBUG,   *args); end
    
    
    def LOG_MASK
      Foreign.LOG_MASK(pri)
    end
    
    def LOG_UPTO pri
      Foreign.LOG_UPTO(pri)
    end
    
    def inspect
      if @opened
        "#<%s: opened=true, ident=\"%s\", options=%d, facility=%d, mask=%d>" %
        [self.name,
         @ident,
         @options,
         @facility,
         @mask]
       else
         "#<#{self.name}: opened=false>"
       end
    end
    
    def instance
      self
    end
    
    def write(pri, setup, msg)
      raise "Syslog must be opened before write" unless @opened
      
      Foreign.syslog(pri, setup, msg)
    end
    private :write
    
  end
end
