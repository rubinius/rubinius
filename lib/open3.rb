#
# = open3.rb: Popen, but with stderr, too
#
# Author:: Yukihiro Matsumoto
# Documentation:: Konrad Meyer
#
# Open3 gives you access to stdin, stdout, and stderr when running other
# programs.
#

#
# Open3 grants you access to stdin, stdout, and stderr when running another
# program. Example:
#
#   require "open3"
#   include Open3
#   
#   stdin, stdout, stderr = popen3('nroff -man')
#
# Open3.popen3 can also take a block which will receive stdin, stdout and
# stderr as parameters.  This ensures stdin, stdout and stderr are closed
# once the block exits. Example:
#
#   require "open3"
#
#   Open3.popen3('nroff -man') { |stdin, stdout, stderr| ... }
#

module Open3
  # 
  # Open stdin, stdout, and stderr streams and start external executable.
  # Non-block form:
  #   
  #   require 'open3'
  #
  #   [stdin, stdout, stderr] = Open3.popen3(cmd)
  #
  # Block form:
  #
  #   require 'open3'
  #
  #   Open3.popen3(cmd) { |stdin, stdout, stderr| ... }
  #
  # The parameter +cmd+ is passed directly to Kernel#exec.
  #
  def popen3(*cmd)
    pw = IO.pipe   # pipe[0] for read, pipe[1] for write
    pr = IO.pipe
    pe = IO.pipe

    pid = fork{
      # child
      fork {
        # grandchild
        pw[1].close
        STDIN.reopen(pw[0])
        pw[0].close

        pr[0].close
        STDOUT.reopen(pr[1])
        pr[1].close

        pe[0].close
        STDERR.reopen(pe[1])
        pe[1].close

        exec(*cmd)
      }
      exit!(0)
    }

    pw[0].close
    pr[1].close
    pe[1].close

    Process.waitpid(pid)

    pi = [pw[1], pr[0], pe[0]]
    pw[1].sync = true

    if block_given?
      begin
        return yield(*pi)
      ensure
        pi.each { |p| p.close unless p.closed? }
      end
    end
    pi
  end
  module_function :popen3
end

if $0 == __FILE__
  a = Open3.popen3("nroff -man")
  Thread.start do
    while line = gets
      a[0].print line
    end
    a[0].close
  end
  while line = a[1].gets
    print ":", line
  end
end
