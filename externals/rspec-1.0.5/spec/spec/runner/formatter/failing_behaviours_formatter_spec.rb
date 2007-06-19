require File.dirname(__FILE__) + '/../../../spec_helper.rb'

module Spec
  module Runner
    module Formatter
      describe "FailingBehavioursFormatter" do
        before(:each) do
          @io = StringIO.new
          @formatter = FailingBehavioursFormatter.new(@io)
        end

        it "should add example name for each failure" do
          @formatter.add_behaviour("b 1")
          @formatter.example_failed("e 1", nil, Reporter::Failure.new(nil, RuntimeError.new))
          @formatter.add_behaviour("b 2")
          @formatter.example_failed("e 2", nil, Reporter::Failure.new(nil, RuntimeError.new))
          @formatter.example_failed("e 3", nil, Reporter::Failure.new(nil, RuntimeError.new))
          @io.string.should eql(<<-EOF
b 1
b 2
EOF
)
        end
      end
    end
  end
end
