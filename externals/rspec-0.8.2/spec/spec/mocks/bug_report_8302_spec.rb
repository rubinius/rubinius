require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Bug8302
  class Foo
    def Foo.class_method(arg)
    end
  
    def instance_bar(arg)
    end
  end

  context "Bug report 8302:" do
    specify "class method is not restored correctly when proxied" do
      Foo.should_not_receive(:class_method).with(Array.new)
      Foo.__verify
      Foo.class_method(Array.new)
    end

    specify "instance method is not restored correctly when proxied" do
      foo = Foo.new
      foo.should_not_receive(:instance_bar).with(Array.new)
      foo.__verify
      foo.instance_bar(Array.new)
    end
  end
end