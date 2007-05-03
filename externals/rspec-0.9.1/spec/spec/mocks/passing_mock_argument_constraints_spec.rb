require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
  module Mocks
    describe "a mock, in handling arguments" do
      before(:each) do
        @mock = Mock.new("test mock")
      end
      
      after(:each) do
        @mock.rspec_verify
      end

      it "should accept true as boolean" do
        @mock.should_receive(:random_call).with(:boolean)
        @mock.random_call(true)
      end
      
      it "should accept false as boolean" do
        @mock.should_receive(:random_call).with(:boolean)
        @mock.random_call(false)
      end

      it "should accept fixnum as numeric" do
        @mock.should_receive(:random_call).with(:numeric)
        @mock.random_call(1)
      end

      it "should accept float as numeric" do
        @mock.should_receive(:random_call).with(:numeric)
        @mock.random_call(1.5)
      end

      it "should accept string as anything" do
        @mock.should_receive(:random_call).with("a", :anything, "c")
        @mock.random_call("a", "whatever", "c")
      end

      it "should match duck type with one method" do
        @mock.should_receive(:random_call).with(DuckTypeArgConstraint.new(:length))
        @mock.random_call([])
      end

      it "should match duck type with two methods" do
        @mock.should_receive(:random_call).with(DuckTypeArgConstraint.new(:abs, :div))
        @mock.random_call(1)
      end

      it "should match non special symbol" do
        @mock.should_receive(:random_call).with(:some_symbol)
        @mock.random_call(:some_symbol)
      end

      it "should match string" do
        @mock.should_receive(:random_call).with(:string)
        @mock.random_call("a string")
      end

      it "should match string against regexp" do
        @mock.should_receive(:random_call).with(/bcd/)
        @mock.random_call("abcde")
      end

      it "should match regexp against regexp" do
        @mock.should_receive(:random_call).with(/bcd/)
        @mock.random_call(/bcd/)
      end
      
      it "should match against a hash submitted and received by value" do
        @mock.should_receive(:random_call).with(:a => "a", :b => "b")
        @mock.random_call(:a => "a", :b => "b")
      end
      
      it "should match against a hash submitted by reference and received by value" do
        opts = {:a => "a", :b => "b"}
        @mock.should_receive(:random_call).with(opts)
        @mock.random_call(:a => "a", :b => "b")
      end
      
      it "should match against a hash submitted by value and received by reference" do
        opts = {:a => "a", :b => "b"}
        @mock.should_receive(:random_call).with(:a => "a", :b => "b")
        @mock.random_call(opts)
      end
      
      it "should match against a Matcher" do
        @mock.should_receive(:msg).with(equal(37))
        @mock.msg(37)
      end
    end
  end
end
