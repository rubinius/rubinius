require 'pty/ext/pty'

module PTY
  # call-seq:
  #   PTY.check(pid, raise = false) => Process::Status or nil
  #   PTY.check(pid, true)          => nil or raises PTY::ChildExited
  #
  # Checks the status of the child process specified by +pid+.
  # Returns +nil+ if the process is still alive.  If the process
  # is not alive, will return a <tt>Process::Status</tt> or raise
  # a <tt>PTY::ChildExited</tt> (if +raise+ was true).
  #
  # +pid+:: The process id of the process to check
  # +raise+:: If true and the process identified by +pid+ is no longer
  #           alive a <tt>PTY::ChildExited</tt> is raised.
  #
  # Returns nil or a <tt>Process::Status</tt> when +raise+ is false.

  def self.check(pid, error=false)
    pid, status = Process.wait2(pid, Process::WNOHANG | Process::WUNTRACED)
    return if pid == -1
    return $? unless error

    if status & 0xff == 0x7f
      state = "stopped"
    elsif Process.kill(pid, 0) == 0
      state = "changed"
    else
      state = "exited"
    end

    msg = "pty = #{state}: #{pid}"
    raise ChildExited, msg
  end

  # call-seq:
  #   PTY.open => [master_io, slave_file]
  #   PTY.open {|master_io, slave_file| ... } => block value
  #
  # Allocates a pty (pseudo-terminal).
  #
  # In the non-block form, returns a two element array, <tt>[master_io,
  # slave_file]</tt>.
  #
  # In the block form, yields two arguments <tt>master_io, slave_file</tt>
  # and the value of the block is returned from +open+.
  #
  # The IO and File are both closed after the block completes if they haven't
  # been already closed.
  #
  # The arguments in both forms are:
  #
  # <tt>master_io</tt>:: the master of the pty, as an IO.
  # <tt>slave_file</tt>:: the slave of the pty, as a File.  The path to the
  #                       terminal device is available via
  #                       <tt>slave_file.path</tt>
  #
  # === Example
  #
  #   PTY.open {|m, s|
  #     p m      #=> #<IO:masterpty:/dev/pts/1>
  #     p s      #=> #<File:/dev/pts/1>
  #     p s.path #=> "/dev/pts/1"
  #   }
  #
  #   # Change the buffering type in factor command,
  #   # assuming that factor uses stdio for stdout buffering.
  #   # If IO.pipe is used instead of PTY.open,
  #   # this code deadlocks because factor's stdout is fully buffered.
  #   require 'io/console' # for IO#raw!
  #   m, s = PTY.open
  #   s.raw! # disable newline conversion.
  #   r, w = IO.pipe
  #   pid = spawn("factor", :in=>r, :out=>s)
  #   r.close
  #   s.close
  #   w.puts "42"
  #   p m.gets #=> "42: 2 3 7\n"
  #   w.puts "144"
  #   p m.gets #=> "144: 2 2 2 2 3 3\n"
  #   w.close
  #   # The result of read operation when pty slave is closed is platform
  #   # dependent.
  #   ret = begin
  #           m.gets          # FreeBSD returns nil.
  #         rescue Errno::EIO # GNU/Linux raises EIO.
  #           nil
  #         end
  #   p ret #=> nil

  def self.open
    master_fd, slave_fd, slave_name = __get_device__

    master = IO.new master_fd, File::RDWR
    master.sync = true

    slave = File.new slave_fd, File::RDWR | File::NOCTTY
    slave.sync = true
    slave.instance_variable_set :@path, slave_name.freeze

    pty = [master, slave]

    if block_given?
      begin
        yield pty
      ensure
        master.close unless master.closed?
        slave.close unless slave.closed?
      end
    else
      return pty
    end
  end

  private_class_method :__get_device__
end
