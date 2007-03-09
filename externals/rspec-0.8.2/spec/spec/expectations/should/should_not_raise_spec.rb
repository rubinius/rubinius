require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_not_raise" do
  specify "should fail when specific exception is raised" do
    lambda {
      lambda do
        "".nonexistent_method
      end.should_not_raise(NoMethodError)
    }.should_fail_with /expected no NoMethodError, got #<NoMethodError: undefined method/
  end

  specify "should fail when exact exception is raised with message" do
    lambda do
      lambda do
        raise(StandardError.new("abc"))
      end.should_not_raise(StandardError, "abc")
    end.should_fail_with "expected no StandardError with \"abc\", got #<StandardError: abc>"
  end

  specify "should fail when exact exception is raised with message matching regexp" do
    lambda do
      lambda do
        raise(StandardError.new("abc"))
      end.should_not_raise(StandardError, /^a.c$/)
    end.should_fail_with "expected no StandardError with message matching /^a.c$/, got #<StandardError: abc>"
  end

  specify "should include actual error in failure message" do
    lambda {
      lambda {
        "".nonexistent_method
      }.should_not_raise(Exception)
    }.should_fail_with /expected no Exception, got #<NoMethodError: undefined method/
  end

  specify "should pass when exact exception is raised with wrong message" do
    lambda do
      lambda do
        raise(StandardError.new("abc"))
      end.should_not_raise(StandardError, "xyz")
    end.should_pass
  end

  specify "should pass when exact exception is raised with message not matching regexp" do
    lambda do
      lambda do
        raise(StandardError.new("abc"))
      end.should_not_raise(StandardError, /xyz/)
    end.should_pass
  end

  specify "should pass when no exception is raised" do
    lambda do
      lambda do
        "".to_s
      end.should_not_raise(NoMethodError)
    end.should_pass
  end

  specify "should pass when other exception is raised" do
    lambda do
      lambda do
        "".nonexistent_method
      end.should_not_raise(SyntaxError)
    end.should_pass
  end

  specify "without exception should pass when no exception is raised" do
    lambda do
      lambda do
        "".to_s
      end.should_not_raise
    end.should_pass
  end
end
