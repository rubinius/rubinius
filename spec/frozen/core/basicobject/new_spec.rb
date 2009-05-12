require File::join( File::dirname(__FILE__), %w{ .. .. spec_helper } )

ruby_version_is "1.9".."1.9.9" do
  describe "BasicObject.new" do
    it "creates a new BasicObject" do
      ( BasicObject === BasicObject.new ).should == true
    end
  end
end

