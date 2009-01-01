require "#{File.dirname __FILE__}/../../spec_helper"
require "#{File.dirname __FILE__}/shared/new_shared"

describe "IO.new" do
  it_behaves_like :io_new, :new
end
