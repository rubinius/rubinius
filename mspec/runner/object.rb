class Object
  def before(at=:each, &block)
    @runner.before(at, &block)
  end

  def after(at=:each, &block)
    @runner.after(at, &block)
  end

  def describe(msg, &block)
    @runner.describe(msg, &block)
  end

  def it(msg, &block)
    @runner.it(msg, &block)
  end

  alias context describe
  alias specify it
  alias setup before
  alias teardown after
end
