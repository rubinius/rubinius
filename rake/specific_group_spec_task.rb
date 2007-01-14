
# A generalized spectask for rubinius
class SpecificGroupSpecTask < Spec::Rake::SpecTask
  def initialize(task, group)
    super(task) do |t|
      t.spec_files = FileList["spec/#{group}/#{task}_spec.rb"]
    end
  end
end


# A generalized spectask for rubinius
class SpecificGroupSpecTask < Spec::Rake::SpecTask
  def initialize(task, group)
    super(task) do |t|
      t.spec_files = FileList["spec/#{group}/#{task}_spec.rb"]
    end
  end
end


# A generalized spectask for rubinius
class SpecificGroupSpecTask < Spec::Rake::SpecTask
  def initialize(task, group)
    super(task) do |t|
      t.spec_files = FileList["spec/#{group}/#{task}_spec.rb"]
    end
  end
end

