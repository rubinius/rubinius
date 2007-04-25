class Guard < Channel
  alias :new! :new

  def new(&prc)
    new! { |value| Thread.new { prc.call value } }
  end

  def initialize(&prc)
    super
    # this does not work nicely in practice since the Guard cannot be
    # garbage collected while the thread runs
    Thread.new do
      loop do
        begin
          prc.call receive
        rescue Exception
        end
      end
    end
  end

  private :receive
end
