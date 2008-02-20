require 'tempfile'

class ArSpec

  ABC = <<-EOF
!<arch>
a               1203110500  501   20    100644  6         `
apple
b               1203110500  501   20    100644  8         `
balloon
c               1203110500  501   20    100644  7         `
crayon

  EOF

  LONG_NAME = <<-EOF
!<arch>
#1/24           1203110500  501   20    100644  55        `
really_long_file_name\000\000\000really long file name contents

#1/20           1203110500  501   20    100644  48        `
12345678901234567\000\000\000one two three ... seventeen
#1/20           1203110500  501   20    100644  45        `
12345678901234567890one two three ... twenty

  EOF

  def initialize(meth, &its)
    @tempfiles = []

    name = "Ar#{meth}"

    describe name do
      before :each do
        @ar_spec = self
      end

      after :each do
        cleanup
      end

      instance_eval(&its)
    end
  end

  def cleanup
    @tempfiles.each do |t| t.close end
  end

  def write_ar(archive)
    ar = Tempfile.new "ar_spec_#{$$}"

    @tempfiles << ar

    ar.write archive
    ar.rewind
    
    ar
  end

end

