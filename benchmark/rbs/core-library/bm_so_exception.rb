# $Id: except-ruby.code,v 1.4 2004/11/13 07:41:33 bfulgham Exp $
# http://www.bagley.org/~doug/shootout/

$HI = 0
$LO = 0

class Lo_Exception < Exception
  def initialize(num)
    @value = num
  end
end

class Hi_Exception < Exception
  def initialize(num)
    @value = num
  end
end

def some_function(num)
  begin
    hi_function(num)
  rescue
    print "We shouldn't get here, exception is: #{$!.type}\n"
  end
end

def hi_function(num)
  begin
    lo_function(num)
  rescue Hi_Exception
    $HI = $HI + 1
  end
end

def lo_function(num)
  begin
    blowup(num)
  rescue Lo_Exception
    $LO = $LO + 1
  end
end

def blowup(num)
  if num % 2 == 0
    raise Lo_Exception.new(num)
  else
    raise Hi_Exception.new(num)
  end
end

Bench.run [500_000] do |n|
  n.times {|i| some_function(i+1) }
end
