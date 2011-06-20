require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../../../shared/file/setgid', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "File::Stat#setgid?" do
  it_behaves_like :file_setgid, :setgid?, FileStat
end

describe "File::Stat#setgid?" do
  before(:each) do
    @name = tmp('test.txt')
    touch @name
  end

  after(:each) do
    rm_r @name
  end

  it "returns false if the file was just made" do
    st = File.stat(@name)
    st.setgid?.should == false
  end

  platform_is_not :windows do
    it "returns true when the gid bit is set" do
      system "chmod g+s #{@name}"

      st = File.stat(@name)
      st.setgid?.should == true
    end
  end
end
