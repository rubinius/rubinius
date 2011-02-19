class MemoryAction
  def register
    MSpec.register :finish, self
  end

  def finish
    require 'rubinius/analyst'

    puts "\nTotal memory: #{Rubinius::Analyst.new.total_memory}"
  end
end
