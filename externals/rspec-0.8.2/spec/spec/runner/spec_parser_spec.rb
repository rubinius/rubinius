require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "c" do

  specify "1" do
  end

  specify "2" do
  end

end

context "SpecParser" do
  setup do
    @p = Spec::Runner::SpecParser.new
  end

  specify "should find spec name at same line" do
    @p.spec_name_for(File.open(__FILE__), 5).should_eql "c 1"
  end

  specify "should find spec name at end of spec line" do
    @p.spec_name_for(File.open(__FILE__), 6).should_eql "c 1"
  end

  specify "should find context name above all specs" do
    @p.spec_name_for(File.open(__FILE__), 4).should_eql "c"
  end

#  specify "should find context name between specs" do
#    @p.spec_name_for(File.open(__FILE__), 7).should_eql "c"
#  end

  specify "should find nothing outside a context" do
    @p.spec_name_for(File.open(__FILE__), 2).should_be_nil
  end
end