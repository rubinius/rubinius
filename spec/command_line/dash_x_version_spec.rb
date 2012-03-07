require File.expand_path('../../spec_helper', __FILE__)

describe "The -Xversion command line option" do
  ruby_version_is ""..."1.9" do
    describe "-X18" do
      it "sets RUBY_VERSION to 1.8.7 when RBXOPT specifies 1.9.3" do
        opts = { :args => %Q{-e "puts RUBY_VERSION" 2>&1}, :env => { "RBXOPT" => "-X19" }, :options => "-X18" }
        ruby_exe(nil, opts).chomp.should == "1.8.7"
      end
    end
  end

  ruby_version_is "1.9" do
    describe "-X19" do
      it "sets RUBY_VERSION to 1.9.3 when RBXOPT specifies 1.8.7" do
        opts = { :args => %Q{-e "puts RUBY_VERSION" 2>&1}, :env => { "RBXOPT" => "-X18" }, :options => "-X19" }
        ruby_exe(nil, opts).chomp.should == "1.9.3"
      end
    end
  end
end
