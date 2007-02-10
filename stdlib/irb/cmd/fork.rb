#
#   fork.rb - 
#   	$Release Version: 0.9.5 $
#   	$Revision: 8322 $
#   	$Date: 2005-04-13 08:27:09 -0700 (Wed, 13 Apr 2005) $
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#   
#

@RCS_ID='-$Id: fork.rb 8322 2005-04-13 15:27:09Z keiju $-'


module IRB
  module ExtendCommand
    class Fork<Nop
      def execute(&block)
	pid = send ExtendCommand.irb_original_method_name("fork")
	unless pid 
	  class<<self
	    alias_method :exit, ExtendCommand.irb_original_method_name('exit')
	  end
	  if iterator?
	    begin
	      yield
	    ensure
	      exit
	    end
	  end
	end
	pid
      end
    end
  end
end


