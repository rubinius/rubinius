#
# Author: Pavel 'argent-smith' Argentov <argentoff@gmail.com>
#

# Serves leader/translations sync check for web.rake (see web:translations:check).
class Transcheck

  attr_reader   :leader, :tr_list
  attr_accessor :verbose

  # Takes leader dirname, a list of translation language codes and an optional
  # verbosity flag.
  def initialize(l, trl, v = false)
    @leader  = l
    @tr_list = trl
    @bugs    = {}
    @verbose = v
    @base, @llcode = File.split l
  end

  # Fires up the recursive directories check.
  def check
    puts "\nTRANSLATION CHECK: Checking '#{@leader}' recursively against #{@tr_list.join ", "} versions\n\n" if @verbose
    check_recursive @leader
  end

  # Prints the overall detection status and stats.
  def status
    @bugs.size == 0 ? ok : nb
  end

  private

  # Recursive check per se.
  def check_recursive lname
    unless (File.split lname).last =~ /^\.{1,2}$/
      if File.directory? lname
        Dir.new(lname).each do |n|
          check_recursive File.join(lname, n)
        end
      else
        @tr_list.each do |lcode|
          tfname = File.join @base, lcode, (lname.gsub leader, '')
          if File.exist? tfname
            @bugs[lname] = "is younger than its '#{lcode}' version" if File.mtime(tfname) < File.mtime(lname)
          else
            @bugs[lname] = "doesn't exist in '#{lcode}' edition"
          end
        end
      end
    end
  end

  # Says OK and explains this trouble.
  def ok
    puts "OK: Everything in #{@tr_list.to_s} is in sync with the leader '#{@leader}'\n\n"
  end

  # Notice the Bugs!
  def nb
    puts "NB:"
    bugs
  end

  # Displays found bugs.
  def bugs
    [
      [/doesn't exist/, "LOST"],
      [/younger/,        "LAG"]
    ].each do |re,lbl|
      if (hsh = @bugs.select do |k,v|
        v =~ re
      end).size > 0
      puts "\n  #{lbl} IN TRANSLATION:"
      hsh.each do |bug|
        puts "    " + (bug.join ": ")
      end
      puts
      end
    end
  end
end
