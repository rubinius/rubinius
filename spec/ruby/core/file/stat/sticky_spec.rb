require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../../../shared/file/sticky', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "File::Stat#sticky?" do
  it_behaves_like :file_sticky, :sticky?, FileStat
end

describe "File::Stat#sticky?" do
  before(:each) do
    @filename = tmp("i_exist")
    touch(@filename)
  end

  after(:each) do
    rm_r @filename
  end

  it "returns false if the file has not sticky bit set" do
    st = File.stat(@filename)
    st.sticky?.should == false
  end

  platform_is_not :windows do
    it "returns true if the file has sticky bit set" do
      system "chmod +t #{@filename}"
      st = File.stat(@filename)
      st.sticky?.should == true
    end
  end
end
