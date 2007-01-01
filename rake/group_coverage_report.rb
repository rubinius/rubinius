
# A generalized coverage report for a spec group. Might need to remove some
# exclusions for testing some things.
class GroupCoverageReport < Spec::Rake::SpecTask
  def initialize(group, exclusions = [])
    exclusions += %w(spec)
    # non-relevant folders
    exclusions += %w(lib/bytecode lib/cpu lib/gc lib/sexp lib/translation)
    # non-relevant files
    exclusions += %w(lib/log.rb lib/machine.rb lib/memory.rb lib/object.rb
                     lib/object_memory.rb lib/platform.rb)
    exclusions.uniq!

    super(group) do |t|
      t.spec_files = FileList["spec/#{group}/*_spec.rb"]
      t.rcov = true
      t.rcov_dir = "coverage/#{group}"
      t.rcov_opts = ['--exclude', exclusions.join(',')]
    end
  end
end

