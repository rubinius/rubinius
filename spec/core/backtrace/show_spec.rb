require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Backtrace" do
  it "returns a string if file paths exceed terminal width and show_color is false" do
    locs = Rubinius::VM.backtrace(1)

    # don't depend on the location of this file
    locs.each_with_index do |loc, idx|
      loc.stub!(:position).and_return(idx.to_s + "a"*90 + ".rb")
    end

    bt = Rubinius::Backtrace.new(locs, 65, 20)

    bt.show("\n", false).should be_kind_of(String)
  end
end
