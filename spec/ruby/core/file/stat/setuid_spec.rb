require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../../../shared/file/setuid', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "File::Stat#setuid?" do
  it_behaves_like :file_setuid, :setuid?, FileStat
end

describe "File::Stat#setuid?" do
  before(:each) do
    @name = tmp('test.txt')
    touch @name
  end

  after(:each) do
    rm_r @name
  end

  it "returns false if the file was just made" do
    st = File.stat(@name)
    st.setuid?.should == false
  end

  platform_is_not :windows do
    it "returns true when the gid bit is set" do
      system "chmod u+s #{@name}"

      st = File.stat(@name)
      st.setuid?.should == true
    end
  end

end
