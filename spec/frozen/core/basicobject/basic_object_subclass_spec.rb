require File::join( File::dirname(__FILE__), %w{ .. .. spec_helper } )

ruby_version_is "1.9" do
  require File::join( File::dirname(__FILE__), %w{ shared behavior } )

  MyBO = Class::new BasicObject

  describe "BasicObject's subclasses behave" do
    extend BasicObjectBehavior

    it "privately" do
      MyBO.private_instance_methods.sort.should == private_features.sort
    end

    it "protectedly" do
      MyBO.protected_instance_methods.sort.should == protected_features.sort
    end

    it "publically" do
      MyBO.instance_methods.sort.should == public_features.sort
    end

  end
end

