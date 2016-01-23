require File.expand_path('../../../spec_helper', __FILE__)

describe "NameError#receiver" do
  it "returns the receiver of the method call where the error occurred" do
    receiver  = 'receiver'
    exception = receiver.doesnt_exist rescue $!

    exception.receiver.should equal(receiver)
  end
end
