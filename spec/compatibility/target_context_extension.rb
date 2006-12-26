require 'rubygems'
require 'spec'
require 'tempfile'

MRI = '/usr/bin/env ruby'
SHOTGUN  = 'shotgun/rubinius'
RCOMPILE = 'bin/rcompile'

module TargetContextExtension
  def rubinius_target(path)
    `#{RCOMPILE} #{path}`
    eval(`#{SHOTGUN} lib/kernel.rbc #{path}.rbc`)
  end

  def mri_target(path)
    eval(`#{MRI} #{path}`)
  end

  def target_rvals_for(expressions)
    expressions.each do |exp|
      t = Tempfile.new('ruby_test_expression')
      t.write "p(#{exp})"
      t.close
      yield exp, rubinius_target(t.path), mri_target(t.path)
      t.unlink
    end
  end

  def generate_specifications(expressions)
    target_rvals_for(expressions) do |exp, rubinius, mri|
      specify "should evaluate '#{exp}' the same way as MRI" do
        rubinius.should == mri
      end
    end
  end
end

