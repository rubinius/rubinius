shared :set_method_visibility do |cmd|
  if cmd == :private 
    original = :public_one
  else
    original = :private_one
  end
  
  one = "#{cmd}_one".to_sym
  aliased = "made_#{cmd}".to_sym
  aliased_again = "#{aliased}_again".to_sym
  check = lambda do |name|
    o = @cls.new
    @cls.send("check_#{cmd}", o, name)
  end

  describe "Module##{cmd}" do
    
    before(:each) do
      @cls = Class.new(ModuleSpecs::Aliasing) do
        def invoke
          self.target
        end

        def self.check_private(o, name)
          self.make_alias(:target, name)
          lambda { o.target }.should raise_error(NoMethodError)
        end

        def self.check_public(o, name)
          self.make_alias(:target, name)
          o.target == 1
        end

        def self.check_protected(o, name)
          self.make_alias(:target, name)
          lambda { o.target }.should raise_error(NoMethodError)
          o.invoke.should == 1
        end
      end
      @cls.make_alias(aliased, original)
    end
    
    it "sets the visibility for subsequent methods if called without args" do
      check[one]
    end
    
    it "sets the named methods visibility" do
      @cls.make_alias(aliased_again, original)
      @cls.send(cmd, aliased, aliased_again)
      check[aliased]
      check[aliased_again]
    end

    it "converts a non string/symbol/fixnum name to string using to_str" do
      to_str = Object.new
      to_str.should_receive(:to_str, :returning => aliased.to_s)
      names = [aliased, aliased.to_s, aliased.to_i, to_str]
      names.each do |name|
        @cls.make_alias(aliased, original)
        @cls.send(cmd, name)
        check[aliased]
      end
    end

    it "raises TypeError when the given names can't be converted using to_str" do
      lambda { @cls.send(cmd, Object.new) }.should raise_error(TypeError)
      invalid = Object.new
      invalid.should_receive(:to_str, :returning => Object.new)
      lambda { @cls.send(cmd, invalid) }.should raise_error(TypeError)
    end
  end
end
