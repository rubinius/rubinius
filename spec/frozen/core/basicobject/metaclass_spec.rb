require File::join( File::dirname(__FILE__), %w{ .. .. spec_helper } )

ruby_version_is "1.9" do
  require File::join( File::dirname(__FILE__), %w{ shared behavior } )

  describe "BasicObject#metaclass" do
    foo = metafoo = nil
    before :each do
      foo = BasicObject::new
      metafoo = class << foo; self end
      metafoo.module_eval do 
	def meta_test_method; 42 end
      end
    end 

    it "returns the basic object's metaclass" do
      metafoo.superclass.should == BasicObject
      ( metafoo === foo ).should == true
    end # it? Cousin it, I presume?

    it "does the job of a metaclass" do
      foo.meta_test_method.should == 42
      lambda { BasicObject::new.meta_test_method }.should raise_error( NameError )
    end # no I want repeat that stupid (ch)|(j)oke
  end

end
