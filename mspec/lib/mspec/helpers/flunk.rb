class Object
  def flunk(msg="This example is a failure")
    Expectation.fail_with "Failed:", msg
  end
end
