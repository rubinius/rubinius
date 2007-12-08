class Object
  def before(at=:each, &block)
    spec_runner.before(at, &block)
  end

  def after(at=:each, &block)
    spec_runner.after(at, &block)
  end

  def describe(mod, str, &block)
    spec_runner.describe(mod, str, &block)
  end

  def it(mod, str, &block)
    spec_runner.it(mod, str, &block)
  end
  
  def set_spec_runner(formatter=nil, out=STDOUT)
    @runner = SpecRunner.new(formatter.new(out))
  end
  
  def spec_runner
    @runner ||= SpecRunner.new
  end

  alias context describe
  alias specify it
  alias setup before
  alias teardown after
end
