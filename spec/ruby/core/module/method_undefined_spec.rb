require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Module#method_undefined" do
  it "is called when a method is undefined from self" do
    begin
      Module.new do
        def self.method_undefined(name)
          $method_undefined = name
        end

        def test
          "test"
        end
        undef_method :test
      end

      $method_undefined.should == :test
    ensure
      $method_undefined = nil
    end
  end
end
