require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "YAML.load_file" do
  it "returns a hash" do
    File.open($test_file,'w' ){|io| YAML.dump( {"bar"=>2, "car"=>1}, io ) }
    YAML.load_file($test_file).should == {"bar"=>2, "car"=>1}
    system "rm #{$test_file}"
  end
end
