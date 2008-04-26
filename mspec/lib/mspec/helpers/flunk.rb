class Object
  def flunk
    Expectation.fail_with "This example", "is a failure"
  end
end
