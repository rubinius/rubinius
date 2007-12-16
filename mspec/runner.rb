require 'mspec/mocks'
require 'mspec/runner/runner'
require 'mspec/runner/object'
require 'mspec/runner/formatters'
require 'mspec/runner/guards'
require 'mspec/runner/shared'

class Object
  def set_spec_runner(formatter=nil, out=STDOUT)
    $mspec_runner = SpecRunner.new(formatter.new(out))
  end

  def spec_runner
    $mspec_runner ||= SpecRunner.new
  end
end
