#
#   change-ws.rb - 
#   	$Release Version: 0.9.5$
#   	$Revision: 8322 $
#   	$Date: 2005-04-13 08:27:09 -0700 (Wed, 13 Apr 2005) $
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#   
#

require "irb/cmd/nop.rb"
require "irb/ext/change-ws.rb"

module IRB
  module ExtendCommand

    class CurrentWorkingWorkspace<Nop
      def execute(*obj)
	irb_context.main
      end
    end

    class ChangeWorkspace<Nop
      def execute(*obj)
	irb_context.change_workspace(*obj)
	irb_context.main
      end
    end
  end
end

