require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

# Need special perms to run chroot
# describe "Dir.chroot" do
#   it 'Dir.chroot can be used to change the process\' root directory, see chroot(2)' do
#     example do
#      Kernel.fork {
#        begin
#          ret = Dir.chroot mock_dir
#          File.open('/root_contents.txt', 'wb') {|f| f.puts ret; f.puts Dir.entries('/').sort}
#          FileUtils.chmod 0777, '/root_contents.txt'
#        rescue SystemCallError
#          warn '**WARN: Insufficient permissions to test Dir.chroot! (Not a huge problem.)'
#        end
#      }
# 
#      Process.waitall
# 
#      contents = File.read "#{mock_dir}/root_contents.txt"
#      FileUtils.rm "#{mock_dir}/root_contents.txt"
# 
#      # Should have the return value + the filenames
#      contents.split("\n").sort
#     end.should == %w|0 . .. .dotfile .dotsubdir subdir_one subdir_two deeply nondotfile file_one.ext file_two.ext root_contents.txt|.sort
#   end
# end
