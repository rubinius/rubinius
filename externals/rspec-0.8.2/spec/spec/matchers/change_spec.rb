#Based on patch from Wilson Bilkovich

require File.dirname(__FILE__) + '/../../spec_helper.rb'
class SomethingExpected
  attr_accessor :some_value
end

context "should change(actual, message)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 5
  end

  specify "should pass when actual is modified by the block" do
    lambda {@instance.some_value = 6}.should change(@instance, :some_value)
  end

  specify "should fail when actual is not modified by the block" do
    lambda do
      lambda {}.should change(@instance, :some_value)
    end.should fail_with("some_value should have changed, but is still 5")
  end
end

context "should_not change(actual, message)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 5
  end

  specify "should pass when actual is not modified by the block" do
    lambda { }.should_not change(@instance, :some_value)
  end

  specify "should fail when actual is not modified by the block" do
    lambda do
      lambda {@instance.some_value = 6}.should_not change(@instance, :some_value)
    end.should fail_with("some_value should not have changed, but did change from 5 to 6")
  end
end

context "should change { block }" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 5
  end

  specify "should pass when actual is modified by the block" do
    lambda {@instance.some_value = 6}.should change { @instance.some_value }
  end

  specify "should fail when actual is not modified by the block" do
    lambda do
      lambda {}.should change{ @instance.some_value }
    end.should fail_with("result should have changed, but is still 5")
  end
  
  specify "should warn if passed a block using do/end" do
    lambda do
      lambda {}.should change do
      end
    end.should_raise(Spec::Matchers::MatcherError, /block passed to should or should_not/)
  end
end

context "should_not change { block }" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 5
  end

  specify "should pass when actual is modified by the block" do
    lambda {}.should_not change{ @instance.some_value }
  end

  specify "should fail when actual is not modified by the block" do
    lambda do
      lambda {@instance.some_value = 6}.should_not change { @instance.some_value }
    end.should fail_with("result should not have changed, but did change from 5 to 6")
  end
  
  specify "should warn if passed a block using do/end" do
    lambda do
      lambda {}.should_not change do
      end
    end.should_raise(Spec::Matchers::MatcherError, /block passed to should or should_not/)
  end
end

context "should change(actual, message).by(expected)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 5
  end

  specify "should pass when attribute is changed by expected amount" do
    lambda { @instance.some_value += 1 }.should change(@instance, :some_value).by(1)
  end

  specify "should fail when the attribute is changed by unexpected amount" do
    lambda do
      lambda { @instance.some_value += 2 }.should change(@instance, :some_value).by(1)
    end.should fail_with("some_value should have been changed by 1, but was changed by 2")
  end

  specify "should fail when the attribute is changed by unexpected amount in the opposite direction" do
    lambda do
      lambda { @instance.some_value -= 1 }.should change(@instance, :some_value).by(1)
    end.should fail_with("some_value should have been changed by 1, but was changed by -1")
  end
end

context "should change{ block }.by(expected)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 5
  end

  specify "should pass when attribute is changed by expected amount" do
    lambda { @instance.some_value += 1 }.should change{@instance.some_value}.by(1)
  end

  specify "should fail when the attribute is changed by unexpected amount" do
    lambda do
      lambda { @instance.some_value += 2 }.should change{@instance.some_value}.by(1)
    end.should fail_with("result should have been changed by 1, but was changed by 2")
  end

  specify "should fail when the attribute is changed by unexpected amount in the opposite direction" do
    lambda do
      lambda { @instance.some_value -= 1 }.should change{@instance.some_value}.by(1)
    end.should fail_with("result should have been changed by 1, but was changed by -1")
  end
end

context "should change(actual, message).from(old)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 'string'
  end

  specify "should pass when attribute is == to expected value before executing block" do
    lambda { @instance.some_value = "astring" }.should change(@instance, :some_value).from("string")
  end

  specify "should fail when attribute is not == to expected value before executing block" do
    lambda do
      lambda { @instance.some_value = "knot" }.should change(@instance, :some_value).from("cat")
    end.should fail_with("some_value should have initially been \"cat\", but was \"string\"")
  end
end

context "should change{ block }.from(old)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 'string'
  end

  specify "should pass when attribute is == to expected value before executing block" do
    lambda { @instance.some_value = "astring" }.should change{@instance.some_value}.from("string")
  end

  specify "should fail when attribute is not == to expected value before executing block" do
    lambda do
      lambda { @instance.some_value = "knot" }.should change{@instance.some_value}.from("cat")
    end.should fail_with("result should have initially been \"cat\", but was \"string\"")
  end
end

context "should change(actual, message).to(new)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 'string'
  end
  
  specify "should pass when attribute is == to expected value after executing block" do
    lambda { @instance.some_value = "cat" }.should change(@instance, :some_value).to("cat")
  end

  specify "should fail when attribute is not == to expected value after executing block" do
    lambda do
      lambda { @instance.some_value = "cat" }.should change(@instance, :some_value).from("string").to("dog")
    end.should fail_with("some_value should have been changed to \"dog\", but is now \"cat\"")
  end
end

context "should change{ block }.to(new)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 'string'
  end
  
  specify "should pass when attribute is == to expected value after executing block" do
    lambda { @instance.some_value = "cat" }.should change{@instance.some_value}.to("cat")
  end

  specify "should fail when attribute is not == to expected value after executing block" do
    lambda do
      lambda { @instance.some_value = "cat" }.should change{@instance.some_value}.from("string").to("dog")
    end.should fail_with("result should have been changed to \"dog\", but is now \"cat\"")
  end
end

context "should change(actual, message).from(old).to(new)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 'string'
  end
  
  specify "should pass when #to comes before #from" do
    lambda { @instance.some_value = "cat" }.should change(@instance, :some_value).to("cat").from("string")
  end

  specify "should pass when #from comes before #to" do
    lambda { @instance.some_value = "cat" }.should change(@instance, :some_value).from("string").to("cat")
  end
end

context "should change{ block }.from(old).to(new)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 'string'
  end
  
  specify "should pass when #to comes before #from" do
    lambda { @instance.some_value = "cat" }.should change{@instance.some_value}.to("cat").from("string")
  end

  specify "should pass when #from comes before #to" do
    lambda { @instance.some_value = "cat" }.should change{@instance.some_value}.from("string").to("cat")
  end
end
