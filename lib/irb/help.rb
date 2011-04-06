#
#   irb/help.rb - print usage module
#   	$Release Version: 0.9.5$
#   	$Revision: 16857 $
#   	$Date: 2008-06-06 01:05:24 -0700 (Fri, 06 Jun 2008) $
#   	by Keiju ISHITSUKA(keiju@ishitsuka.com)
#
# --
#
#   
#

module IRB
  def IRB.print_usage
    lc = IRB.conf[:LC_MESSAGES]
    path = lc.find("irb/help-message.rb")
    space_line = false
    File.foreach(path) do
      |l|
      if /^\s*$/ =~ l
	lc.puts l unless space_line
	space_line = true
	next
      end
      space_line = false
      
      l.sub!(/#.*$/, "")
      l.sub!(/<<HELP/, "")
      l.sub!(/HELP/, "")
      next if /^\s*$/ =~ l
      lc.puts l
    end
  end
end

