namespace :debug do

  desc "Create a dot graph of the Rakefile's dependencies.  Use :exclude to exclude rules and dependencies."
  task :graph, :include, :exclude, :minimum do |_, args|
    begin
      $: << File.expand_path("~/Work/p4/zss/src/ZenHacks/dev/lib/")
      require 'graph'
    rescue LoadError
      abort 'install ZenHacks'
    end

    exclude = Regexp.new(args[:exclude]) if args[:exclude]
    include = Regexp.new(args[:include]) if args[:exclude]
    minimum = (args[:minimum] || 10).to_i

    graph = Graph.new

    Rake::Task.tasks.each do |task|
      next unless task.name =~ /^vm/
      next if include and task.name !~ include
      next if exclude and task.name =~ exclude

      prereqs = task.prerequisites
      prereqs = prereqs.reject do |prereq|
        prereq =~ exclude
      end if exclude

      graph[task.name] = prereqs
    end

    invert_graph = graph.invert
    invert_graph.filter_size minimum if minimum > 0
    invert_graph.prefix << 'rankdir="LR"'

    invert_graph.keys_by_count.first(10).each_with_index do |key, i|
      if i < 5 then
        invert_graph.attribs[key] << "color = red"
      else
        invert_graph.attribs[key] << "color = orange"
      end
    end

    open 'rubinius_tasks.dot', 'w' do |io| io << invert_graph end
  end

  def get_task name
    Rake::Task.tasks.find { |t| t.name == name }
  end

  desc "Display prerequisites for a task"
  task :prereqs, :task do |_, args|
    raise "supply task argument" unless args[:task]

    task = get_task args[:task]

    raise "No such task #{args[:task].inspect}" unless task

    puts task.prerequisites.join("\n")
  end

  desc "Display prerequisites for a task"
  task :all_prereqs, :task do |_, args|
    raise "supply task argument" unless args[:task]

    task = get_task args[:task]

    raise "No such task #{args[:task].inspect}" unless task

    all = task.prerequisites
    all += all.map { |n| get_task(n).prerequisites }
    all = all.flatten.uniq

    puts all.join("\n")
  end

  def print_deps(task, depth = 0, done = {})
    deps = Rake::Task.tasks.select { |t| t.prerequisites.include? task.name }
    deps.each do |dep|
      next if done[dep.name]
      done[dep.name] = true
      puts "#{'  ' * depth}#{dep.name}"
      print_deps(dep, depth+1, done)
    end
  end

  desc "Display tasks that depend on a task"
  task :dependees, :task do |_, args|
    raise "supply task argument" unless args[:task]

    task = get_task args[:task]

    raise "No such task #{args[:task].inspect}" unless task

    print_deps task
  end

end

