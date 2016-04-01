# -*- mode: ruby -*-

#$dev_mode = true

DIR = File.dirname task.application.rakefile
$: << File.join(DIR, "gen/lib")

require 'rbe/cpp'
require 'rbe/loader'
require 'rbe/gen/generator'

OUT_DIR = File.join(DIR, "build_src")
SRC_DIR = File.join(DIR, "src")

directory OUT_DIR

CLASS_LIST = RBe::Cpp::ClassList.new

if $dev_mode
  LIBS = FileList.new(File.join(DIR, "lib", "**", "*.{rb,erb}"))
else
  LIBS = []
end

DEFS = FileList.new(File.join(DIR, "defs/*.def"))

task :update_hpp
task :update_cpp
task :copy_files

DEFS.each do |name|
  c = RBe::Loader.load_file name
  CLASS_LIST.add c
end

task :populate_copy_tasks do |t|
  srcs = FileList.new(File.join(SRC_DIR, "**/*.{c,cpp,h,hpp}"))
  srcs.each do |src|
    name = File.basename src
    dst = File.join(OUT_DIR, name)
    file dst => [src, OUT_DIR] do |u|
      cp u.prerequisites[0], u.name
    end
    task :copy_files => dst
  end
end

task :populate_class_init_file_task do |t|
  class_init_filename = File.join(OUT_DIR, "_class_init.cpp")
  file class_init_filename => LIBS + [OUT_DIR] + DEFS do |t|
    puts "generating #{class_init_filename}"
    File.open t.name, "w" do |f|
      f.write RBe::Gen::Generator.generate(:init_classes_impl, CLASS_LIST.list)
    end
  end
  task :update_cpp => class_init_filename
end

[
  [:populate_cpp_file_tasks, :class_impl,   :update_cpp, ".cpp"],
  [:populate_hpp_file_tasks, :class_header, :update_hpp, ".hpp"]
].each do |pop_task_name, template, task_name, ext|
  task pop_task_name do |t|
    DEFS.each do |name|
      filename = File.join(OUT_DIR, File.basename(name)).sub(/\.def$/, ext)
      file filename => [name, OUT_DIR] + LIBS do |u|
        print "generating #{filename}"
        class_name = 'B' + File.basename(u.name).sub(/\..*$/, '')
        c = CLASS_LIST[class_name]
        raise "class #{class_name} not found" unless c
        tmpname = u.name + ".tmp"
        File.open tmpname, "w" do |f|
          f.write RBe::Gen::Generator.generate(template, c)
        end
        if !File.exist? u.name
          File.rename tmpname, u.name
        else
          diffq = `diff -q #{u.name} #{tmpname}`
          if diffq.empty?
            print " ... unchanged"
            File.unlink tmpname
          else
            File.unlink u.name
            File.rename tmpname, u.name
          end
        end
        puts
      end
      task task_name => filename
    end
  end
end

task :populate_file_tasks => [:populate_cpp_file_tasks, :populate_hpp_file_tasks,
                              :populate_class_init_file_task]
task :populate_tasks => [:populate_file_tasks, :populate_copy_tasks]
task :default => [:populate_tasks, :update_hpp, :update_cpp, :copy_files]
