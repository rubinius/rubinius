namespace :debug do

  desc "Create a dot graph of the Rakefile's dependencies.  Use :exclude to exclude rules and dependencies."
  task :graph, :exclude, :filter do |_, args|
    begin
      require 'graph'
    rescue LoadError
      abort 'install ZenHacks'
    end

    exclude = /#{args[:exclude]}/ if args[:exclude]

    graph = Graph.new
    graph.prefix << 'rankdir="LR"'

    Rake::Task.tasks.each do |task|
      next if task.name =~ /^stats|^vm\/.depends.mf$/ # too many
      next if exclude and task.name =~ exclude

      prereqs = task.prerequisites
      prereqs = prereqs.reject do |prereq|
        prereq =~ exclude
      end if exclude

      graph[task.name] = prereqs
    end

    open 'rubinius_tasks.dot', 'w' do |io| io << graph end
  end

  desc "Display prerequisites for a task"
  task :prereqs, :task do |_, args|
    raise "supply task argument" unless args[:task]

    task = Rake::Task.tasks.find { |t| t.name == args[:task] }

    raise "No such task #{args[:task].inspect}" unless task

    puts task.prerequisites.join(', ')
  end

  desc "Display tasks that depend on a task"
  task :dependees, :task do |_, args|
    raise "supply task argument" unless args[:task]

    task = Rake::Task.tasks.find { |t| t.name == args[:task] }

    raise "No such task #{args[:task].inspect}" unless task

    tasks = Rake::Task.tasks.select { |t| t.prerequisites.include? task.name }

    puts tasks.join(', ')
  end

end

