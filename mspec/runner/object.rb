class Object
  def before(at=:each, &block)
    spec_runner.before(at, &block)
  end

  def after(at=:each, &block)
    spec_runner.after(at, &block)
  end

  def describe(*args, &block)
    spec_runner.describe(*args, &block)
  end

  def it(msg, &block)
    spec_runner.it(msg, &block)
  end
  
  def set_spec_runner(formatter=nil, out=STDOUT)
    $mspec_runner = SpecRunner.new(formatter.new(out))
  end
  
  def spec_runner
    $mspec_runner ||= SpecRunner.new
  end

  alias context describe
  alias specify it
  alias setup before
  alias teardown after
end
