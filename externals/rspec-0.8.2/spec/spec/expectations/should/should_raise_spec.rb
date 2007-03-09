require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_raise" do
  specify "should fail when exact exception is raised with wrong message" do
    lambda do
      lambda do
        raise(StandardError.new("chunky bacon"))
      end.should_raise(StandardError, "rotten tomatoes")
    end.should_raise(Spec::Expectations::ExpectationNotMetError)
  end

  specify "should fail when exact exception is raised with message not matching regexp" do
    lambda do
      lambda do
        raise(StandardError.new("chunky bacon"))
      end.should_raise(StandardError, /lean/)
    end.should_raise(Spec::Expectations::ExpectationNotMetError)
  end

  specify "should fail when no exception is raised" do
    lambda do
      lambda {}.should_raise(SyntaxError)
    end.should_raise(Spec::Expectations::ExpectationNotMetError, "expected SyntaxError but nothing was raised")
  end

  specify "should fail when wrong exception is raised" do
    lambda do
      lambda do
        "".nonexistent_method
      end.should_raise(SyntaxError)
    end.should_raise(Spec::Expectations::ExpectationNotMetError, /expected SyntaxError, got #<NoMethodError: undefined method/)
  end

  specify "should pass when exact exception is raised" do
    lambda do
      lambda do
        "".nonexistent_method
      end.should_raise(NoMethodError)
    end.should_not_raise
  end

  specify "should pass when exact exception is raised with message" do
    lambda do
      lambda do
        raise(StandardError.new("this is standard"))
      end.should_raise(StandardError, "this is standard")
    end.should_not_raise
  end

  specify "should pass when exact exception is raised with message matching regexp" do
    lambda do
      lambda do
        raise(StandardError.new("this is standard"))
      end.should_raise(StandardError, /standard$/)
    end.should_not_raise
  end

  specify "should pass when subclass exception is raised" do
    lambda do
      lambda do
        "".nonexistent_method
      end.should_raise
    end.should_not_raise
  end
  
end
