require File.dirname(__FILE__) + '/../spec_helper'

# Binding has no direct creation
context 'Creating Bindings' do
  specify 'No .new provided' do
    example do
      begin
        Binding.new
      rescue => e
        e.class
      end
    end.should == NoMethodError
  end  

  specify 'Kernel.binding creates a new Binding' do
    example do
      Kernel.binding.class  
    end.should == Binding

    example do
      binding.class 
    end.should == Binding
  end
end

# Instance methods: #clone, #dup
context 'Initialised Binding' do
  setup do
    @o = Object.new
    def @o.get_binding()
      value = 1
      binding
    end
  end

  specify 'May be duplicated with #dup' do
    example do
      b = @o.get_binding
      eval('value', b) == eval('value', b.dup)
    end.should == true
  end  

  specify 'May be cloned with #clone' do
    example do
      b = @o.get_binding
      eval('value', b) == eval('value', b.clone)
    end.should == true
  end  

  
  specify 'Normal #dup and #clone semantics apply' do
    example do
      d, c = @o.get_binding, @o.get_binding

      def d.single?(); true; end
      def c.single?(); true; end

      d.freeze
      c.freeze

      [d.dup.frozen? == false, d.dup.methods.include?('single?') == false,
       c.clone.frozen? == true, c.clone.methods.include?('single?') == true,
       eval('value', d) == eval('value', d.dup), eval('value', c) == eval('value', c.clone)] 
    end.all? {|x| x == true}.should == true
  end  
end
