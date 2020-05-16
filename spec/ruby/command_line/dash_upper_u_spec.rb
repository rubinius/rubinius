describe "ruby -U" do
  it "warns that -U is deprecated" do
    ruby_exe('', :options => '-U', :args => '2>&1').should == \
      "The -U option is deprecated. Rubinius internal encoding is always UTF-8\n"
  end
end
