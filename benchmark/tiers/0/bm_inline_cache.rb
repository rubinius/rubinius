require 'benchmark'
require 'benchmark/ips'

class Class0; def called_method; end; end
class Class1; def called_method; end; end
class Class2; def called_method; end; end
class Class3; def called_method; end; end
class Class4; def called_method; end; end
class Class5; def called_method; end; end
class Class6; def called_method; end; end
class Class7; def called_method; end; end
class Class8; def called_method; end; end
class Class9; def called_method; end; end

def call_on(obj)
  obj.called_method
end

object0 = Class0.new
object1 = Class1.new
object2 = Class2.new
object3 = Class3.new
object4 = Class4.new
object5 = Class5.new
object6 = Class6.new
object7 = Class7.new
object8 = Class8.new
object9 = Class9.new

Benchmark.ips do |x|
  x.report "monomorphic call site" do |times|
    i = 0
    while i < times
      call_on(object0)
      i += 1
    end
  end

  x.report "bimorphic call site" do |times|
    i = 0
    while i < times
      call_on(object0)
      call_on(object1)
      i += 1
    end
  end

  x.report "polymorphic call site with 3 entrie" do |times|
    i = 0
    while i < times
      call_on(object0)
      call_on(object1)
      call_on(object2)
      i += 1
    end
  end

  x.report "polymorphic call site with 6 entrie" do |times|
    i = 0
    while i < times
      call_on(object0)
      call_on(object1)
      call_on(object2)
      call_on(object3)
      call_on(object4)
      call_on(object5)
      i += 1
    end
  end

  x.report "megamorphic call site with 10 entrie" do |times|
    i = 0
    while i < times
      call_on(object0)
      call_on(object1)
      call_on(object2)
      call_on(object3)
      call_on(object4)
      call_on(object5)
      call_on(object6)
      call_on(object7)
      call_on(object8)
      call_on(object9)
      i += 1
    end
  end
end
