describe "ruby -E" do
  it "raises a RuntimeError if used with -U" do
    ruby_exe('', :options => '-Eascii:ascii -U', :args => '2>&1').should == \
      "The -E option setting internal encoding is deprecated. Rubinius internal encoding is always UTF-8\nThe -U option is deprecated. Rubinius internal encoding is always UTF-8\n"
  end
end
