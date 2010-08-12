require File.expand_path('../spec_helper', __FILE__)

load_extension("regexp")

describe "CApiRegexp" do
  before :each do
    @p = CApiRegexpSpecs.new
  end

  it "rb_reg_new should return a new valid Regexp" do
    my_re = @p.a_re
    my_re.kind_of?(Regexp).should == true
    ('1a' =~ my_re).should == 1
    ('1b' =~ my_re).should == nil
    my_re.source.should == 'a'
  end

  it "rb_reg_nth_match should return a the appropriate match data entry" do
    @p.a_re_1st_match(/([ab])/.match("a")).should == 'a'
    @p.a_re_1st_match(/([ab])/.match("b")).should == 'b'
    @p.a_re_1st_match(/[ab]/.match("a")).should == nil
    @p.a_re_1st_match(/[ab]/.match("c")).should == nil
  end
end
