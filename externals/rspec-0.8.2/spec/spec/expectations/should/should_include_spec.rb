require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_include" do
  setup do
    @dummy = "dummy"
    @equal_dummy = "dummy"
    @another_dummy = "another_dummy"
    @nil_var = nil
  end

  specify "should fail when array inclusion is missing" do
    lambda do
      [1, 2, 3].should_include(5)
    end.should_fail
  end

  specify "should fail when enumerable inclusion is missing" do
    lambda do
      IO.constants.should_include("BLAH")
    end.should_fail
  end

  specify "should fail when hash inclusion is missing" do
    lambda do
      {"a" => 1}.should_include("b")
    end.should_fail
  end

  specify "should fail when string inclusion is missing" do
    lambda do
      @dummy.should_include("abc")
    end.should_fail
  end

  specify "should pass when array inclusion is present" do
    lambda do
      [1, 2, 3].should_include(2)
    end.should_pass
  end

  specify "should pass when enumerable inclusion is present" do
    lambda do
      IO.constants.should_include("SEEK_SET")
    end.should_pass
  end

  specify "should pass when hash inclusion is present" do
    lambda do
      {"a" => 1}.should_include("a")
    end.should_pass
  end

  specify "should pass when string inclusion is present" do
    lambda do
      @dummy.should_include("mm")
    end.should_pass
  end
end
   