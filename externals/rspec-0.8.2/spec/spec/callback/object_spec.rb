require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "An Object" do
  setup do
    @callback_object = Object.new
  end

  specify "should be defined as a block and notified" do
    specify_registration_and_notification do |callback_key, expected_callback|
      @callback_object.register_callback(callback_key, &expected_callback)
    end
  end

  specify "should be defined as a proc and notified" do
    specify_registration_and_notification do |callback_key, expected_callback|
      @callback_object.register_callback(callback_key, expected_callback)
    end
  end

  specify "define should require callback with the call method" do
    proc do
      specify_registration_and_notification do |callback_key, expected_callback|
        @callback_object.register_callback(callback_key)
      end
    end.should_raise(RuntimeError, "You must define the callback that accepts the call method.")
  end

  specify "should undefine a proc" do
    specify_unregistration do |callback_key, callback|
      @callback_object.unregister_callback callback_key, callback
    end
  end

  specify "undefine should require callback with the call method" do
    proc do
      specify_unregistration do |callback_key, expected_callback|
        @callback_object.unregister_callback(callback_key, nil)
      end
    end.should_raise(RuntimeError, "You may only undefine callbacks that use the call method.")
  end
  
  specify "should handle errors" do
    @callback_object.register_callback(:error_callback) do
      raise "First Error"
    end

    @callback_object.register_callback(:error_callback) do
      raise "Second Error"
    end

    error_messages = []
    @callback_object.notify_callbacks(:error_callback) do |e|
      error_messages << e.message
    end

    error_messages.should_eql ["First Error", "Second Error"]
  end

  specify "should not fail with nothing to notify" do
    @callback_object.notify_callbacks(:foobar, :argument)
  end

  protected
  def specify_registration_and_notification
    callback_key = :create
    passed_callback_argument = nil
    expected_callback = proc do |callback_argument|
      passed_callback_argument = callback_argument
    end

    returned_callback = yield(callback_key, expected_callback)
    returned_callback.should_equal expected_callback

    expected_argument = Object.new
    @callback_object.notify_callbacks(callback_key, expected_argument)
    passed_callback_argument.should_equal expected_argument
  end

  def specify_unregistration
    callback_key = :create
    callback_called = false
    callback = @callback_object.register_callback(callback_key) do
      callback_called = true
    end

    yield(callback_key, callback)
    @callback_object.notify_callbacks(callback_key)
    callback_called.should_equal false
  end
end
