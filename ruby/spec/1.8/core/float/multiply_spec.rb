require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#*" do
  it "returns self multiplied by other" do 
    (4923.98221 * 2).should be_close(9847.96442, TOLERANCE) 
    (6712.5 * 0.25).should be_close(1678.125, TOLERANCE) 
    (256.4096 * 0xffffffff).should be_close(1101270846124.03, TOLERANCE * 0xffffffff)
  end
end
