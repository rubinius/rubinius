describe 'The -K command line option sets __ENCODING__' do
  it "warns that -K is deprecated" do
    ruby_exe('', :options => '-K', :args => '2>&1').should == \
      "The -K option is deprecated. Rubinius internal encoding is always UTF-8\n"
  end
end
