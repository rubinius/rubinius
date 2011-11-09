#
#   math-mode.rb - 
#   	$Release Version: 0.9.5$
#   	$Revision: 11708 $
#   	$Date: 2007-02-12 15:01:19 -0800 (Mon, 12 Feb 2007) $
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#   
#
require "mathn"

module IRB
  class Context
    attr_reader :math_mode
    alias math? math_mode

    def math_mode=(opt)
      if @math_mode == true && opt == false
	IRB.fail CantReturnToNormalMode
	return
      end

      @math_mode = opt
      if math_mode
	main.extend Math
	print "start math mode\n" if verbose?
      end
    end

    def inspect?
      @inspect_mode.nil? && !@math_mode or @inspect_mode
    end
  end
end

