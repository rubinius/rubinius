#
#   help.rb - helper using ri
#   	$Release Version: 0.9.5$
#   	$Revision: 11708 $
#   	$Date: 2007-02-12 15:01:19 -0800 (Mon, 12 Feb 2007) $
#
# --
#
#   
#

require 'rdoc/ri/driver'

module IRB
  module ExtendCommand
    module Help
      include RDoc::RI
      def self.execute(context, *names)
        names.each do |name|
          begin
            Driver.run(['-T', name.to_s])
          rescue Driver::NotFoundError
            puts $!.message
          rescue SystemExit
          end
        end
        nil
      end
    end
  end
end
