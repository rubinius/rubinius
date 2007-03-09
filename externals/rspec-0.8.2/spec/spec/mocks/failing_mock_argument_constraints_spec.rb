require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
  module Mocks
    context "FailingMockArgumentConstraints" do
      setup do
        @mock = Mock.new("test mock", {
          :auto_verify => false
        })
        @reporter = Mock.new("reporter", {
          :null_object => true,
          :auto_verify => false
        })
      end
      
      specify "should reject goose when expecting a duck" do
        @mock.should_receive(:random_call).with(DuckTypeArgConstraint.new(:abs, :div))
        lambda { @mock.random_call("I don't respond to :abs or :div") }.should_raise(MockExpectationError)
      end
      
      specify "should reject non boolean" do
        @mock.should_receive(:random_call).with(:boolean)
        lambda do
          @mock.random_call("false")
        end.should_raise(MockExpectationError)
      end
      
      specify "should reject non numeric" do
        @mock.should_receive(:random_call).with(:numeric)
        lambda do
          @mock.random_call("1")
        end.should_raise(MockExpectationError)
      end
      
      specify "should reject non string" do
        @mock.should_receive(:random_call).with(:string)
        lambda do
          @mock.random_call(123)
        end.should_raise(MockExpectationError)
      end
      
      specify "should fail if regexp does not match submitted string" do
        @mock.should_receive(:random_call).with(/bcd/)
        lambda { @mock.random_call("abc") }.should_raise(MockExpectationError)
      end
      
      specify "should fail if regexp does not match submitted regexp" do
        @mock.should_receive(:random_call).with(/bcd/)
        lambda { @mock.random_call(/bcde/) }.should_raise(MockExpectationError)
      end
      
      specify "should fail for a hash w/ wrong values" do
        @mock.should_receive(:random_call).with(:a => "b", :c => "d")
        lambda do
          @mock.random_call(:a => "b", :c => "e")
        end.should_raise(MockExpectationError, /Mock 'test mock' expected :random_call with \(\{(:a=>\"b\", :c=>\"d\"|:c=>\"d\", :a=>\"b\")\}\) but received it with \(\{(:a=>\"b\", :c=>\"e\"|:c=>\"e\", :a=>\"b\")\}\)/)
      end
      
      specify "should fail for a hash w/ wrong keys" do
        @mock.should_receive(:random_call).with(:a => "b", :c => "d")
        lambda do
          @mock.random_call("a" => "b", "c" => "d")
        end.should_raise(MockExpectationError, /Mock 'test mock' expected :random_call with \(\{(:a=>\"b\", :c=>\"d\"|:c=>\"d\", :a=>\"b\")\}\) but received it with \(\{(\"a\"=>\"b\", \"c\"=>\"d\"|\"c\"=>\"d\", \"a\"=>\"b\")\}\)/)
      end
      
      specify "should match against a Matcher" do
        lambda do
          @mock.should_receive(:msg).with(equal(3))
          @mock.msg(37)
        end.should_raise(MockExpectationError, "Mock 'test mock' expected :msg with (equal 3) but received it with (37)")
      end
    end
  end
end
