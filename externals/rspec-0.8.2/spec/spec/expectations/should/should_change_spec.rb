require File.dirname(__FILE__) + '/../../../spec_helper.rb'
class SomethingExpected
  attr_accessor :some_value
end

context "should_change(actual, message)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 5
  end

  specify "should pass when actual is modified by the block" do
    lambda {@instance.some_value = 6}.should_change(@instance, :some_value)
  end

  specify "should fail when actual is not modified by the block" do
    lambda do
      lambda {}.should_change(@instance, :some_value)
    end.should_fail_with "some_value should have changed, but is still 5"
  end
end

context "should_change { block }" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 5
  end

  specify "should pass when actual is modified by the block" do
    lambda {@instance.some_value = 6}.should_change { @instance.some_value }
  end

  specify "should fail when actual is not modified by the block" do
    lambda do
      lambda {}.should_change{ @instance.some_value }
    end.should_fail_with "result should have changed, but is still 5"
  end
end

context "should_change(actual, message).by(expected)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 5
  end

  specify "should pass when attribute is changed by expected amount" do
    lambda { @instance.some_value += 1 }.should_change(@instance, :some_value).by(1)
  end

  specify "should fail when the attribute is changed by unexpected amount" do
    lambda do
      lambda { @instance.some_value += 2 }.should_change(@instance, :some_value).by(1)
    end.should_fail_with "some_value should have been changed by 1, but was changed by 2"
  end

  specify "should fail when the attribute is changed by unexpected amount in the opposite direction" do
    lambda do
      lambda { @instance.some_value -= 1 }.should_change(@instance, :some_value).by(1)
    end.should_fail_with "some_value should have been changed by 1, but was changed by -1"
  end
end

context "should_change{ block }.by(expected)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 5
  end

  specify "should pass when attribute is changed by expected amount" do
    lambda { @instance.some_value += 1 }.should_change{@instance.some_value}.by(1)
  end

  specify "should fail when the attribute is changed by unexpected amount" do
    lambda do
      lambda { @instance.some_value += 2 }.should_change{@instance.some_value}.by(1)
    end.should_fail_with "result should have been changed by 1, but was changed by 2"
  end

  specify "should fail when the attribute is changed by unexpected amount in the opposite direction" do
    lambda do
      lambda { @instance.some_value -= 1 }.should_change{@instance.some_value}.by(1)
    end.should_fail_with "result should have been changed by 1, but was changed by -1"
  end
end

context "should_change(actual, message).from(old)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 'string'
  end

  specify "should pass when attribute is == to expected value before executing block" do
    lambda { @instance.some_value = "astring" }.should_change(@instance, :some_value).from("string")
  end

  specify "should fail when attribute is not == to expected value before executing block" do
    lambda do
      lambda { @instance.some_value = "knot" }.should_change(@instance, :some_value).from("cat")
    end.should_fail_with "some_value should have initially been \"cat\", but was \"string\""
  end
end

context "should_change{ block }.from(old)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 'string'
  end

  specify "should pass when attribute is == to expected value before executing block" do
    lambda { @instance.some_value = "astring" }.should_change{@instance.some_value}.from("string")
  end

  specify "should fail when attribute is not == to expected value before executing block" do
    lambda do
      lambda { @instance.some_value = "knot" }.should_change{@instance.some_value}.from("cat")
    end.should_fail_with "result should have initially been \"cat\", but was \"string\""
  end
end

context "should_change(actual, message).to(new)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 'string'
  end
  
  specify "should pass when attribute is == to expected value after executing block" do
    lambda { @instance.some_value = "cat" }.should_change(@instance, :some_value).to("cat")
  end

  specify "should fail when attribute is not == to expected value after executing block" do
    lambda do
      lambda { @instance.some_value = "cat" }.should_change(@instance, :some_value).from("string").to("dog")
    end.should_fail_with "some_value should have been changed to \"dog\", but is now \"cat\""
  end
end

context "should_change{ block }.to(new)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 'string'
  end
  
  specify "should pass when attribute is == to expected value after executing block" do
    lambda { @instance.some_value = "cat" }.should_change{@instance.some_value}.to("cat")
  end

  specify "should fail when attribute is not == to expected value after executing block" do
    lambda do
      lambda { @instance.some_value = "cat" }.should_change{@instance.some_value}.from("string").to("dog")
    end.should_fail_with "result should have been changed to \"dog\", but is now \"cat\""
  end
end

context "should_change(actual, message).from(old).to(new)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 'string'
  end
  
  specify "should pass when #to comes before #from" do
    lambda { @instance.some_value = "cat" }.should_change(@instance, :some_value).to("cat").from("string")
  end

  specify "should pass when #from comes before #to" do
    lambda do
      lambda { @instance.some_value = "cat" }.should_change(@instance, :some_value).from("string").to("cat")
    end.should_pass
  end
end

context "should_change{ block }.from(old).to(new)" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 'string'
  end
  
  specify "should pass when #to comes before #from" do
    lambda { @instance.some_value = "cat" }.should_change{@instance.some_value}.to("cat").from("string")
  end

  specify "should pass when #from comes before #to" do
    lambda { @instance.some_value = "cat" }.should_change{@instance.some_value}.from("string").to("cat")
  end
end
