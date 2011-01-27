require File::join( File::dirname(__FILE__), %w{ .. .. spec_helper } )

ruby_version_is "1.9" do
  require File::join( File::dirname(__FILE__), %w{ shared behavior } )

  MyBO = Class::new BasicObject

  describe "BasicObject's subclasses" do
    extend BasicObjectBehavior

    it "contain only private methods from BasicObject" do
      MyBO.private_instance_methods.sort.should == private_features.sort
    end

    it "contain only protected methods from BasicObject" do
      MyBO.protected_instance_methods.sort.should == protected_features.sort
    end

    it "contain only public methods from BasicObject" do
      MyBO.instance_methods.sort.should == public_features.sort
    end

    it "can mix in Kernel and Kernel's methods work properly" do
      cls = Class.new BasicObject
      cls.send :include, Kernel
      obj = cls.new
      
      # obviously not testing every method, but a sampling should suffice
      obj.instance_variables.should == []
      obj.instance_variable_set(:@foo, 'foo').should == 'foo'
      obj.instance_variable_get(:@foo).should == 'foo'
      
      lambda {obj.send :hash}.should_not raise_error
    end
  end
end

