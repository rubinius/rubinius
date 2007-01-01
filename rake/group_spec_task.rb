
# A generalized spectask for rubinius
class GroupSpecTask < Spec::Rake::SpecTask
  def initialize(group)
    super(group) do |t|
      t.spec_files = FileList["spec/#{group}/*_spec.rb"]
    end
  end
end

