require File::join( File::dirname(__FILE__), %w{ .. .. spec_helper } )

ruby_version_is "1.9" do
  require File::join( File::dirname(__FILE__), %w{ shared behavior } )

  describe "BasicObject class" do

    it "has no ancestors" do
      BasicObject.ancestors.should == [ BasicObject ]
    end

    it "has no superclass" do
      BasicObject.superclass.should be_nil
    end
    
    it "is a class" do
      ( Class === BasicObject ).should == true
    end

  end
end

